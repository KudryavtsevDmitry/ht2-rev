"""Blind world-loading test: run against a game at its main menu.

Uses client coordinates, scaled from the game's 800x600 menu layout.
Leaves the game running unless --close-after is specified; does not save it.
"""
import ctypes
import argparse
from ctypes import wintypes
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--close-after', type=float, help='Close the window this many seconds after loading.')
parser.add_argument('--load-timeout', type=int, default=30, help='Seconds to wait for loading (increase under a debugger).')
parser.add_argument('--wait-only', action='store_true', help='Wait for an already-started load; do not click the menus.')
args = parser.parse_args()

user32 = ctypes.WinDLL('user32', use_last_error=True)
kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)
user32.FindWindowW.restype = wintypes.HWND
user32.FindWindowW.argtypes = [wintypes.LPCWSTR, wintypes.LPCWSTR]
user32.GetClientRect.argtypes = [wintypes.HWND, ctypes.POINTER(wintypes.RECT)]
user32.ClientToScreen.argtypes = [wintypes.HWND, ctypes.POINTER(wintypes.POINT)]
user32.SetForegroundWindow.argtypes = [wintypes.HWND]
user32.GetWindowThreadProcessId.argtypes = [wintypes.HWND, ctypes.POINTER(wintypes.DWORD)]
kernel32.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
kernel32.OpenProcess.restype = wintypes.HANDLE
kernel32.WaitForSingleObject.argtypes = [wintypes.HANDLE, wintypes.DWORD]
kernel32.CloseHandle.argtypes = [wintypes.HANDLE]
user32.PostMessageW.argtypes = [wintypes.HWND, wintypes.UINT, ctypes.c_size_t, ctypes.c_ssize_t]
kernel32.GetExitCodeProcess.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.DWORD)]
user32.SendMessageTimeoutW.argtypes = [wintypes.HWND, wintypes.UINT, ctypes.c_size_t,
                                    ctypes.c_ssize_t, wintypes.UINT, wintypes.UINT,
                                    ctypes.POINTER(ctypes.c_size_t)]
user32.SendMessageTimeoutW.restype = ctypes.c_ssize_t

window = user32.FindWindowW(None, 'King Of The Road')
if not window:
    raise SystemExit('Game window not found. Open the game at its main menu first.')
pid = wintypes.DWORD()
user32.GetWindowThreadProcessId(window, ctypes.byref(pid))
process = kernel32.OpenProcess(0x100000 | 0x1000, False, pid.value)
if not process:
    raise ctypes.WinError(ctypes.get_last_error())

def click(x, y):
    rect = wintypes.RECT()
    if not user32.GetClientRect(window, ctypes.byref(rect)):
        raise ctypes.WinError(ctypes.get_last_error())
    point = wintypes.POINT(round(x * rect.right / 800), round(y * rect.bottom / 600))
    if not user32.ClientToScreen(window, ctypes.byref(point)):
        raise ctypes.WinError(ctypes.get_last_error())
    user32.SetCursorPos(point.x, point.y)
    time.sleep(0.5)
    user32.mouse_event(0x0002, 0, 0, 0, 0)
    time.sleep(0.15)
    user32.mouse_event(0x0004, 0, 0, 0, 0)
    print(f'Clicked client ({x}, {y}) at screen ({point.x}, {point.y})', flush=True)

try:
    user32.SetForegroundWindow(window)
    time.sleep(1)
    if not args.wait_only:
        click(657, 141)  # ИГРА
        time.sleep(1.5)
        click(398, 140)  # НОВАЯ ИГРА
    stalled = args.wait_only
    recovered = 0
    for second in range(args.load_timeout):
        if kernel32.WaitForSingleObject(process, 0) == 0:
            raise SystemExit('FAIL: game exited during world loading.')
        result = ctypes.c_size_t()
        responsive = bool(user32.SendMessageTimeoutW(window, 0, 0, 0, 2, 100,
                                                     ctypes.byref(result)))
        print(f'{second:02d}s: responding={responsive}', flush=True)
        stalled |= not responsive
        recovered = recovered + 1 if stalled and responsive else 0
        if recovered >= 3:
            print('World-loading stall ended; game remains alive and responsive.')
            if args.close_after is not None:
                if kernel32.WaitForSingleObject(process, round(args.close_after * 1000)) == 0:
                    raise SystemExit('FAIL: game exited before the requested close.')
                print(f'Closing game window after {args.close_after:g} seconds.', flush=True)
                if not user32.PostMessageW(window, 0x0010, 0, 0):  # WM_CLOSE
                    raise ctypes.WinError(ctypes.get_last_error())
                if kernel32.WaitForSingleObject(process, 10000) != 0:
                    raise SystemExit('FAIL: game did not exit after WM_CLOSE.')
                code = wintypes.DWORD()
                kernel32.GetExitCodeProcess(process, ctypes.byref(code))
                print(f'Game exited with code 0x{code.value:08x}.', flush=True)
                if code.value != 0:
                    raise SystemExit('FAIL: game crashed while closing the window.')
            break
        time.sleep(1)
    else:
        raise SystemExit('INCONCLUSIVE: no completed loading transition observed.')
finally:
    kernel32.CloseHandle(process)
