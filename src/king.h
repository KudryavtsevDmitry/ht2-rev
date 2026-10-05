// king.h - declarations shared by the C++ code that is linked into king.exe
// together with the reassembled king.masm.
//
// How the two halves reach each other:
//
//  * C++ -> asm. A procedure or variable of king.masm is bound to a C++ name
//    with /alternatename (x86 C names carry a leading underscore, the masm
//    labels don't). PROC labels are public already; data labels must be in
//    the PUBLIC list at the top of king.masm.
//      ASM_PROC(label)            procedure, called through CallC/CallStd/
//                                 CallThis (C++ can't declare free thiscall
//                                 functions, so all of them go this way)
//      ASM_VAR(type, name, label) variable
//
//  * asm -> C++. A rewritten thiscall method is exported as an extern "C"
//    __fastcall function with an unused second parameter: fastcall passes it
//    in EDX, so for the callee fastcall(this, edx, args) is exactly
//    thiscall(this, args). king.masm assembles the original PROC only with
//    ASM_MAINLOOP defined; otherwise its label is pointed at the export, e.g.
//    $L_5dd440 TEXTEQU <@CGameApp_OnIdle@12>
//
// Comments: +N is a byte offset into an object, vtbl+N a byte offset into
// its vtable, [FUN_xxxxxx] / [$L_xxxxxx] a label in king.masm.
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stddef.h>

#define KING_ALIAS(cname, label) __pragma(comment(linker, "/alternatename:" cname "=" label))
#define ASM_PROC(label) extern "C" void label(); KING_ALIAS("_" #label, #label)
#define ASM_VAR(type, name, label) extern "C" type name; KING_ALIAS("_" #name, #label)

