// gameapp.cpp - CGameApp, the engine's application class, rewritten from
// king.masm. The methods of the main loop are in mainloop.cpp.
//
// Building with  nmake ASM_GAMEAPP=1  makes king.masm assemble the original
// procedures of these functions again.

#include "king.h"

static char* StrCat(char* dest, const char* src)
{
    // the game's code has strcat inlined, its C runtime has none to link
    strcpy(dest + strlen(dest), src);
    return dest;
}

// ---------------------------------------------------------------------------
// Construction and destruction

// Reads the settings of truck.ini. The ini file is iniName in iniDir; for a
// NULL iniDir in the current directory, for "__argv1" in the directory of the
// command line's first argument (or of the program, without one).
CGameApp* CGameApp::Construct(const char* iniName, const char* iniDir)
{
    CWinApp::Construct(NULL);
    vtbl = (void**)&g_vtblCGameApp;
    m_pMenuFont = NULL;
    m_336 = 0;
    m_pMainView = NULL;
    m_bPaused = FALSE;
    m_bSound = FALSE;
    m_bMidi = FALSE;
    g_pApp = this;
    g_pfnPrevOnFatal = g_pfnOnFatal;
    g_pfnOnFatal = (FatalHook)CGameApp_OnFatal;
    m_pauseTime = 0;
    m_lastInputTime = 0;
    DWORD modeFlags = 0;
    m_bQuit = FALSE;
    m_bRunFrames = FALSE;
    m_appDir = NULL;
    m_crossSize = 0;
    m_mouseY = 0;
    m_mouseX = 0;
    m_flags = 0;
    m_delRead = 0;
    m_delWrite = 0;
    _set_new_handler((NewHandler)CGameApp_OnNoMemory);
    _set_new_mode(1);                   // malloc fails through the new handler too

    char value[128];                    // also holds the command line
    if (iniDir == NULL) {
        m_iniFile = (char*)operator new(strlen(iniName) + 3);
        m_iniFile[0] = '.';
        m_iniFile[1] = '\\';
        strcpy(m_iniFile + 2, iniName);
    } else {
        const char* dir;
        if (strcmp(iniDir, "__argv1") == 0) {
            strcpy(value, GetCommandLineA());
            char* p = value + 1;        // past the quote around the program
            char* arg = NextToken(&p);
            char* arg1 = NextToken(&p);
            if (arg1)
                arg = arg1;
            if (*arg == '"')
                arg++;
            dir = PathDir(arg);
        } else {
            dir = iniDir;
        }
        m_iniFile = (char*)operator new(strlen(dir) + strlen(iniName) + 2);
        strcpy(m_iniFile, dir);
        StrCat(m_iniFile, "\\");
        StrCat(m_iniFile, iniName);
    }

    m_396 = 0;
    GetPrivateProfileStringA("RENDER", "statistic", "off", value, 60, m_iniFile);
    if (strcmp(value, "on") == 0)
        m_flags |= APP_STATS;
    else if (strcmp(value, "full") == 0)
        m_flags |= APP_STATS | APP_STATS_WINDOWS;

    GetPrivateProfileStringA("RENDER", "targettexture", "none", value, 60, m_iniFile);
    if (strcmp(value, "none") == 0) {
        m_targetTexture = NULL;
    } else {
        m_targetTexture = (char*)operator new(strlen(value) + 1);
        strcpy(m_targetTexture, value);
    }

    int clear, xres, yres, cres, n;
    Ini_GetInt("RENDER", "clear", 0, &clear, m_iniFile);
    m_clearColor = clear;
    Ini_GetInt("ENV", "xres", 640, &xres, m_iniFile);
    Ini_GetInt("ENV", "yres", 480, &yres, m_iniFile);
    Ini_GetInt("ENV", "cres", 2, &cres, m_iniFile);
    Ini_GetInt("ENV", "numdev", 1, &n, m_iniFile);
    g_deviceIndex = --n;
    Ini_GetInt("ENV", "zres", 16, &n, m_iniFile);
    g_zres = n;

    GetPrivateProfileStringA("RENDER", "zbuf", "on", value, 60, m_iniFile);
    if (strcmp(value, "on") == 0)
        modeFlags = 0x4000;
    GetPrivateProfileStringA("ENV", "protectinput", "off", value, 60, m_iniFile);
    m_bProtectInput = strcmp(value, "on") == 0;
    GetPrivateProfileStringA("ENV", "bordin", "off", value, 60, m_iniFile);
    if (strcmp(value, "on") == 0)
        m_flags |= APP_BORDIN;
    GetPrivateProfileStringA("ENV", "alttab", "off", value, 60, m_iniFile);
    m_bAltTab = strcmp(value, "on") == 0;
    GetPrivateProfileStringA("ENV", "fullscreen", "on", value, 60, m_iniFile);
    if (strcmp(value, "on") == 0)
        modeFlags |= 0xe00;
    GetPrivateProfileStringA("ENV", "d3d", "on", value, 60, m_iniFile);
    if (strcmp(value, "on") == 0)
        modeFlags |= (((modeFlags & 0x400) ? 0x1600 : 0) + 0xa00) | 0x1000;
    GetPrivateProfileStringA("ENV", "mouse", "on", value, 60, m_iniFile);
    m_bJoyMouse = strcmp(value, "joystick") == 0;
    GetPrivateProfileStringA("ENV", "midi", "off", value, 60, m_iniFile);
    if (strcmp(value, "on") == 0)
        m_bMidi = TRUE;
    GetPrivateProfileStringA("ENV", "sound", "off", value, 60, m_iniFile);
    if (strcmp(value, "on") == 0) {
        m_bSound = TRUE;
        g_bSound = TRUE;
    }
    GetPrivateProfileStringA("ENV", "langv", "", value, 30, m_iniFile);
    if (strcmp(value, "eng") == 0)
        m_language = LANG_ENG;
    else if (strcmp(value, "ger") == 0)
        m_language = LANG_GER;
    else if (strcmp(value, "fra") == 0)
        m_language = LANG_FRA;
    else if (strcmp(value, "spa") == 0)
        m_language = LANG_SPA;
    else
        m_language = LANG_OTHER;

    m_height = yres;
    m_width = xres;
    m_cres = cres;
    m_modeFlags = modeFlags;
    if (xres == 320)
        m_crossSize = 3;
    else if (xres == 640)
        m_crossSize = 7;
    else
        m_crossSize = xres / 100;
    return this;
}

