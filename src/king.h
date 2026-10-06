// king.h - declarations shared by the C++ code that is linked into king.exe
// together with the reassembled king.masm.
//
// How the two halves reach each other:
//
//  * C++ -> asm. A procedure or variable of king.masm is bound to a C++ name
//    with /alternatename (x86 C names carry a leading underscore, the masm
//    labels don't). PROC labels are public already; data labels and labels
//    inside a PROC must be in the PUBLIC list at the top of king.masm.
//      ASM_PROC(label)            procedure, called through CallC/CallStd/
//                                 CallThis (C++ can't declare free thiscall
//                                 functions, so all of them go this way)
//      ASM_FUNC(name, label)      the same under a readable name
//      ASM_VAR(type, name, label) variable
//
//  * asm -> C++. A rewritten thiscall method is exported as an extern "C"
//    __fastcall function with an unused second parameter: fastcall passes it
//    in EDX, so for the callee fastcall(this, edx, args) is exactly
//    thiscall(this, args). king.masm assembles the original PROC only with
//    the switch of its group (ASM_MAINLOOP, ASM_GAMEAPP, ...) defined;
//    otherwise its label is pointed at the export, e.g.
//    $L_5dd440 TEXTEQU <@CGameApp_OnIdle@12>
//    The C++ code calls rewritten functions directly, never through their
//    labels: a label doesn't exist in the builds where its function is C++.
//
// Comments: +N is a byte offset into an object, vtbl+N a byte offset into
// its vtable, [FUN_xxxxxx] / [$L_xxxxxx] a label in king.masm.
#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stddef.h>

#define KING_ALIAS(cname, label) __pragma(comment(linker, "/alternatename:" cname "=" label))
#define ASM_PROC(label) extern "C" void label(); KING_ALIAS("_" #label, #label)
#define ASM_FUNC(name, label) extern "C" void name(); KING_ALIAS("_" #name, #label)
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
// The game's C runtime (VC6 LIBCMT, named in symbols.csv). Declared by hand:
// the SDK headers define some of these inline or aren't included.

// The C++ code is compiled for the x87 FPU (/arch:IA32); it converts floating
// point to integers through __ftol2 / __ftol2_sse, VC6 code through __ftol,
// which has the same contract (ST(0) truncated into EDX:EAX, popped).
KING_ALIAS("__ftol2", "__ftol")
KING_ALIAS("__ftol2_sse", "__ftol")

extern "C" {
typedef struct _iobuf FILE;
int __cdecl sprintf(char* buffer, const char* format, ...);
FILE* __cdecl fopen(const char* name, const char* mode);
int __cdecl fclose(FILE* stream);
int __cdecl _mkdir(const char* dir);
size_t __cdecl strlen(const char* s);
char* __cdecl strcpy(char* dest, const char* src);
int __cdecl strcmp(const char* a, const char* b);
}

// ---------------------------------------------------------------------------
// Procedures of king.masm

ASM_PROC(FUN_6037e0)    // Timer_Now
ASM_PROC(FUN_4e12b0)    // Joy_ForceFeedback
ASM_PROC(FUN_5b6180)    // Gfx_BeginScene
ASM_PROC(FUN_5b6190)    // Gfx_EndScene
ASM_PROC(FUN_5b7ce0)    // Gfx_HLine
ASM_PROC(FUN_5b7f20)    // Gfx_VLine
ASM_PROC(FUN_5b8980)    // Gfx_DrawImage
ASM_PROC(FUN_5ea170)    // Gfx_DrawText
ASM_PROC(FUN_5e9a10)    // Gfx_SaveTga
ASM_PROC(FUN_5e9f00)    // Gfx_SaveScreenBmp
ASM_PROC(FUN_5b6bf0)    // CSurface::Prepare
ASM_PROC(FUN_5b6500)    // CSurface::ScreenToClient
ASM_PROC(FUN_5b6710)    // CSurface::GetRect
ASM_PROC(FUN_5b64c0)    // CSurface::GetState84
ASM_PROC(FUN_5b64e0)    // CSurface::SetState84
ASM_PROC(FUN_5b6330)    // Rect::Contains
ASM_PROC(FUN_5d21b0)    // list node: unlink
ASM_PROC(FUN_5d8630)    // CWindow::Clear
ASM_PROC(FUN_5d88c0)    // CWindow::Present
ASM_PROC(FUN_5d3460)    // ObjectContainer::DeleteAll
ASM_PROC(FUN_5d3570)    // ObjectContainer::Add
ASM_PROC(FUN_5d81c0)    // Video_SetMode
ASM_PROC(FUN_5e8c60)    // Font_Load
ASM_PROC(FUN_5bc2f0)    // Font::~Font
ASM_PROC(FUN_603b70)    // Ini_GetInt
ASM_PROC(FUN_5279e0)    // King_PlayMovie
ASM_PROC(FUN_511820)    // King_LeaveGame
ASM_PROC(FUN_512720)

