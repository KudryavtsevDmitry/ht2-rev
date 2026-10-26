// data.cpp - the program's data that C++ owns now.
#include "king.h"

// The original _BSS segment, byte for byte (king.masm points its labels at
// _KingBss+offset unless ASM_DATA). Zero-initialised like the original.
extern "C" __declspec(align(32)) unsigned char KingBss[KING_BSS_SIZE];
unsigned char KingBss[KING_BSS_SIZE];
