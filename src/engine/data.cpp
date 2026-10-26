// data.cpp - the program's data that C++ owns now.
#define KING_BSS_SIZE 0xf52e4

// The original _BSS segment, byte for byte (src\asm reaches it as
// _KingBss+offset). Zero-initialised like the original.
extern "C" __declspec(align(32)) unsigned char KingBss[KING_BSS_SIZE];
unsigned char KingBss[KING_BSS_SIZE];
