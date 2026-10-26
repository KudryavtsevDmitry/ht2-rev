// data.cpp - the program's data that C++ owns now.
#define KING_BSS_SIZE 0xf52e4

// The original _BSS segment, byte for byte (king.masm points its labels at
// _KingBss+offset unless ASM_DATA). Zero-initialised like the original.
#ifndef ASM_DATA
extern "C" __declspec(align(32)) unsigned char KingBss[KING_BSS_SIZE];
unsigned char KingBss[KING_BSS_SIZE];
#endif
