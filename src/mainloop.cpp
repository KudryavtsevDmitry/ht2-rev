// mainloop.cpp - the main loop of king.exe, rewritten from king.masm.
//
// king.exe is an MFC application: WinMain -> AfxWinMain [FUN_636b51] calls
// InitInstance and then Run. The game runs from the idle handler of the
// message loop, one frame per call:
//
//   CWinApp::Run             [$L_63267d]   message loop (CWinThread::Run inlined)
//    `- OnIdle     vtbl+96   [$L_5dd440]   CGameApp::OnIdle, called while no
//        |                                 messages are waiting; always TRUE
//        `- Frame  vtbl+168  [$L_569650]   CKingApp::Frame (the engine's own:
//            |                             CGameApp::Frame [$L_5e1af0])
//            `- RunFrame vtbl+164 [$L_5e2040]  CGameApp::RunFrame: executes the
//                                          windows, input, cursor, present
//
// The functions keep the behaviour of the assembly they replace (group
// mainloop of src\replace.txt). Building with  nmake ASM=mainloop  links the
// original assembly instead.

#include "king.h"
#include <intrin.h>

// The compiler references _fltused when floating point is used; it comes
// from the CRT, which this object is linked without.
extern "C" int _fltused = 0;

static const int CROSSHAIR_COLOR = 5;

// ---------------------------------------------------------------------------
// MFC 4.2 CWinApp::Run with CWinThread::Run [FUN_632c50] inlined

int CWinApp::Run()
{
    if (m_pMainWnd == NULL && AfxOleGetUserCtrl())
        AfxPostQuitMessage(0);

    BOOL bIdle = TRUE;
    LONG lIdleCount = 0;
    for (;;) {
        // nothing to do for the message queue: run the game
        while (bIdle && !PeekMessageA(&m_msgCur, NULL, 0, 0, PM_NOREMOVE)) {
            if (!v_OnIdle(lIdleCount++))
                bIdle = FALSE;
        }
        do {
            if (!v_PumpMessage())       // WM_QUIT
                return v_ExitInstance();
            if (v_IsIdleMessage(&m_msgCur)) {
                bIdle = TRUE;
                lIdleCount = 0;
            }
        } while (PeekMessageA(&m_msgCur, NULL, 0, 0, PM_NOREMOVE));
    }
}

// ---------------------------------------------------------------------------
// CGameApp

BOOL CGameApp::OnIdle(LONG count)
{
    // Frames are switched off, or this is a nested call from inside a frame.
    // [FUN_5dd4d0] uses g_bNoFrame to see that the loop has stopped.
    if (!m_bRunFrames) {
        g_bNoFrame = TRUE;
        return TRUE;
    }
    g_bNoFrame = FALSE;
    m_bRunFrames = FALSE;
    v_Frame();
    if (m_bQuit) {
        v_DestroyAllWindows();
        DestroyWindow(g_hWnd);
        return TRUE;
    }
    m_bRunFrames = TRUE;
    CWinApp::OnIdle(count);
    return TRUE;                        // keep calling: one frame per call
}

void CGameApp::Frame()
{
    v_RunFrame();
    if (m_pMainView && m_pMainView->m_bExit) {
        m_pMainView->m_bExit = FALSE;
        m_bQuit = TRUE;
    }
    m_336 = 0;
}

static void StateDump_Write(const char* text)
{
    if (!g_stateDump.data)
        return;
    while (*text && g_stateDump.pos < g_stateDump.size)
        g_stateDump.data[g_stateDump.pos++] = *text++;
}

void CGameApp::RunFrame()
{
    g_testInfoCount = 0;
    double t0 = Timer_Now();

    if (g_bTaskDump) {
        g_stateDump.pos = 0;
        if (g_stateDump.data)
            __stosb((BYTE*)g_stateDump.data, 0, g_stateDump.size);
        StateDump_Write("\n\nBEGIN FRAME\n");
    }

    if (!m_bPaused) {
        m_pScreen->v_Prepare();
        m_pScreen->Clear(m_clearColor);
        g_clearTime = (float)(Timer_Now() - t0);
        v_PreExecute();

        if (g_bTaskDump)
            StateDump_Write("BEGIN EXECUTE\n");

        // every visible window does its frame: game view, HUD, menus, ...
        for (CWindow* w = m_pWindows; w; w = w->m_pNext) {
            if (!(w->m_flags & WND_VISIBLE) || w == m_pScreen)
                continue;
            double t = Timer_Now();
            w->v_Prepare();
            w->m_prepareTime = Timer_Now() - t;
            w->v_Execute();
            double t1 = Timer_Now();
            double t2 = Timer_Now();
            w->m_time32 = t2 - t1;
            w->m_totalTime = t2 - t;
        }

        if (g_bTaskDump)
            StateDump_Write("END EXECUTE\n");

        m_pScreen->v_Prepare();
        v_PostExecute();
        m_pauseTime = 0;                // the pause length is used up
    } else {
        m_pScreen->v_Prepare();
        v_PostExecute();
    }

    if (m_pInputWnd) {
        ProcessInput();
        DrawCursor();
    }

    if (g_bTaskDump)
        StateDump_Write("END FRAME\n");

    if (g_renderFlags & 0x80)
        m_pScreen->Clear(m_clearColor);

    double t = Timer_Now();
    if (!(m_flags & APP_NO_PRESENT))
        m_pScreen->Present();
    g_presentTime = (float)(Timer_Now() - t);

    // windows closed during the frame
    while (m_delRead != m_delWrite) {
        DestroyWindowNow(m_delQueue[m_delRead]);
        m_delRead = (m_delRead + 1) % 10;
    }

    // a resize of the Win32 window requests a new mode
    if (m_width != m_reqWidth || m_height != m_reqHeight)
        SetVideoMode(m_reqWidth, m_reqHeight, m_264, m_modeFlags);
}