// Milliseconds: QueryPerformanceCounter, scaled against timeGetTime once at
// startup [FUN_603f30].
inline double Timer_Now() { return CallC<double>(FUN_6037e0); }

// DirectInput force feedback: DISFFC_CONTINUE when on, DISFFC_PAUSE when off.
inline void Joy_ForceFeedback(BOOL on) { CallC(FUN_4e12b0, on); }

// GetPrivateProfileString of section/key from file; *value is the number in
// it (strtol, any base), or def if the key is missing or starts with 'N'.
inline int Ini_GetInt(const char* section, const char* key, int def, int* value, const char* file)
{
    return CallC<int>(FUN_603b70, section, key, def, value, file);
}

// ---------------------------------------------------------------------------
// Input (input.cpp)

// Input events are queued by the window procedure and consumed once per frame.
struct InputEvent {             // slot of the 100-entry ring buffer [$L_702660]
    short msg;                  // WM_xxx
    short x, y;                 // mouse position, screen then window coordinates
    short key;                  // virtual key of keyboard messages
};

extern "C" {
// Takes the oldest event; returns TRUE if the queue was empty.
BOOL __cdecl Input_GetEvent(InputEvent* ev);
// Appends an event; returns TRUE if the queue was full (the event is lost).
BOOL __cdecl Input_PutEvent(const InputEvent* ev);
// Moves the mouse cursor to a point of the game window's client area.
void __cdecl Input_SetCursorPos(int x, int y);
}

// ---------------------------------------------------------------------------
// Drawing

struct Rect {
    int left, top, right, bottom;

    // Edges included.
    BOOL Contains(int x, int y) { return CallThis<BOOL>(FUN_5b6330, this, x, y); }
};

struct CSurface {               // DirectDraw/Direct3D backed drawing surface
    WORD width, height;         // +0
    BYTE m_4[80];
    int  m_84;                  // +84  shared with the parent surfaces
    BYTE m_88[8];
    CSurface* m_pParent;        // +96  NULL: a surface of its own
    Rect m_rect;                // +100 the part of the parent it covers

    // Restores lost DirectDraw surfaces and makes this surface the draw
    // target (Direct3D viewport or locked buffer).
    void Prepare() { CallThis(FUN_5b6bf0, this); }
    // Screen coordinates -> coordinates relative to this (sub)surface.
    void ScreenToClient(short* x, short* y) { CallThis(FUN_5b6500, this, x, y); }
    // The rectangle the surface covers: m_rect, or 0,0,width,height.
    Rect* GetRect(Rect* r) { return CallThis<Rect*>(FUN_5b6710, this, r); }
    // m_84 of the outermost parent; setting it sets the whole parent chain
    // and returns the old value.
    int  GetState84() { return CallThis<int>(FUN_5b64c0, this); }
    int  SetState84(int v) { return CallThis<int>(FUN_5b64e0, this, v); }
};
CHECK_OFFSET(CSurface, m_84, 84);
CHECK_OFFSET(CSurface, m_pParent, 96);
CHECK_OFFSET(CSurface, m_rect, 100);

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

struct Font;                    // bitmap font, .fon or .sft file

