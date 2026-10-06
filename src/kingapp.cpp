// kingapp.cpp - CKingApp, the game's application class, rewritten from
// king.masm. CKingApp::Frame is in mainloop.cpp.
//
// Building with  nmake ASM_KINGAPP=1  makes king.masm assemble the original
// procedures of these functions again.

#include "king.h"

void CKingApp::Destruct()
{
    vtbl = (void**)&g_vtblCKingApp;
    Joy_Shutdown();
    for (int i = 0; i < 6; i++)
        g_696cf0[i] = 0;
    CallThis(FUN_5c3990, this);         // the base class's destructor
}

// Before the windows execute.
void CKingApp::PreExecute()
{
    // [FUN_5e48f0] was called for g_controls here: an empty function in this
    // build
    if (g_6974c2)
        g_6974bc = (float)(g_6974c2 * 0.001 + g_6974bc);
    else
        g_6974bc = (float)g_pMainView->m_2968;

    if (g_p6d2078 && g_p6d2078->m_616 && CallC<BOOL>(FUN_53f750, g_p6d2078->m_616)
        && g_p6d2078->m_616->m_21600 && g_p6d2078->m_616->m_21600->m_10048 == 1
        && (g_pMainView->m_flags & WND_VISIBLE))
        CallThis(FUN_53b4c0, g_p6d2078);
}

// After the windows have executed: the engine's overlays, then the joystick
// is read into g_controls and g_joyAxes.
void CKingApp::PostExecute()
{
    CGameApp::PostExecute();
    CallC(FUN_50c1d0);
    if (g_p6d2078 && g_pWnd6cec6c->m_1356->m_objects.m_0 < 2) {
        Rect r = { 0, 0, m_width, m_height };
        DrawStyle style = {};
        style.m_24 = 0.5f;
        Gfx_DrawBox(m_pScreen->m_pSurface, &r, 2, style);
    }

    Controls_Update();
    float now = (float)(Timer_Now() * 0.01);
    JoyState js = {};
    if (g_bJoystick && Joy_Read(&js, 0) == 0) {
        // buttons, then the POV hat as four buttons
        static const DWORD povAngle[4] = { 0, 27000, 18000, 9000 };    // up, left, down, right
        DWORD pov = js.pov[0];
        JoyInfo* info = g_pJoyInfo;
        for (int i = 0; i < 36; i++) {
            ControlState* c = &g_controls[CONTROL_JOY_BUTTON + i];
            BOOL down;
            if (i < 32)
                down = (js.buttons[i] & 0x80) != 0;
            else
                down = info->numPovs > 0 && g_bJoyPov && LOWORD(pov) != 0xffff
                    && pov == povAngle[i - 32];
            if (down) {
                if (!g_joyDown[i])
                    c->on = 1;
                g_joyDown[i] = TRUE;
                g_joyTime[i] = now;
            } else {
                if (g_joyDown[i])
                    g_joyDown[i] = FALSE;
                // a latched control stays on for a while after the release
                if (c->latched && (double)now - g_joyTime[i] > 0.3)
                    c->on = 0;
            }
        }

        // axes: X, Y, Z, the rotations, the two sliders, scaled to -1..1
        for (int i = 0; i < 8; i++) {
            double v = js.axis[i] * g_joyAxisScale;
            if (!info->axisPresent[i]) {
                g_joyAxes[i].value = 0;
                continue;
            }
            if (g_joyAxes[i].invert)
                v = -v;
            if (!(v >= -1.0))
                v = -1.0;
            else if (v > 1.0)
                v = 1.0;
            g_joyAxes[i].value = (float)v;
        }
    }
    CallC(FUN_56ce40);
}

// A key pressed while the game is paused: typing a cheat code.
void CKingApp::OnPausedKey(int key)
{
    King_CheatKey(key);
}

// F6 in the game saves a screenshot, .\screenshots\ddphotoNNNN.bmp, with a
// camera sound; all other input goes the engine's way.
void CKingApp::DispatchInput(InputEvent* ev)
{
    if (g_p6d2078 && ev->msg == WM_KEYDOWN && ev->key == VK_F6) {
        if (Sound* sound = Sound_Find("photoSound"))
            sound->Play(1.0f, 1.0f);
        _mkdir(".\\screenshots");
        char path[64];
        sprintf(path, "%s\\ddphoto%04d.bmp", ".\\screenshots", g_screenshot);
        FILE* file = fopen(path, "wb");
        Gfx_SaveScreenBmp(file);
        fclose(file);
        g_screenshot++;
        return;
    }
    CGameApp::DispatchInput(ev);
}

// ---------------------------------------------------------------------------
// Entry points for king.masm, which uses them in place of the labels in [ ]

extern "C" {

void __fastcall CKingApp_Destruct(CKingApp* self, void*)                   // [FUN_4e1e60]
{
    self->Destruct();
}

CKingApp* __fastcall CKingApp_DeletingDestructor(CKingApp* self, void*, unsigned flags) // [$L_4e1e40]
{
    self->Destruct();
    if (flags & 1)
        operator delete(self);
    return self;
}

void __fastcall CKingApp_PreExecute(CKingApp* self, void*)                 // [$L_4e0d50]
{
    self->PreExecute();
}

void __fastcall CKingApp_PostExecute(CKingApp* self, void*)                // [$L_4e0df0]
{
    self->PostExecute();
}

void __fastcall CKingApp_OnPausedKey(CKingApp* self, void*, int key)       // [$L_4e1f00]
{
    self->OnPausedKey(key);
}

void __fastcall CKingApp_DispatchInput(CKingApp* self, void*, InputEvent* ev) // [$L_576440]
{
    self->DispatchInput(ev);
}

}
