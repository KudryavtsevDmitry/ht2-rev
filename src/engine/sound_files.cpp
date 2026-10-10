// sound_files.cpp - the unit at 0x5fdc70 of king.masm: sound buffer slots.
//
// Building with  nmake ASM_SOUND_FILES=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include "x87.h"
#include <oleauto.h>

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef DWORD Dwords[1];
extern "C" int __cdecl rand();

extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")    // unlink from its list
extern "C" void FUN_5d2190(); KING_ALIAS("_FUN_5d2190", "@Node_Link@12")    // link into list (thiscall node, list)
ASM_PROC(FUN_5d2ba0)    // log (text, value)
ASM_PROC(FUN_5c2800)    // msavi: create the sound buffer
extern "C" void FUN_5c36e0(); KING_ALIAS("_FUN_5c36e0", "@Entry_Apply@16")
ASM_PROC(FUN_6021f0)
extern "C" void FUN_602510(); KING_ALIAS("_FUN_602510", "_Dp_SetPlayerName")
extern "C" void FUN_6022a0(); KING_ALIAS("_FUN_6022a0", "_Dp_Shutdown")
ASM_PROC(FUN_602e80)
extern "C" void FUN_5ab160(); KING_ALIAS("_FUN_5ab160", "_File_WriteInt")  // write an int to a file (value, file)
ASM_PROC(FUN_5c3f20)
ASM_PROC(FUN_5f19b0)

// msavi
typedef void(__cdecl* WaveRead)(DWORD size, BYTE* buf, DWORD* got, int ctx);
ASM_VAR(DWORD, g_msaviBufSize, $L_702f64)
ASM_VAR(DWORD, g_msaviLastPos, $L_702f68)
ASM_VAR(DWORD, g_msaviPlayed, $L_702f6c)
ASM_VAR(DWORD, g_msaviWritePos, $L_702f70)
ASM_VAR(Dwords, g_msavi702f78, $L_702f78)
ASM_VAR(int, g_702f94, $L_702f94)
ASM_VAR(BYTE*, g_msavi702f98, $L_702f98)
ASM_VAR(int, g_msavi702f9c, $L_702f9c)
ASM_VAR(BYTE*, g_msaviBuffer, $L_702fa0)     // IDirectSoundBuffer
ASM_VAR(BYTE*, g_msavi702fa4, $L_702fa4)
ASM_VAR(BYTE*, g_msaviFormat, $L_702fa8)     // WAVEFORMATEX
ASM_VAR(int, g_msaviReadCtx, $L_702fac)
ASM_VAR(WaveRead, g_msaviRead, $L_702fb0)
ASM_VAR(AsmLabel, s_msaviNoFormat, $L_67f5c4)
ASM_VAR(AsmLabel, s_msaviNoRead, $L_67f5a0)
ASM_VAR(AsmLabel, s_msaviNoBuffer, $L_67f578)
ASM_VAR(AsmLabel, s_msaviLockFailed, $L_67f748)
ASM_VAR(AsmLabel, s_msaviPosFailed, $L_67f7bc)
ASM_VAR(AsmLabel, s_msaviStatusFailed, $L_67f7fc)
ASM_VAR(AsmLabel, s_msaviFillFailed, $L_67f7e0)

ASM_VAR(AsmLabel, g_vtbl651948, $L_651948)
ASM_VAR(AsmLabel, g_vtbl651b00, $L_651b00)
ASM_VAR(AsmLabel, g_vtbl651d14, $L_651d14)
ASM_VAR(AsmLabel, g_vtbl651d64, $L_651d64)
ASM_VAR(AsmLabel, g_vtbl651db0, $L_651db0)
ASM_VAR(BYTE*, g_707f6c, $L_707f6c)
ASM_VAR(int, g_70ebe8, $L_70ebe8)
ASM_VAR(int, g_70ebec, $L_70ebec)
ASM_VAR(int, g_70ebf0, $L_70ebf0)
ASM_VAR(int, g_70ebf4, $L_70ebf4)
ASM_VAR(int, g_70ebf8, $L_70ebf8)
ASM_VAR(Dwords, g_7025f0, $L_7025f0)
ASM_VAR(Dwords, g_67fa60, $L_67fa60)
ASM_VAR(AsmLabel, s_treeShadow, $L_67f9ac)

#define E_FAIL_ ((int)0x80004005)

static __forceinline void Log(const char* s, int v)
{
    CallC(FUN_5d2ba0, s, v);
}

static __forceinline void Release(BYTE* com)
{
    (*(void(__stdcall**)(BYTE*))(*(BYTE**)com + 8))(com);
}

template <class R = int, class... A>
static __forceinline R Com(BYTE* com, unsigned off, A... args)
{
    return (*(R(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

ASM_PROC(FUN_5cfb30)
ASM_PROC(FUN_5d2ac0)
ASM_PROC(FUN_6037e0)    // time in seconds, returns double
ASM_PROC(FUN_5d35b0)
ASM_PROC(FUN_5d3790)
ASM_PROC(FUN_5d3080)    // next sibling
ASM_PROC(FUN_5d38c0)
ASM_PROC(FUN_404420)
ASM_VAR(int, g_67faa0, $L_67faa0)
ASM_VAR(int, g_71e800, $L_71e800)
ASM_VAR(int, g_71e7d4, $L_71e7d4)
ASM_VAR(int, g_71e7e4, $L_71e7e4)
ASM_VAR(int, g_71e844, $L_71e844)
ASM_VAR(AsmLabel, g_pool71e830, $L_71e830)
ASM_VAR(AsmLabel, g_pool7200c0, $L_7200c0)
typedef int Ints[1];
ASM_VAR(Ints, g_71e858, $L_71e858)
ASM_VAR(AsmLabel, g_720080, $L_720080)
ASM_VAR(AsmLabel, g_vtbl651eb4, $L_651eb4)
ASM_VAR(BYTE*, g_723d44, $L_723d44)
ASM_VAR(CRITICAL_SECTION, g_cs723b40, $L_723b40)
ASM_VAR(AsmLabel, g_683c58, $L_683c58)

extern "C" {

static BYTE* NewEntry()
{
    BYTE* e = (BYTE*)operator new(32);
    if (e) {
        F(int, e, 4) = 0;
        F(int, e, 8) = 0;
        F(void*, e, 0) = g_vtbl651948;
    }
    return e;
}

static __forceinline BYTE* Table_Bucket(BYTE* self, int key)
{
    BYTE* e = F(BYTE*, self, (key % 10) * 32 + 8);
    while (e && F(int, e, 12) != key)
        e = F(BYTE*, e, 8);
    return e;
}

}

extern "C" {

// [FUN_5ffb20]
int __cdecl Obj723d44_Delete()
{
    if (g_723d44)
        CallVirt(g_723d44, 0, 1);
    g_723d44 = 0;
    return 0;
}

// [FUN_5ffc30]
int __cdecl Set683c58(DWORD lo, DWORD hi)
{
    EnterCriticalSection(&g_cs723b40);
    ((DWORD*)g_683c58)[0] = lo;
    ((DWORD*)g_683c58)[1] = hi;
    LeaveCriticalSection(&g_cs723b40);
    return 0;
}

// [FUN_600540]
BYTE* __fastcall Obj600540_Delete(BYTE* self, void*, int flags)
{
    CallThis(FUN_404420, self);
    if (flags & 1)
        operator delete(self);
    return self;
}

}

