// ai_statics.cpp - the static initializers and exit handlers of the AI
// globals: bare labels in king.masm (init function, then its atexit
// registration; or a destructor applied to the global)

#include "king.h"

extern "C" int __cdecl AiStatic68ceb0_AtExit();
extern "C" int __cdecl Static401e70_AtExit();
extern "C" int __cdecl Static401ec0_AtExit();
extern "C" int __cdecl Static402140_AtExit();
extern "C" int __cdecl Static402480_AtExit();
extern "C" void __cdecl AI_Init68a830();
extern "C" void __cdecl AI_Init68cbe8();
extern "C" void __cdecl AI_Init68cc08();
extern "C" void __cdecl AI_Init68ceb0();
extern "C" void __cdecl AI_Init68cf00();
extern "C" void __fastcall Vec104_Destroy(BYTE**);
extern "C" void __fastcall Vec24Rec_Destroy(BYTE**);
extern "C" void __fastcall Vec4_FreeStorage(BYTE**, int);
extern "C" void __fastcall Vec4_FreeStorageB(BYTE**, int);

// Static initializers and exit handlers of the AI globals (bare labels in king.masm)
// [$L_401e30] at exit: Vec104_Destroy of the global at 68ccb8
extern "C" void __cdecl Ai_Exit401e30() { Vec104_Destroy((BYTE**)((BYTE*)KingBss + 9368)); }
// [$L_401e40] static initializer: AI_Init68cc08, then its exit handler
extern "C" int __cdecl Ai_Init401e40() { AI_Init68cc08(); return Static401e70_AtExit(); }
// [$L_401e80] at exit: Vec24Rec_Destroy of the global at 68cc08
extern "C" void __cdecl Ai_Exit401e80() { Vec24Rec_Destroy((BYTE**)((BYTE*)KingBss + 9192)); }
// [$L_401e90] static initializer: AI_Init68cf00, then its exit handler
extern "C" int __cdecl Ai_Init401e90() { AI_Init68cf00(); return Static401ec0_AtExit(); }
// [$L_402100] at exit: Vec4_FreeStorage of the global at 68cc98
extern "C" void __cdecl Ai_Exit402100() { Vec4_FreeStorage((BYTE**)((BYTE*)KingBss + 9336), 0); }
// [$L_402110] static initializer: AI_Init68cbe8, then its exit handler
extern "C" int __cdecl Ai_Init402110() { AI_Init68cbe8(); return Static402140_AtExit(); }
// [$L_402240] at exit: Vec4_FreeStorage of the global at 68cc18
extern "C" void __cdecl Ai_Exit402240() { Vec4_FreeStorage((BYTE**)((BYTE*)KingBss + 9208), 0); }
// [$L_402250] static initializer: AI_Init68ceb0, then its exit handler
extern "C" int __cdecl Ai_Init402250() { AI_Init68ceb0(); return AiStatic68ceb0_AtExit(); }
// [$L_402430] at exit: Vec4_FreeStorageB of the global at 68cca8
extern "C" void __cdecl Ai_Exit402430() { Vec4_FreeStorageB((BYTE**)((BYTE*)KingBss + 9352), 0); }
// [$L_402440] static initializer: AI_Init68a830, then its exit handler
extern "C" int __cdecl Ai_Init402440() { AI_Init68a830(); return Static402480_AtExit(); }
// [$L_402730] at exit: Vec4_FreeStorageB of the global at 68cc88
extern "C" void __cdecl Ai_Exit402730() { Vec4_FreeStorageB((BYTE**)((BYTE*)KingBss + 9320), 0); }
