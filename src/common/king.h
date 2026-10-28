// king.h - declarations shared by the C++ code of king.exe.
//
// Names: a function keeps the name it got when it was rewritten. Older code
// may still call it under its original label (FUN_xxxxxx, $L_xxxxxx); such a
// label is bound to the function with KING_ALIAS, the linker's
// /alternatename (x86 C names carry a leading underscore; fastcall ones are
// @name@bytes, stdcall ones _name@bytes). Those calls go through CallC,
// CallStd, CallThis: the label is declared void() and the call states the
// argument and return types.
//
// thiscall: C++ can't declare free thiscall functions, so a method is an
// extern "C" __fastcall function with an unused second parameter: fastcall
// passes it in EDX, so for the callee fastcall(this, edx, args) is exactly
// thiscall(this, args). Callers pass 0 for it.
//
// Comments: +N is a byte offset into an object, vtbl+N a byte offset into
// its vtable, [FUN_xxxxxx] / [$L_xxxxxx] the original procedure's label
// (tag last-masm).
#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stddef.h>

#define KING_ALIAS(cname, label) __pragma(comment(linker, "/alternatename:" cname "=" label))
// The program's data is owned by C++: the original _RDATA, _DATA and _BSS are
// KingRdata, KingData (data_image.cpp) and KingBss (data.cpp). KING_VAR(type,
// name, address) is the variable at an original address (hex digits
// without 0x).
#define KING_RDATA_BASE 0x64b000
#define KING_DATA_BASE 0x66c000
#define KING_BSS_BASE 0x68a820
#define KING_BSS_SIZE 0xf52e4
extern "C" unsigned char KingRdata[];
extern "C" unsigned char KingData[];
extern "C" unsigned char KingBss[KING_BSS_SIZE];
#define KING_ADDR(a) ((a) >= KING_BSS_BASE ? KingBss + ((a) - KING_BSS_BASE)     : (a) >= KING_DATA_BASE ? KingData + ((a) - KING_DATA_BASE) : KingRdata + ((a) - KING_RDATA_BASE))
#define KING_VAR(type, name, address) static type& name = *(type*)KING_ADDR(0x##address);
// CRT_PROC(label, modern): a procedure of the old C runtime, now the modern
// runtime's function of that name (decorated)
#define CRT_PROC(label, modern) extern "C" void label(); KING_ALIAS("_" #label, modern)

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

// The standard streams 0 stdin, 1 stdout, 2 stderr of the modern runtime.
extern "C" FILE* __cdecl __acrt_iob_func(unsigned i);
#define CRT_STREAM(i) __acrt_iob_func(i)

// feof: the converted code tested the old runtime's FILE::_flag (+12) for
// _IOEOF (16) as VC6's feof macro did; the modern FILE is opaque.
extern "C" int __cdecl feof(FILE* f);
#define FILE_EOF(f) (feof((FILE*)(f)))

// ---------------------------------------------------------------------------
// Procedures of king.exe

extern "C" void FUN_6037e0(); KING_ALIAS("_FUN_6037e0", "_Tsc_Now")    // Timer_Now
extern "C" void FUN_5b6180(); KING_ALIAS("_FUN_5b6180", "_Device_BeginScene")    // Gfx_BeginScene
extern "C" void FUN_5b6190(); KING_ALIAS("_FUN_5b6190", "_Device_WaitIdle")    // Gfx_EndScene
extern "C" void FUN_5b7ce0(); KING_ALIAS("_FUN_5b7ce0", "_Surface_HLine")    // Gfx_HLine
extern "C" void FUN_5b7f20(); KING_ALIAS("_FUN_5b7f20", "_Surface_VLine")    // Gfx_VLine
extern "C" void FUN_5b8980(); KING_ALIAS("_FUN_5b8980", "_Gfx_DrawImage")    // Gfx_DrawImage
extern "C" void FUN_5ea170(); KING_ALIAS("_FUN_5ea170", "_Text_Draw")    // Gfx_DrawText
extern "C" void FUN_5e9a10(); KING_ALIAS("_FUN_5e9a10", "_Tga_Write")    // Gfx_SaveTga
extern "C" void FUN_5e9f00(); KING_ALIAS("_FUN_5e9f00", "_Screen_GrabToFile")    // Gfx_SaveScreenBmp
extern "C" void FUN_5b6bf0(); KING_ALIAS("_FUN_5b6bf0", "@Surface_Prepare@4")    // CSurface::Prepare
extern "C" void FUN_5b6500(); KING_ALIAS("_FUN_5b6500", "@Window_ToRoot@16")    // CSurface::ScreenToClient
extern "C" void FUN_5b6710(); KING_ALIAS("_FUN_5b6710", "@Window_GetRect@12")    // CSurface::GetRect
extern "C" void FUN_5b64c0(); KING_ALIAS("_FUN_5b64c0", "@Window_RootSurface@8")    // CSurface::GetState84
extern "C" void FUN_5b64e0(); KING_ALIAS("_FUN_5b64e0", "@Window_SetSurface@12")    // CSurface::SetState84
extern "C" void FUN_5b6330(); KING_ALIAS("_FUN_5b6330", "@Rect_Contains@16")    // Rect::Contains
extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")    // list node: unlink
extern "C" void FUN_5d8630(); KING_ALIAS("_FUN_5d8630", "@Window_Show@12")    // CWindow::Clear
extern "C" void FUN_5d88c0(); KING_ALIAS("_FUN_5d88c0", "@Window_Refresh@8")    // CWindow::Present
extern "C" void FUN_5d3460(); KING_ALIAS("_FUN_5d3460", "@Owner_Clear@8")    // ObjectContainer::DeleteAll
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")    // ObjectContainer::Add
extern "C" void FUN_5d81c0(); KING_ALIAS("_FUN_5d81c0", "_Dd_RequestMode")    // Video_SetMode
extern "C" void FUN_5e8c60(); KING_ALIAS("_FUN_5e8c60", "_Font_LoadFile")    // Font_Load
extern "C" void FUN_5bc2f0(); KING_ALIAS("_FUN_5bc2f0", "@Poly_Destroy@8")    // Font::~Font
extern "C" void FUN_603b70(); KING_ALIAS("_FUN_603b70", "_Ini_ReadInt")    // Ini_GetInt
extern "C" int __cdecl Movie_Play(const char* name, int mode);   // carat.cpp
extern "C" void FUN_511820(); KING_ALIAS("_FUN_511820", "_Shell_LeaveRace")    // King_LeaveGame
extern "C" void __cdecl Replay_Open();    // carat.cpp