// Loads a font file; NULL if it can't be opened.
inline Font* Font_Load(const char* path) { return CallC<Font*>(FUN_5e8c60, path); }
inline void Font_Destroy(Font* font) { CallThis(FUN_5bc2f0, font); }
// Nothing without a font or text.
inline void Gfx_DrawText(CSurface* s, int x, int y, Font* font, const char* text, int color, int flags)
{
    CallC(FUN_5ea170, s, x, y, font, text, color, flags);
}
// Writes the surface to an open file as TGA / the screen as BMP.
inline void Gfx_SaveTga(CSurface* s, FILE* file) { CallC(FUN_5e9a10, s, file); }
inline void Gfx_SaveScreenBmp(FILE* file) { CallC(FUN_5e9f00, file); }

// Switches the DirectDraw display mode; flags -1: the current ones.
inline void Video_SetMode(int width, int height, int cres, DWORD flags)
{
    CallC(FUN_5d81c0, width, height, cres, flags);
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
    CWindow*  m_pPrev;          // +4   CGameApp's window list
    CWindow*  m_pNext;          // +8
    CSurface* m_pSurface;       // +12
    DWORD     m_flags;          // +16  WND_*
    int       m_20;
    double    m_totalTime;      // +24  timings of the last frame, ms
    double    m_time32;         // +32
    double    m_prepareTime;    // +40
    int       m_48;
    Cursor*   m_pCursor;        // +52  NULL: crosshair

    void v_Delete() { CallVirt(this, 0, 1); }            // delete this
    void v_OnVideoMode(int width, int height, int cres, DWORD flags)
    {
        CallVirt(this, 40, width, height, cres, flags);
    }
    void v_Execute() { CallVirt(this, 48); }             // per-frame work
    int  v_OnInput(InputEvent* ev) { return CallVirt<int>(this, 52, ev); }
    void v_OnKeyDown(InputEvent* ev) { CallVirt(this, 56, ev); }   // CGameApp::m_bProtectInput
    void v_Prepare() { CallVirt(this, 60); }             // m_pSurface->Prepare()
    // Draws the window's lines of the statistics overlay above y; returns
    // the y for the next window.
    int  v_DrawStats(CWindow* screen, int y) { return CallVirt<int>(this, 64, screen, y); }
    void v_68() { CallVirt(this, 68); }                  // before it is deleted

    void Clear(int color) { CallThis(FUN_5d8630, this, color); }  // color < 0: no-op
    void Present() { CallThis(FUN_5d88c0, this); }
    // Takes the window out of its list (method of the list node base class).
    void Unlink() { CallThis(FUN_5d21b0, this); }
};
CHECK_OFFSET(CWindow, m_flags, 16);
CHECK_OFFSET(CWindow, m_totalTime, 24);
CHECK_OFFSET(CWindow, m_prepareTime, 40);
CHECK_OFFSET(CWindow, m_pCursor, 52);

// An object container of the engine (unit at 0x5cf180).
struct ObjectContainer {
    int  m_0;                   // CKingApp::PostExecute compares it with 2
    BYTE m_4[28];

    void Add(void* obj) { CallThis(FUN_5d3570, this, obj); }
    void DeleteAll() { CallThis(FUN_5d3460, this); }
};

// The game screen: CGameApp::m_pMainView, [$L_696ca0] in King.
struct CMainView : CWindow {
    BYTE   m_56[2968 - 56];
    double m_2968;              // clock of the game (the AI's time)
    BYTE   m_2976[16];
    ObjectContainer m_objects;  // +2992
    int    m_3024;
    int    m_3028;
    int    m_3032;
    BOOL   m_bExit;             // +3036 request to leave the game screen

    void v_140() { CallVirt(this, 140); }                // King [$L_4e06f0]
    void DeleteObjects() { m_objects.DeleteAll(); }
};
CHECK_OFFSET(CMainView, m_2968, 2968);
CHECK_OFFSET(CMainView, m_objects, 2992);
CHECK_OFFSET(CMainView, m_bExit, 3036);

// ---------------------------------------------------------------------------
// Application