// Hands the input events queued by the window procedure to the windows:
// mouse events to the window under the mouse, the rest to the input window.
void CGameApp::ProcessInput()
{
    InputEvent ev;
    BOOL bEmpty = TRUE;
    for (;;) {
        // with m_bHoldInput the last event is handed out again
        if (!m_bHoldInput)
            bEmpty = Input_GetEvent(&ev);
        if (bEmpty)
            break;

        switch (ev.msg) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            m_lastInputTime = Timer_Now();
            // fall through
        case WM_MOUSEMOVE: {
            m_lastInputTime = Timer_Now();
            DWORD flags = m_flags;
            if (flags & APP_SMOOTH_MOUSE) {
                m_mouseX = (int)((m_mouseX + ev.x) * 0.5);
                m_mouseY = (int)((m_mouseY + ev.y) * 0.5);
                ev.x = (short)m_mouseX;
                ev.y = (short)m_mouseY;
            } else {
                m_mouseX = ev.x;
                m_mouseY = ev.y;
            }
            CWindow* target = (flags & APP_MOUSE_CAPTURE) ? m_pInputWnd
                                                          : WindowFromPoint(m_mouseX, m_mouseY);
            if (target && !m_bPaused) {
                target->m_pSurface->ScreenToClient(&ev.x, &ev.y);
                target->v_OnInput(&ev);
            }
            break;
        }
        default:                        // keyboard, double clicks, ...
            m_pInputWnd->m_pSurface->ScreenToClient(&ev.x, &ev.y);
            if (!(m_flags & (APP_NO_PRESENT | 0x40)) && ev.msg == WM_KEYDOWN) {
                m_lastInputTime = Timer_Now();
                if (ev.key == VK_PAUSE) {
                    m_bPaused ^= 1;
                    if (m_bPaused) {
                        Joy_ForceFeedback(FALSE);
                        m_pauseTime = Timer_Now();
                    } else {
                        Joy_ForceFeedback(TRUE);
                        m_pauseTime = Timer_Now() - m_pauseTime;
                    }
                } else if (m_bPaused) {
                    v_OnPausedKey(ev.key);
                }
            }
            if (ev.msg == WM_SYSKEYDOWN && ev.key == VK_F4)
                v_RequestQuit();
            v_DispatchInput(&ev);
            break;
        }
    }
}

// Draws the cursor of the window under the mouse over the frame, or a
// crosshair if that window has none.
void CGameApp::DrawCursor()
{
    if (!(m_pInputWnd->m_flags & WND_CURSOR))
        return;

    CSurface* surface = m_pScreen->m_pSurface;
    surface->Prepare();

    Cursor* cursor;
    if (m_flags & APP_MOUSE_CAPTURE) {
        cursor = m_pInputWnd->m_pCursor;
    } else {
        CWindow* w = WindowFromPoint(m_mouseX, m_mouseY);
        cursor = w ? w->m_pCursor : NULL;
    }
    if (m_flags & APP_NO_PRESENT)
        return;

    int x = m_mouseX, y = m_mouseY;
    if (cursor) {
        Gfx_DrawImage(surface, x - cursor->hotX, y - cursor->hotY, cursor->image, 0);
        return;
    }

    int r = m_crossSize;
    Gfx_BeginScene();
    if (r > 3) {                        // two pixels thick
        Gfx_HLine(surface, y + 1, x - r, x + r + 1, CROSSHAIR_COLOR, 0);
        Gfx_HLine(surface, y, x - r, x + r + 1, CROSSHAIR_COLOR, 0);
        Gfx_VLine(surface, x + 1, y - r, y + r + 1, CROSSHAIR_COLOR, 0);
        Gfx_VLine(surface, x, y - r, y + r + 1, CROSSHAIR_COLOR, 0);
    } else {
        Gfx_HLine(surface, y, x - r, x + r, CROSSHAIR_COLOR, 0);
        Gfx_VLine(surface, x, y - r, y + r, CROSSHAIR_COLOR, 0);
    }
    Gfx_EndScene();
}

// ---------------------------------------------------------------------------
// CKingApp

void CKingApp::Frame()
{
    v_RunFrame();

    if (g_bPlayTrirrMovie) {
        King_PlayMovie("\\trirr.bne", 2);
        g_bPlayTrirrMovie = FALSE;
    }
    // where the engine's Frame quits the program
    if (g_pMainView->m_bExit) {
        g_pMainView->m_bExit = FALSE;
        King_LeaveGame();
    }
    if (g_pWnd6cec6c->m_1300 && g_pMainView->m_3024) {
        g_pMainView->DeleteObjects();
        g_p6d2078 = NULL;
        CallC(FUN_512720);
    }
    if (g_pWnd6cec6c->m_1316 && g_pMainView->m_3024) {
        g_pMainView->v_140();
        g_pMainView->m_3024 = 0;
    }
}

// ---------------------------------------------------------------------------
// Entry points for king.masm, which uses them in place of the labels in [ ]

extern "C" {

int __fastcall CWinApp_Run(CWinApp* self, void*)                    // [$L_63267d]
{
    return self->Run();
}

BOOL __fastcall CGameApp_OnIdle(CGameApp* self, void*, LONG count)  // [$L_5dd440]
{
    return self->OnIdle(count);
}

void __fastcall CGameApp_Frame(CGameApp* self, void*)               // [$L_5e1af0]
{
    self->Frame();
}

void __fastcall CGameApp_RunFrame(CGameApp* self, void*)            // [$L_5e2040]
{
    self->RunFrame();
}

void __fastcall CKingApp_Frame(CKingApp* self, void*)               // [$L_569650]
{
    self->Frame();
}

}
