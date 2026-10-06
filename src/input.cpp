// input.cpp - the input event queue, rewritten from king.masm: the message
// handlers of the game's Win32 window put mouse and keyboard messages into
// it, and CGameApp::ProcessInput takes them out once per frame.
//
// Building with  nmake ASM_INPUT=1  makes king.masm assemble the original
// procedures of these functions again.

#include "king.h"

enum { INPUT_QUEUE_SIZE = 100 };

typedef InputEvent InputQueue[INPUT_QUEUE_SIZE];
ASM_VAR(InputQueue, g_inputQueue, $L_702660)
ASM_VAR(int, g_inputRead, $L_702984)           // next event to take
ASM_VAR(int, g_inputWrite, $L_702988)          // next free slot

// The functions declared extern "C" in king.h replace these labels of
// king.masm themselves.

BOOL Input_GetEvent(InputEvent* ev)                 // [FUN_5aca90]
{
    if (g_inputRead == g_inputWrite)
        return TRUE;
    *ev = g_inputQueue[g_inputRead];
    g_inputRead = (g_inputRead + 1) % INPUT_QUEUE_SIZE;
    return FALSE;
}

BOOL Input_PutEvent(const InputEvent* ev)           // [FUN_5acb60]
{
    int next = (g_inputWrite + 1) % INPUT_QUEUE_SIZE;
    if (next == g_inputRead)
        return TRUE;
    g_inputQueue[g_inputWrite] = *ev;
    g_inputWrite = next;
    return FALSE;
}

void Input_SetCursorPos(int x, int y)               // [FUN_5acaf0]
{
    if (!g_hWnd)
        return;
    POINT p = { x, y };
    ClientToScreen(g_hWnd, &p);
    SetCursorPos(p.x, p.y);
}

// The original leaves the fields an event doesn't use unset: the mouse
// position of key events, the key of mouse events.
static void PutKey(UINT msg, UINT key)
{
    InputEvent ev = { (short)msg, 0, 0, (short)key };
    Input_PutEvent(&ev);
}

static void PutMouse(UINT msg, POINT point)
{
    InputEvent ev = { (short)msg, (short)point.x, (short)point.y, 0 };
    Input_PutEvent(&ev);
}

// ---------------------------------------------------------------------------
// Entry points for king.masm, which uses them in place of the labels in [ ].
// The message handlers belong to the game's Win32 window (vtable [$L_6523e4],
// message map [$L_651ee8]); they don't use the window object.

extern "C" {

void __fastcall CMainWnd_OnSysKeyDown(void*, void*, UINT nChar, UINT, UINT) // [$L_5acb30]
{
    PutKey(WM_SYSKEYDOWN, nChar);
}

void __fastcall CMainWnd_OnKeyDown(void*, void*, UINT nChar, UINT, UINT)    // [$L_5acbc0]
{
    PutKey(WM_KEYDOWN, nChar);
}

void __fastcall CMainWnd_OnKeyUp(void*, void*, UINT nChar, UINT, UINT)      // [$L_5acbf0]
{
    PutKey(WM_KEYUP, nChar);
}

void __fastcall CMainWnd_OnChar(void*, void*, UINT nChar, UINT, UINT)       // [$L_5acc20]
{
    PutKey(WM_CHAR, nChar);
}

void __fastcall CMainWnd_OnLButtonDown(void*, void*, UINT, POINT point)     // [$L_5acc50]
{
    PutMouse(WM_LBUTTONDOWN, point);
}

void __fastcall CMainWnd_OnRButtonDown(void*, void*, UINT, POINT point)     // [$L_5acc80]
{
    PutMouse(WM_RBUTTONDOWN, point);
}

void __fastcall CMainWnd_OnLButtonUp(void*, void*, UINT, POINT point)       // [$L_5accb0]
{
    PutMouse(WM_LBUTTONUP, point);
}

void __fastcall CMainWnd_OnRButtonUp(void*, void*, UINT, POINT point)       // [$L_5acce0]
{
    PutMouse(WM_RBUTTONUP, point);
}

void __fastcall CMainWnd_OnMouseMove(void*, void*, UINT, POINT point)       // [$L_5acd10]
{
    PutMouse(WM_MOUSEMOVE, point);
}

}