// Milliseconds: QueryPerformanceCounter, scaled against timeGetTime once at
// startup [FUN_603f30].
inline double Timer_Now() { return CallC<double>(FUN_6037e0); }

// DirectInput force feedback: DISFFC_CONTINUE when on, DISFFC_PAUSE when off.
extern "C" int __cdecl Joy_ForceFeedback(int on);     // engine.cpp

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

// C runtime: operator new failure handling (C++ names in the old runtime,
// C names in the modern one)
typedef int (__cdecl* NewHandler)(size_t size);
extern "C" NewHandler __cdecl _set_new_handler(NewHandler handler);
extern "C" int __cdecl _set_new_mode(int mode);

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
KING_ALIAS("?Construct@CWinApp@@QAEXPBD@Z", "@WinApp_Construct@12")
KING_ALIAS("?Destruct@CWinApp@@QAEXXZ", "@WinApp_Destruct@4")
KING_ALIAS("?OnIdle@CWinApp@@QAEHJ@Z", "@WinApp_OnIdle@12")

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
// Variables of king.exe

// state.dump, a crash trace written when [ENV] taskdump is set in truck.ini
struct StateDump {
    HANDLE hFile;
    HANDLE hMapping;
    int    size;
    char*  data;                // mapped view
    int    pos;
};

KING_VAR(CGameApp*, g_pApp, 721750)
KING_VAR(HWND, g_hWnd, 720920)
KING_VAR(BOOL, g_bNoFrame, 721774)           // last OnIdle call ran no frame
KING_VAR(DWORD, g_renderFlags, 702e8c)
KING_VAR(BOOL, g_bTaskDump, 721768)
KING_VAR(StateDump, g_stateDump, 71e808)
KING_VAR(int, g_testInfoCount, 720720)       // printed as "test info: %d"
KING_VAR(float, g_clearTime, 721748)         // frame start until the screen is cleared
KING_VAR(float, g_presentTime, 72174c)

// Hooks called by the fatal error function [FUN_5d2e70] before the program
// ends. Each module that installs one keeps the previous one and chains to it.
typedef void (__cdecl* FatalHook)();
KING_VAR(FatalHook, g_pfnOnFatal, 67fb7c)
KING_VAR(FatalHook, g_pfnPrevOnFatal, 7217a0) // CGameApp's predecessor
extern "C" void CGameApp_OnFatal(); KING_ALIAS("_CGameApp_OnFatal", "_CGameApp_Fatal")          // stops the sound, then the predecessor
extern "C" void CGameApp_OnNoMemory(); KING_ALIAS("_CGameApp_OnNoMemory", "_CGameApp_NoMemory")       // new handler: fatal "Out of memory <size>"

KING_VAR(int, g_deviceIndex, 7208e4)         // [ENV] numdev - 1
KING_VAR(int, g_zres, 7208e8)                // [ENV] zres
KING_VAR(BOOL, g_bSound, 72176c)             // [ENV] sound
typedef char PathBuffer[128];
KING_VAR(PathBuffer, g_pathBuf, 720c88)      // CGameApp::MakePath's result
KING_VAR(void* const, g_vtblCGameApp, 652664)

