// king_app.cpp - CKingApp, the game's application class, rewritten from
// king.masm. CKingApp::Frame is in mainloop.cpp.

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

    if (g_p6d2078 && g_p6d2078->m_616 && Car_InList(g_p6d2078->m_616)
        && g_p6d2078->m_616->m_21600 && g_p6d2078->m_616->m_21600->m_10048 == 1
        && (g_pMainView->m_flags & WND_VISIBLE))
        CallThis(FUN_53b4c0, g_p6d2078);
}

// After the windows have executed: the engine's overlays, then the joystick
// is read into g_controls and g_joyAxes.
void CKingApp::PostExecute()
{
    CGameApp::PostExecute();
    Loading_Update();
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
// Entry points for the assembly, which uses them in place of the labels in [ ]

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

// === cheat codes typed in the game (FUN_5764f0)

extern "C" void __cdecl Ai_CheatCode(int code);
extern "C" int __cdecl strncmp(const char*, const char*, size_t);
extern "C" size_t __cdecl strlen(const char*);
KING_VAR(int, g_ck6f34e0, 6f34e0)
KING_VAR(char, g_ckTyped6f2c54, 6f2c54)
KING_VAR(const char*, g_ckCodes67b760, 67b760)
KING_VAR(int, g_ck6d2090, 6d2090)
KING_VAR(int, g_ck6d2088, 6d2088)
KING_VAR(int, g_ck6d208c, 6d208c)
KING_VAR(int, g_ck6d2094, 6d2094)
KING_VAR(int, g_ck6ced94, 6ced94)
KING_VAR(int, g_ck6f33f0, 6f33f0)

extern "C" {

// [FUN_5764f0] key k appended to the typed text (6f2c54, its length 6f34e0) and matched against the ten codes
// (67b760): no code starts so, the text is cleared; a whole code sets its cheat flag (6d2090 / 6d2088 / 6d208c /
// 6d2094 / 6ced94), tells the AI (2 or 3), or all of them (and 6f33f0), and clears the text
void __cdecl Cheat_KeyTyped(char k)
{
    char* typed = &g_ckTyped6f2c54;
    const char** codes = &g_ckCodes67b760;
    typed[g_ck6f34e0] = k;
    g_ck6f34e0++;
    typed[g_ck6f34e0] = 0;
    int i = 0;
    while (strncmp(codes[i], typed, g_ck6f34e0))
    {
        i++;
        if (i == 10)
        {
            g_ck6f34e0 = 0;
            return;
        }
    }
    if ((int)strlen(codes[i]) != g_ck6f34e0)
        return;
    switch (i)
    {
    case 0: g_ck6d2090 = 1; break;
    case 1: g_ck6d2088 = 1; break;
    case 2: g_ck6d208c = 1; break;
    case 3: g_ck6d2094 = 1; break;
    case 5: g_ck6ced94 = 1; break;
    case 6: Ai_CheatCode(2); break;
    case 7: Ai_CheatCode(3); break;
    case 8:
        g_ck6d2090 = 1;
        g_ck6d2088 = 1;
        g_ck6d208c = 1;
        g_ck6d2094 = 1;
        g_ck6ced94 = 1;
        Ai_CheatCode(1);
        Ai_CheatCode(2);
        Ai_CheatCode(3);
        g_ck6f33f0 = 1;
        break;
    }
    g_ck6f34e0 = 0;
}

} // extern "C"