void CGameApp::Destruct()
{
    vtbl = (void**)&g_vtblCGameApp;
    m_bRunFrames = FALSE;
    DestroyAllWindows();
    operator delete(m_appDir);
    operator delete(m_iniFile);
    operator delete(m_targetTexture);
    g_pApp = NULL;
    CWinApp::Destruct();
}

// ---------------------------------------------------------------------------
// Windows

// Takes a window out of the window list and deletes it.
void CGameApp::DestroyWindowNow(CWindow* w)
{
    if (w == m_pWindows)
        m_pWindows = w->m_pNext;
    if (w == m_pLastWindow)
        m_pLastWindow = w->m_pPrev;
    w->Unlink();
    if (w)
        w->v_Delete();
}

// Deletes all windows, the screen last.
void CGameApp::DestroyAllWindows()
{
    if (!m_pScreen)
        return;
    CWindow* next;
    for (CWindow* w = m_pWindows; w; w = next) {
        next = w->m_pNext;
        if (w == m_pScreen)
            continue;
        w->v_68();
        if (w)
            w->v_Delete();
    }
    if (m_pScreen)
        m_pScreen->v_Delete();
    m_pScreen = NULL;
    m_pWindows = NULL;
}

// The topmost visible window whose surface contains the point (screen
// coordinates). The windows are drawn first to last, so they are tested
// last to first.
CWindow* CGameApp::WindowFromPoint(int x, int y)
{
    for (CWindow* w = m_pLastWindow; w; w = w->m_pPrev) {
        if (!(w->m_flags & WND_VISIBLE))
            continue;
        Rect r;
        w->m_pSurface->GetRect(&r);
        if (r.Contains(x, y))
            return w;
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// Video mode

void CGameApp::SetVideoMode(int width, int height, int cres, DWORD flags)
{
    if (flags == (DWORD)-1)
        flags = m_modeFlags;
    int state84 = m_pScreen->m_pSurface->GetState84();
    // [FUN_5e48f0] was called for every window's surface here: an empty
    // function in this build
    Video_SetMode(width, height, m_cres, flags);

    // the font of the statistics
    if (m_pMenuFont) {
        Font_Destroy(m_pMenuFont);
        operator delete(m_pMenuFont);
        m_pMenuFont = Font_Load(g_pApp->MakePath("menu.fon"));
        if (!m_pMenuFont)
            m_pMenuFont = Font_Load("menu.fon");
        if (!m_pMenuFont) {
            m_pMenuFont = Font_Load(g_pApp->MakePath("menu.sft"));
            if (!m_pMenuFont)
                m_pMenuFont = Font_Load("menu.sft");
        }
    }

    for (CWindow* w = m_pWindows; w; w = w->m_pNext)
        w->v_OnVideoMode(width, height, cres, flags);
    m_width = m_reqWidth = width;
    m_height = m_reqHeight = height;
    m_pScreen->m_pSurface->SetState84(state84);     // the screen's new surface
}

// name in the program's directory, in a static buffer
char* CGameApp::MakePath(const char* name)
{
    strcpy(g_pathBuf, m_appDir);
    StrCat(g_pathBuf, "\\");
    StrCat(g_pathBuf, name);
    return g_pathBuf;
}

// ---------------------------------------------------------------------------
// Frame

// After the windows have executed: the statistics overlay ([RENDER]
// statistic) and the series of screenshots while g_bRecordFrames is set.
void CGameApp::PostExecute()
{
    if ((m_flags & APP_STATS) && m_pScreen && m_pMenuFont) {
        double now = Timer_Now();
        g_frameTime = now - g_lastStatsTime;
        double ms = g_frameTime;
        if (!(ms >= 1.0))
            ms = 1.0;
        int y = m_height - 20;
        char text[64];
        sprintf(text, "%d", (int)ms);
        Gfx_DrawText(m_pScreen->m_pSurface, 50, y, m_pMenuFont, text, 5, 0);
        sprintf(text, "%d", g_702e9c);
        Gfx_DrawText(m_pScreen->m_pSurface, 100, y + 9, m_pMenuFont, text, 5, 0);
        // the frame time as a bar, three pixels high
        Gfx_HLine(m_pScreen->m_pSurface, y, 100, (int)(ms * m_frameBarScale) + 100, 7, 0);
        Gfx_HLine(m_pScreen->m_pSurface, y - 1, 100, (int)(ms * m_frameBarScale) + 100, 7, 0);
        Gfx_HLine(m_pScreen->m_pSurface, y - 2, 100, (int)(ms * m_frameBarScale) + 100, 7, 0);
        sprintf(text, "%d fps,%d xres,%d yres", (int)(1000.0 / ms), g_pApp->m_width, g_pApp->m_height);
        Gfx_DrawText(m_pScreen->m_pSurface, 50, y - 10, m_pMenuFont, text, 7, 0);

        if ((m_flags & (APP_STATS | APP_STATS_WINDOWS)) == (APP_STATS | APP_STATS_WINDOWS)) {
            for (CWindow* w = m_pWindows; w; w = w->m_pNext) {
                if ((w->m_flags & WND_SURFACE) && w != m_pScreen)
                    y = w->v_DrawStats(m_pScreen, y - 20);
            }
        }
        g_lastStatsTime = now;
    }

    if (g_bRecordFrames && g_recordFrame < 9999) {
        char dir[20], path[116];
        sprintf(dir, "%s\\", ".\\F7DIR");
        _mkdir(".\\F7DIR");
        sprintf(path, "%sphoto%04d.tga", dir, g_recordFrame);
        FILE* file = fopen(path, "wb");
        Gfx_SaveTga(m_pScreen->m_pSurface, file);
        fclose(file);
        g_recordFrame++;
    }
}

// An input event that no window under the mouse took, for the input window:
// with [ENV] protectinput key presses through vtbl+56, everything else
// through vtbl+52. Nothing while the game is paused.
void CGameApp::DispatchInput(InputEvent* ev)
{
    if (m_bProtectInput && ev->msg == WM_KEYDOWN) {
        if (!m_bPaused)
            m_pInputWnd->v_OnKeyDown(ev);
        return;
    }
    if (!m_bPaused)
        m_pInputWnd->v_OnInput(ev);
}

// Moves the mouse cursor to a point of the game window.
void CGameApp::SetMousePos(int x, int y)
{
    m_mouseX = x;
    m_mouseY = y;
    Input_SetCursorPos(x, y);
}

// ---------------------------------------------------------------------------
// Entry points for king.masm, which uses them in place of the labels in [ ]

extern "C" {

CGameApp* __fastcall CGameApp_Construct(CGameApp* self, void*, const char* iniName, const char* iniDir) // [FUN_5e3150]
{
    return self->Construct(iniName, iniDir);
}

void __fastcall CGameApp_Destruct(CGameApp* self, void*)                    // [FUN_5e3010]
{
    self->Destruct();
}

CGameApp* __fastcall CGameApp_DeletingDestructor(CGameApp* self, void*, unsigned flags) // [$L_5e30a0]
{
    self->Destruct();
    if (flags & 1)
        operator delete(self);
    return self;
}

void __fastcall CGameApp_SetMousePos(CGameApp* self, void*, int x, int y)  // [FUN_5e30c0]
{
    self->SetMousePos(x, y);
}

void __fastcall CGameApp_DestroyWindowNow(CGameApp* self, void*, CWindow* w) // [FUN_5dba30]
{
    self->DestroyWindowNow(w);
}

void __fastcall CGameApp_DestroyAllWindows(CGameApp* self, void*)          // [FUN_5e2fa0]
{
    self->DestroyAllWindows();
}

CWindow* __fastcall CGameApp_WindowFromPoint(CGameApp* self, void*, int x, int y) // [FUN_5e1c90]
{
    return self->WindowFromPoint(x, y);
}

void __fastcall CGameApp_SetVideoMode(CGameApp* self, void*, int width, int height, int cres,
                                      DWORD flags)                         // [FUN_5e1b30]
{
    self->SetVideoMode(width, height, cres, flags);
}

char* __fastcall CGameApp_MakePath(CGameApp* self, void*, const char* name) // [FUN_5e4270]
{
    return self->MakePath(name);
}

void __fastcall CGameApp_PostExecute(CGameApp* self, void*)                // [FUN_5e1d20]
{
    self->PostExecute();
}

void __fastcall CGameApp_DispatchInput(CGameApp* self, void*, InputEvent* ev) // [FUN_5e1ff0]
{
    self->DispatchInput(ev);
}

}