extern "C" void FUN_5e4300(); KING_ALIAS("_FUN_5e4300", "_DataPath_Set")    // PathDir

// Splits the next word (separated by blanks and tabs) off *p, terminating it
// in place, and advances *p; NULL when there is none.
extern "C" char* __cdecl NextToken(char** p);         // engine.cpp
// The directory part of path, in a static buffer; "." if it has none.
inline char* PathDir(const char* path) { return CallC<char*>(FUN_5e4300, path); }

// statistics overlay (CGameApp::PostExecute)
KING_VAR(double, g_lastStatsTime, 721798)
KING_VAR(double, g_frameTime, 721790)        // ms between the last two overlays
KING_VAR(int, g_702e9c, 702e9c)              // shown below the frame time
// a TGA screenshot of every frame into .\F7DIR while set
KING_VAR(BOOL, g_bRecordFrames, 721760)
KING_VAR(int, g_recordFrame, 721758)

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

KING_VAR(CMainView*, g_pMainView, 696ca0)
KING_VAR(CWnd6cec6c*, g_pWnd6cec6c, 6cec6c)
KING_VAR(KingObj6d2078*, g_p6d2078, 6d2078)
KING_VAR(BOOL, g_bPlayTrirrMovie, 6f3428)
KING_VAR(void* const, g_vtblCKingApp, 64d830)

// Plays name from the [INSTALL] movie directory of truck.ini if the file exists.
inline BOOL King_PlayMovie(const char* name, int mode) { return Movie_Play(name, mode); }
// King's reaction to CMainView::m_bExit: blanks the screen and shuts the game
// screen's subsystems down.
inline void King_LeaveGame() { CallC(FUN_511820); }

extern "C" void FUN_5764f0(); KING_ALIAS("_FUN_5764f0", "_Cheat_KeyTyped")    // King_CheatKey
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")    // Sound_Find
extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")    // Sound::Play
extern "C" void FUN_5bd710(); KING_ALIAS("_FUN_5bd710", "_Ctl_DrawFrame")    // Gfx_DrawBox
extern "C" void FUN_530050(); KING_ALIAS("_FUN_530050", "@Words295_ClearEmpty@8")    // Controls::Update
extern "C" void FUN_578fe0(); KING_ALIAS("_FUN_578fe0", "_Joy_ReadState")    // Joy_Read
extern "C" void FUN_5c3990(); KING_ALIAS("_FUN_5c3990", "@Med5c3990_Destroy@4")    // destructor of CKingApp's base class
extern "C" void __cdecl Loading_Update();       // carat.cpp
extern "C" void FUN_56ce40(); KING_ALIAS("_FUN_56ce40", "_Hud_Frame")
extern "C" void FUN_53b4c0(); KING_ALIAS("_FUN_53b4c0", "@Player_Controls@8")

// Appends a key typed during the pause to the cheat code being typed and
// checks it against the list of codes [$L_67b760].
inline void King_CheatKey(int key) { CallC(FUN_5764f0, key); }

// car if it is in the list of vehicles, else NULL (vehicle.cpp)
extern "C" BYTE* __cdecl Car_InList(void* car);

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
KING_VAR(Controls, g_controls, 6d1948)
// Clears 'on' of the controls that aren't latched.
inline void Controls_Update() { CallThis(FUN_530050, g_controls); }

struct JoyAxis {
    float value;                // -1..1
    int   m_4;
    BOOL  invert;
};
typedef JoyAxis JoyAxes[8];
KING_VAR(JoyAxes, g_joyAxes, 6d1bb0)         // X, Y, Z, rotations, two sliders

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
KING_VAR(BOOL, g_bJoystick, 696ca4)
KING_VAR(JoyInfo*, g_pJoyInfo, 6f4b8c)
KING_VAR(BOOL, g_bJoyPov, 6d2064)            // the POV hat works as buttons
KING_VAR(double, g_joyAxisScale, 6f3520)
KING_VAR(JoyFlags, g_joyDown, 696b08)        // buttons, then POV directions
KING_VAR(JoyTimes, g_joyTime, 696910)        // when they were last down (Timer_Now / 100)

// Polls the joystick; nonzero if it can't be read.
inline int Joy_Read(JoyState* state, int a2) { return CallC<int>(FUN_578fe0, state, a2); }
extern "C" int __cdecl Joy_Shutdown();            // mmt.cpp
extern "C" double __cdecl Game_Time();            // world.cpp: the game time
extern "C" double __cdecl Game_FrameTime();       // world.cpp: the last frame's time, at most 0.25 s

KING_VAR(float, g_6974bc, 6974bc)
KING_VAR(short, g_6974c2, 6974c2)
KING_VAR(int, g_screenshot, 6f34dc)          // number of the next F6 screenshot
typedef int Int6[6];
KING_VAR(Int6, g_696cf0, 696cf0)