// The library functions are labelled in king.masm with their decorated names
// (symbols.csv), so a declaration with the library's own signature binds to
// the label by itself.
#define AFXAPI __stdcall
BOOL AFXAPI AfxOleGetUserCtrl();                // ?AfxOleGetUserCtrl@@YGHXZ
void AFXAPI AfxPostQuitMessage(int nExitCode);  // ?AfxPostQuitMessage@@YGXH@Z

// C runtime: operator new failure handling.
typedef int (__cdecl* NewHandler)(size_t size);
NewHandler __cdecl _set_new_handler(NewHandler handler); // ?_set_new_handler@@YAP6AHI@ZP6AHI@Z@Z
int __cdecl _set_new_mode(int mode);                     // ?_set_new_mode@@YAHH@Z

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

    // MFC's, see KING_ALIAS below
    void Construct(const char* appName);    // CWinApp::CWinApp
    void Destruct();                        // CWinApp::~CWinApp
    BOOL OnIdle(LONG count);
    int  Run();                 // mainloop.cpp
};
CHECK_OFFSET(CWinApp, m_pMainWnd, 28);
CHECK_OFFSET(CWinApp, m_msgCur, 48);
static_assert(sizeof(CWinApp) == 192, "CWinApp size");

// This struct has no vtable or constructors of its own, so it calls MFC's
// as plain members, under names the linker maps to MFC's (OnIdle is
// virtual in MFC: ?OnIdle@CWinApp@@UAEHJ@Z).
KING_ALIAS("?Construct@CWinApp@@QAEXPBD@Z", "??0CWinApp@@QAE@PBD@Z")
KING_ALIAS("?Destruct@CWinApp@@QAEXXZ", "??1CWinApp@@UAE@XZ")
KING_ALIAS("?OnIdle@CWinApp@@QAEHJ@Z", "?OnIdle@CWinApp@@UAEHJ@Z")

enum {
    APP_BORDIN        = 0x02,   // [ENV] bordin = on
    APP_SMOOTH_MOUSE  = 0x04,   // average each mouse position with the previous one
    APP_MOUSE_CAPTURE = 0x08,   // mouse events go to m_pInputWnd, not the window under the mouse
    APP_NO_PRESENT    = 0x20,   // neither the cursor nor the frame reach the screen
    APP_STATS_WINDOWS = 0x10000000, // [RENDER] statistic = full: also the windows' lines
    APP_STATS         = 0x20000000, // [RENDER] statistic = on: frame time, fps, video mode
};

// Languages, [ENV] langv
enum { LANG_ENG, LANG_OTHER, LANG_GER, LANG_FRA, LANG_SPA };

// Engine application class, vtable [$L_652664]. The game's class derives
// from it through one more class (vtable [$L_65195c]).
struct CGameApp : CWinApp {
    int        m_delWrite;      // +192 ring buffer of windows deleted at the end of the frame
    int        m_delRead;       // +196
    CWindow*   m_delQueue[10];  // +200
    CWindow*   m_pWindows;      // +240 first window: executed first, hit-tested last
    CWindow*   m_pLastWindow;   // +244 last window of the same list
    CWindow*   m_pInputWnd;     // +248 window with the input focus
    CWindow*   m_pScreen;       // +252 owner of the primary surface
    int        m_width;         // +256 video mode, [ENV] xres
    int        m_height;        // +260 [ENV] yres
    int        m_cres;          // +264 [ENV] cres
    DWORD      m_modeFlags;     // +268 [ENV] fullscreen, d3d, [RENDER] zbuf
    Font*      m_pMenuFont;     // +272 menu.fon or menu.sft, for the statistics
    char*      m_appDir;        // +276 the program's directory, set by InitInstance
    char*      m_iniFile;       // +280 truck.ini
    int        m_reqWidth;      // +284 requested video mode, set at the end of the frame
    int        m_reqHeight;     // +288
    DWORD      m_flags;         // +292 APP_*
    int        m_clearColor;    // +296 [RENDER] clear; < 0: the screen is not cleared
    BOOL       m_bAltTab;       // +300 [ENV] alttab
    int        m_mouseX;        // +304 screen coordinates
    int        m_mouseY;        // +308
    int        m_crossSize;     // +312 half size of the crosshair cursor
    BOOL       m_bJoyMouse;     // +316 [ENV] mouse = joystick: the input queue is not read
    int        m_language;      // +320 LANG_*
    CMainView* m_pMainView;     // +324
    BOOL       m_bRunFrames;    // +328 OnIdle runs frames; FALSE while one runs
    BOOL       m_bPaused;       // +332 Pause key
    int        m_336;
    BOOL       m_bQuit;         // +340
    BOOL       m_bSound;        // +344 [ENV] sound
    BOOL       m_bMidi;         // +348 [ENV] midi
    BOOL       m_bProtectInput; // +352 [ENV] protectinput: key presses go to vtbl+56 of the input window
    int        m_356;
    double     m_pauseTime;     // +360 paused: start time; unpaused: length, for one frame
    double     m_frameBarScale; // +368 pixels per ms of the statistics' frame time bar
    double     m_lastInputTime; // +376
    int        m_384;
    int        m_388;
    HINSTANCE  m_hInstance;     // +392 set by InitInstance
    int        m_396;
    char*      m_targetTexture; // +400 [RENDER] targettexture; NULL for "none"