#define CHECK_OFFSET(type, field, offset) \
    static_assert(offsetof(type, field) == offset, #type "::" #field " is not at +" #offset)

template <class R = void, class... A>
__forceinline R CallC(void (*fn)(), A... args)
{
    return ((R(__cdecl*)(A...))fn)(args...);
}

template <class R = void, class... A>
__forceinline R CallStd(void (*fn)(), A... args)
{
    return ((R(__stdcall*)(A...))fn)(args...);
}

template <class R = void, class... A>
__forceinline R CallThis(void (*fn)(), const void* self, A... args)
{
    return ((R(__thiscall*)(const void*, A...))fn)(self, args...);
}

// Calls the function at vtbl+vtblOffset of self, so overrides keep working.
template <class R = void, class... A>
__forceinline R CallVirt(const void* self, unsigned vtblOffset, A... args)
{
    void* fn = *(void**)(*(char* const*)self + vtblOffset);
    return ((R(__thiscall*)(const void*, A...))fn)(self, args...);
}

// ---------------------------------------------------------------------------
// Procedures of king.masm

ASM_PROC(FUN_6037e0)    // Timer_Now
ASM_PROC(FUN_5aca90)    // Input_GetEvent
ASM_PROC(FUN_4e12b0)    // Joy_ForceFeedback
ASM_PROC(FUN_5b6180)    // Gfx_BeginScene
ASM_PROC(FUN_5b6190)    // Gfx_EndScene
ASM_PROC(FUN_5b7ce0)    // Gfx_HLine
ASM_PROC(FUN_5b7f20)    // Gfx_VLine
ASM_PROC(FUN_5b8980)    // Gfx_DrawImage
ASM_PROC(FUN_5b6bf0)    // CSurface::Prepare
ASM_PROC(FUN_5b6500)    // CSurface::ScreenToClient
ASM_PROC(FUN_5d8630)    // CWindow::Clear
ASM_PROC(FUN_5d88c0)    // CWindow::Present
ASM_PROC(FUN_5d3460)    // object container: delete all
ASM_PROC(FUN_5dba30)    // CGameApp::DestroyWindowNow
ASM_PROC(FUN_5e1b30)    // CGameApp::SetVideoMode
ASM_PROC(FUN_5e1c90)    // CGameApp::WindowFromPoint
ASM_PROC(FUN_63274d)    // CWinApp::OnIdle (MFC)
ASM_PROC(FUN_639967)    // AfxOleGetUserCtrl (MFC)
ASM_PROC(FUN_63b4d3)    // AfxPostQuitMessage (MFC)
ASM_PROC(FUN_5279e0)    // King_PlayMovie
ASM_PROC(FUN_511820)    // King_LeaveGame
ASM_PROC(FUN_512720)

// Milliseconds: QueryPerformanceCounter, scaled against timeGetTime once at
// startup [FUN_603f30].
inline double Timer_Now() { return CallC<double>(FUN_6037e0); }

// Input events are queued by the window procedure and consumed once per frame.
struct InputEvent {             // slot of the 100-entry ring buffer [$L_702660]
    short msg;                  // WM_xxx
    short x, y;                 // mouse position, screen then window coordinates
    short key;                  // virtual key of keyboard messages
};

// Takes the oldest event; returns TRUE if the queue was empty.
inline BOOL Input_GetEvent(InputEvent* ev) { return CallC<BOOL>(FUN_5aca90, ev); }

// DirectInput force feedback: DISFFC_CONTINUE when on, DISFFC_PAUSE when off.
inline void Joy_ForceFeedback(BOOL on) { CallC(FUN_4e12b0, on); }

// ---------------------------------------------------------------------------
// Drawing

struct CSurface {               // DirectDraw/Direct3D backed drawing surface
    WORD width, height;         // +0

    // Restores lost DirectDraw surfaces and makes this surface the draw
    // target (Direct3D viewport or locked buffer).
    void Prepare() { CallThis(FUN_5b6bf0, this); }
    // Screen coordinates -> coordinates relative to this (sub)surface.
    void ScreenToClient(short* x, short* y) { CallThis(FUN_5b6500, this, x, y); }
};

// Direct3D BeginScene/EndScene around primitive drawing.
inline void Gfx_BeginScene() { CallC(FUN_5b6180); }
inline void Gfx_EndScene() { CallC(FUN_5b6190); }
inline void Gfx_HLine(CSurface* s, int y, int x0, int x1, int color, int flags)
{
    CallC(FUN_5b7ce0, s, y, x0, x1, color, flags);
}
inline void Gfx_VLine(CSurface* s, int x, int y0, int y1, int color, int flags)
{
    CallC(FUN_5b7f20, s, x, y0, y1, color, flags);
}
inline void Gfx_DrawImage(CSurface* s, int x, int y, void* image, int flags)
{
    CallC(FUN_5b8980, s, x, y, image, flags);
}

// ---------------------------------------------------------------------------
// Windows of the engine (not Win32 windows: rectangles of the game screen)

enum {
    WND_SURFACE = 0x01,         // has its own surface, Clear/Present work on it
    WND_VISIBLE = 0x02,         // executed every frame and hit-tested
    WND_CURSOR  = 0x04,         // shows the mouse cursor while it has the input
};

struct Cursor {
    void* image;
    int   hotX, hotY;
};

struct CWindow {
    void**    vtbl;
    int       m_4;
    CWindow*  m_pNext;          // +8   next in CGameApp::m_pWindows
    CSurface* m_pSurface;       // +12
    DWORD     m_flags;          // +16  WND_*
    int       m_20;
    double    m_totalTime;      // +24  timings of the last frame, ms
    double    m_time32;         // +32
    double    m_prepareTime;    // +40
    int       m_48;
    Cursor*   m_pCursor;        // +52  NULL: crosshair

    void v_Execute() { CallVirt(this, 48); }             // per-frame work
    int  v_OnInput(InputEvent* ev) { return CallVirt<int>(this, 52, ev); }
    void v_Prepare() { CallVirt(this, 60); }             // m_pSurface->Prepare()

    void Clear(int color) { CallThis(FUN_5d8630, this, color); }  // color < 0: no-op
    void Present() { CallThis(FUN_5d88c0, this); }
};
CHECK_OFFSET(CWindow, m_flags, 16);
CHECK_OFFSET(CWindow, m_totalTime, 24);
CHECK_OFFSET(CWindow, m_prepareTime, 40);
CHECK_OFFSET(CWindow, m_pCursor, 52);

// The game screen: CGameApp::m_pMainView, [$L_696ca0] in King.
struct CMainView : CWindow {
    BYTE m_56[2992 - 56];
    BYTE m_objects[32];         // +2992 object container
    int  m_3024;
    int  m_3028;
    int  m_3032;
    BOOL m_bExit;               // +3036 request to leave the game screen

    void v_140() { CallVirt(this, 140); }                // King [$L_4e06f0]
    void DeleteObjects() { CallThis(FUN_5d3460, m_objects); }
};
CHECK_OFFSET(CMainView, m_objects, 2992);
CHECK_OFFSET(CMainView, m_bExit, 3036);

// ---------------------------------------------------------------------------
// Application

inline BOOL AfxOleGetUserCtrl() { return CallStd<BOOL>(FUN_639967); }
inline void AfxPostQuitMessage(int exitCode) { CallStd(FUN_63b4d3, exitCode); }

struct CWinApp {                // MFC 4.2, statically linked
    void**  vtbl;
    BYTE    m_4[24];            // CCmdTarget
    void*   m_pMainWnd;         // +28  CWinThread::m_pMainWnd
    BYTE    m_32[16];
    MSG     m_msgCur;           // +48  CWinThread::m_msgCur
    BYTE    m_76[116];          // rest of CWinThread and CWinApp

    BOOL v_PumpMessage() { return CallVirt<BOOL>(this, 92); }
    BOOL v_OnIdle(LONG count) { return CallVirt<BOOL>(this, 96, count); }
    BOOL v_IsIdleMessage(MSG* msg) { return CallVirt<BOOL>(this, 100, msg); }
    int  v_ExitInstance() { return CallVirt<int>(this, 104); }

    BOOL OnIdle(LONG count) { return CallThis<BOOL>(FUN_63274d, this, count); }
    int  Run();                 // mainloop.cpp
};
CHECK_OFFSET(CWinApp, m_pMainWnd, 28);
CHECK_OFFSET(CWinApp, m_msgCur, 48);
static_assert(sizeof(CWinApp) == 192, "CWinApp size");

enum {
    APP_SMOOTH_MOUSE  = 0x04,   // average each mouse position with the previous one
    APP_MOUSE_CAPTURE = 0x08,   // mouse events go to m_pInputWnd, not the window under the mouse
    APP_NO_PRESENT    = 0x20,   // neither the cursor nor the frame reach the screen
};

// Engine application class, vtable [$L_652664]. The game's class derives
// from it through one more class (vtable [$L_65195c]).
struct CGameApp : CWinApp {
    int        m_delWrite;      // +192 ring buffer of windows deleted at the end of the frame
    int        m_delRead;       // +196
    CWindow*   m_delQueue[10];  // +200
    CWindow*   m_pWindows;      // +240 all windows, in execution order
    CWindow*   m_pHitList;      // +244 windows in hit-test order
    CWindow*   m_pInputWnd;     // +248 window with the input focus
    CWindow*   m_pScreen;       // +252 owner of the primary surface
    int        m_width;         // +256 video mode
    int        m_height;        // +260
    int        m_264;
    DWORD      m_modeFlags;     // +268
    void*      m_272;
    int        m_276;
    char*      m_iniFile;       // +280
    int        m_reqWidth;      // +284 requested video mode, set at the end of the frame
    int        m_reqHeight;     // +288
    DWORD      m_flags;         // +292 APP_*
    int        m_clearColor;    // +296 < 0: the screen is not cleared
    int        m_300;
    int        m_mouseX;        // +304 screen coordinates
    int        m_mouseY;        // +308
    int        m_crossSize;     // +312 half size of the crosshair cursor
    BOOL       m_bHoldInput;    // +316 don't take events from the queue
    int        m_320;
    CMainView* m_pMainView;     // +324
    BOOL       m_bRunFrames;    // +328 OnIdle runs frames; FALSE while one runs
    BOOL       m_bPaused;       // +332 Pause key
    int        m_336;
    BOOL       m_bQuit;         // +340
    int        m_344;
    int        m_348;
    BOOL       m_352;           // send WM_KEYDOWN to vtbl+56 of the input window
    int        m_356;
    double     m_pauseTime;     // +360 paused: start time; unpaused: length, for one frame
    int        m_368;
    int        m_372;
    double     m_lastInputTime; // +376

    void v_DestroyAllWindows() { CallVirt(this, 160); }
    void v_RunFrame() { CallVirt(this, 164); }
    void v_Frame() { CallVirt(this, 168); }
    void v_PreExecute() { CallVirt(this, 172); }         // before the windows execute
    void v_PostExecute() { CallVirt(this, 176); }        // after them, overlays
    void v_RequestQuit() { CallVirt(this, 180); }        // Alt+F4
    void v_OnPausedKey(int key) { CallVirt(this, 188, key); }
    void v_DispatchInput(InputEvent* ev) { CallVirt(this, 192, ev); }  // to m_pInputWnd

    void DestroyWindowNow(CWindow* w) { CallThis(FUN_5dba30, this, w); }
    void SetVideoMode(int width, int height, int a3, DWORD flags)
    {
        CallThis(FUN_5e1b30, this, width, height, a3, flags);
    }
    CWindow* WindowFromPoint(int x, int y) { return CallThis<CWindow*>(FUN_5e1c90, this, x, y); }

    // mainloop.cpp
    BOOL OnIdle(LONG count);
    void Frame();
    void RunFrame();
    void ProcessInput();
    void DrawCursor();
};
CHECK_OFFSET(CGameApp, m_delWrite, 192);
CHECK_OFFSET(CGameApp, m_pWindows, 240);
CHECK_OFFSET(CGameApp, m_pScreen, 252);
CHECK_OFFSET(CGameApp, m_flags, 292);
CHECK_OFFSET(CGameApp, m_mouseX, 304);
CHECK_OFFSET(CGameApp, m_pMainView, 324);
CHECK_OFFSET(CGameApp, m_bQuit, 340);
CHECK_OFFSET(CGameApp, m_pauseTime, 360);
CHECK_OFFSET(CGameApp, m_lastInputTime, 376);

// The game's application class, vtable [$L_64d830], object [$L_696750].
struct CKingApp : CGameApp {
    void Frame();
};

// ---------------------------------------------------------------------------
// Variables of king.masm

// state.dump, a crash trace written when [ENV] taskdump is set in truck.ini
struct StateDump {
    HANDLE hFile;
    HANDLE hMapping;
    int    size;
    char*  data;                // mapped view
    int    pos;
};

ASM_VAR(HWND, g_hWnd, $L_720920)
ASM_VAR(BOOL, g_bNoFrame, $L_721774)           // last OnIdle call ran no frame
ASM_VAR(DWORD, g_renderFlags, $L_702e8c)
ASM_VAR(BOOL, g_bTaskDump, $L_721768)
ASM_VAR(StateDump, g_stateDump, $L_71e808)
ASM_VAR(int, g_testInfoCount, $L_720720)       // printed as "test info: %d"
ASM_VAR(float, g_clearTime, $L_721748)         // frame start until the screen is cleared
ASM_VAR(float, g_presentTime, $L_72174c)

// King
struct CWnd6cec6c : CWindow {
    BYTE m_56[1300 - 56];
    BOOL m_1300;
    int  m_1304;
    int  m_1308;
    int  m_1312;
    BOOL m_1316;
};
CHECK_OFFSET(CWnd6cec6c, m_1316, 1316);

ASM_VAR(CMainView*, g_pMainView, $L_696ca0)
ASM_VAR(CWnd6cec6c*, g_pWnd6cec6c, $L_6cec6c)
ASM_VAR(void*, g_p6d2078, $L_6d2078)
ASM_VAR(BOOL, g_bPlayTrirrMovie, $L_6f3428)

// Plays name from the [INSTALL] movie directory of truck.ini if the file exists.
inline BOOL King_PlayMovie(const char* name, int mode) { return CallC<BOOL>(FUN_5279e0, name, mode); }
// King's reaction to CMainView::m_bExit: blanks the screen and shuts the game
// screen's subsystems down.
inline void King_LeaveGame() { CallC(FUN_511820); }