    void v_DestroyAllWindows() { CallVirt(this, 160); }
    void v_RunFrame() { CallVirt(this, 164); }
    void v_Frame() { CallVirt(this, 168); }
    void v_PreExecute() { CallVirt(this, 172); }         // before the windows execute
    void v_PostExecute() { CallVirt(this, 176); }        // after them, overlays
    void v_RequestQuit() { CallVirt(this, 180); }        // Alt+F4
    void v_OnPausedKey(int key) { CallVirt(this, 188, key); }
    void v_DispatchInput(InputEvent* ev) { CallVirt(this, 192, ev); }  // to m_pInputWnd

    // gameapp.cpp
    CGameApp* Construct(const char* iniName, const char* iniDir);
    void Destruct();
    void SetMousePos(int x, int y);
    void DestroyWindowNow(CWindow* w);
    void DestroyAllWindows();
    CWindow* WindowFromPoint(int x, int y);
    void SetVideoMode(int width, int height, int cres, DWORD flags);
    char* MakePath(const char* name);
    void PostExecute();
    void DispatchInput(InputEvent* ev);

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
CHECK_OFFSET(CGameApp, m_modeFlags, 268);
CHECK_OFFSET(CGameApp, m_flags, 292);
CHECK_OFFSET(CGameApp, m_mouseX, 304);
CHECK_OFFSET(CGameApp, m_pMainView, 324);
CHECK_OFFSET(CGameApp, m_bQuit, 340);
CHECK_OFFSET(CGameApp, m_bProtectInput, 352);
CHECK_OFFSET(CGameApp, m_pauseTime, 360);
CHECK_OFFSET(CGameApp, m_lastInputTime, 376);
CHECK_OFFSET(CGameApp, m_hInstance, 392);
CHECK_OFFSET(CGameApp, m_targetTexture, 400);

// The game's application class, vtable [$L_64d830], object [$L_696750].
struct CKingApp : CGameApp {
    // mainloop.cpp
    void Frame();

    // kingapp.cpp
    void Destruct();
    void PreExecute();
    void PostExecute();
    void OnPausedKey(int key);
    void DispatchInput(InputEvent* ev);
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

ASM_VAR(CGameApp*, g_pApp, $L_721750)
ASM_VAR(HWND, g_hWnd, $L_720920)
ASM_VAR(BOOL, g_bNoFrame, $L_721774)           // last OnIdle call ran no frame
ASM_VAR(DWORD, g_renderFlags, $L_702e8c)
ASM_VAR(BOOL, g_bTaskDump, $L_721768)
ASM_VAR(StateDump, g_stateDump, $L_71e808)
ASM_VAR(int, g_testInfoCount, $L_720720)       // printed as "test info: %d"
ASM_VAR(float, g_clearTime, $L_721748)         // frame start until the screen is cleared
ASM_VAR(float, g_presentTime, $L_72174c)

// Hooks called by the fatal error function [FUN_5d2e70] before the program
// ends. Each module that installs one keeps the previous one and chains to it.
typedef void (__cdecl* FatalHook)();
ASM_VAR(FatalHook, g_pfnOnFatal, $L_67fb7c)
ASM_VAR(FatalHook, g_pfnPrevOnFatal, $L_7217a0) // CGameApp's predecessor
ASM_FUNC(CGameApp_OnFatal, $L_5e3120)          // stops the sound, then the predecessor
ASM_FUNC(CGameApp_OnNoMemory, $L_5e30f0)       // new handler: fatal "Out of memory <size>"

ASM_VAR(int, g_deviceIndex, $L_7208e4)         // [ENV] numdev - 1
ASM_VAR(int, g_zres, $L_7208e8)                // [ENV] zres
ASM_VAR(BOOL, g_bSound, $L_72176c)             // [ENV] sound
typedef char PathBuffer[128];
ASM_VAR(PathBuffer, g_pathBuf, $L_720c88)      // CGameApp::MakePath's result
ASM_VAR(void* const, g_vtblCGameApp, $L_652664)

ASM_PROC(FUN_5ab1b0)    // NextToken
ASM_PROC(FUN_5e4300)    // PathDir

// Splits the next word (separated by blanks and tabs) off *p, terminating it
// in place, and advances *p; NULL when there is none.
inline char* NextToken(char** p) { return CallC<char*>(FUN_5ab1b0, p); }
// The directory part of path, in a static buffer; "." if it has none.
inline char* PathDir(const char* path) { return CallC<char*>(FUN_5e4300, path); }

// statistics overlay (CGameApp::PostExecute)
ASM_VAR(double, g_lastStatsTime, $L_721798)
ASM_VAR(double, g_frameTime, $L_721790)        // ms between the last two overlays
ASM_VAR(int, g_702e9c, $L_702e9c)              // shown below the frame time
// a TGA screenshot of every frame into .\F7DIR while set
ASM_VAR(BOOL, g_bRecordFrames, $L_721760)
ASM_VAR(int, g_recordFrame, $L_721758)

// ---------------------------------------------------------------------------
// King

struct CWnd6cec6c : CWindow {
    BYTE m_56[1300 - 56];
    BOOL m_1300;
    int  m_1304;
    int  m_1308;
    int  m_1312;
    BOOL m_1316;
    BYTE m_1320[1356 - 1320];
    CMainView* m_1356;          // the game screen
};
CHECK_OFFSET(CWnd6cec6c, m_1316, 1316);
CHECK_OFFSET(CWnd6cec6c, m_1356, 1356);

struct KingObj21600 {
    BYTE  m_0[10048];
    short m_10048;
};

struct KingObj616 {
    BYTE          m_0[21600];
    KingObj21600* m_21600;
};

struct KingObj6d2078 {
    BYTE        m_0[616];
    KingObj616* m_616;
};

ASM_VAR(CMainView*, g_pMainView, $L_696ca0)
ASM_VAR(CWnd6cec6c*, g_pWnd6cec6c, $L_6cec6c)
ASM_VAR(KingObj6d2078*, g_p6d2078, $L_6d2078)
ASM_VAR(BOOL, g_bPlayTrirrMovie, $L_6f3428)
ASM_VAR(void* const, g_vtblCKingApp, $L_64d830)

// Plays name from the [INSTALL] movie directory of truck.ini if the file exists.
inline BOOL King_PlayMovie(const char* name, int mode) { return CallC<BOOL>(FUN_5279e0, name, mode); }
// King's reaction to CMainView::m_bExit: blanks the screen and shuts the game
// screen's subsystems down.
inline void King_LeaveGame() { CallC(FUN_511820); }

ASM_PROC(FUN_5764f0)    // King_CheatKey
ASM_PROC(FUN_5ef3d0)    // Sound_Find
ASM_PROC(FUN_5fdaf0)    // Sound::Play
ASM_PROC(FUN_5bd710)    // Gfx_DrawBox
ASM_PROC(FUN_530050)    // Controls::Update
ASM_PROC(FUN_578fe0)    // Joy_Read
ASM_PROC(FUN_579290)    // Joy_Shutdown
ASM_PROC(FUN_5c3990)    // destructor of CKingApp's base class
ASM_PROC(FUN_50c1d0)
ASM_PROC(FUN_56ce40)
ASM_PROC(FUN_53f750)
ASM_PROC(FUN_53b4c0)

// Appends a key typed during the pause to the cheat code being typed and
// checks it against the list of codes [$L_67b760].
inline void King_CheatKey(int key) { CallC(FUN_5764f0, key); }

struct Sound {
    void Play(float volume, float pitch) { CallThis(FUN_5fdaf0, this, volume, pitch); }
};
// The sound of that name, NULL if there is none.
inline Sound* Sound_Find(const char* name) { return CallC<Sound*>(FUN_5ef3d0, name); }

struct DrawStyle {              // passed by value
    int   color;                // flag 2: fill colour
    int   m_4[5];
    float m_24;
    int   m_28[2];
};
static_assert(sizeof(DrawStyle) == 36, "DrawStyle size");
// A rectangle drawn after flags: 2 filled with style.color, 8 an image, ...
inline void Gfx_DrawBox(CSurface* s, Rect* r, int flags, DrawStyle style)
{
    CallC(FUN_5bd710, s, r, flags, style);
}

// Controls: the keyboard keys, then 32 joystick buttons, then the POV hat
// as four buttons (up, left, down, right).
struct ControlState {
    BYTE on;                    // pressed, or a press of this frame
    BYTE latched;               // 0: Controls::Update clears 'on' every frame
};
enum { CONTROL_JOY_BUTTON = 259, CONTROL_COUNT = 295 };
typedef ControlState Controls[CONTROL_COUNT];
ASM_VAR(Controls, g_controls, $L_6d1948)
// Clears 'on' of the controls that aren't latched.
inline void Controls_Update() { CallThis(FUN_530050, g_controls); }

struct JoyAxis {
    float value;                // -1..1
    int   m_4;
    BOOL  invert;
};
typedef JoyAxis JoyAxes[8];
ASM_VAR(JoyAxes, g_joyAxes, $L_6d1bb0)         // X, Y, Z, rotations, two sliders

struct JoyInfo {
    int  m_0, m_4;
    BOOL axisPresent[8];        // +8  in the order of g_joyAxes
    int  m_40;
    WORD numPovs;               // +44
};
CHECK_OFFSET(JoyInfo, numPovs, 44);

struct JoyState {               // DIJOYSTATE
    LONG  axis[8];              // lX, lY, lZ, lRx, lRy, lRz, rglSlider[2]
    DWORD pov[4];               // hundredths of a degree, LOWORD 0xffff: centred
    BYTE  buttons[32];          // 0x80: down
};
static_assert(sizeof(JoyState) == 80, "JoyState size");

typedef BOOL JoyFlags[36];
typedef float JoyTimes[36];
ASM_VAR(BOOL, g_bJoystick, $L_696ca4)
ASM_VAR(JoyInfo*, g_pJoyInfo, $L_6f4b8c)
ASM_VAR(BOOL, g_bJoyPov, $L_6d2064)            // the POV hat works as buttons
ASM_VAR(double, g_joyAxisScale, $L_6f3520)
ASM_VAR(JoyFlags, g_joyDown, $L_696b08)        // buttons, then POV directions
ASM_VAR(JoyTimes, g_joyTime, $L_696910)        // when they were last down (Timer_Now / 100)

// Polls the joystick; nonzero if it can't be read.
inline int Joy_Read(JoyState* state, int a2) { return CallC<int>(FUN_578fe0, state, a2); }
inline void Joy_Shutdown() { CallC(FUN_579290); }

ASM_VAR(float, g_6974bc, $L_6974bc)
ASM_VAR(short, g_6974c2, $L_6974c2)
ASM_VAR(int, g_screenshot, $L_6f34dc)          // number of the next F6 screenshot
typedef int Int6[6];
ASM_VAR(Int6, g_696cf0, $L_696cf0)
