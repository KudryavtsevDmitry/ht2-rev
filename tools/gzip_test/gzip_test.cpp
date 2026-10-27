// gzip_test.cpp - packs and unpacks random buffers through the game's gzip
// (Gz_Compress / Gz_Decompress over memory streams) and writes every packed
// buffer to a file. run.ps1 links it with the game's objects of two commits
// and compares the files: the packed bytes must be the same, and each
// unpacked buffer must equal its input.
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALIAS(cname, label) __pragma(comment(linker, "/alternatename:" cname "=" label))
extern "C" void* __cdecl T_New(void* data, unsigned size);
extern "C" int __cdecl T_Compress(void* in, void* out);
extern "C" int __cdecl T_Decompress(void* in, void* out);
extern "C" int __cdecl T_Close(void** p);
#ifdef OLD_GZIP
ALIAS("_T_New", "FUN_62aaa0") ALIAS("_T_Compress", "FUN_62ab60") ALIAS("_T_Decompress", "FUN_62abc0") ALIAS("_T_Close", "FUN_62a950")
#else
ALIAS("_T_New", "_Gz_MemStreamNew") ALIAS("_T_Compress", "_Gz_Compress") ALIAS("_T_Decompress", "_Gz_Decompress") ALIAS("_T_Close", "_Gz_StreamClose")
#endif

static unsigned g_seed = 12345;
static unsigned Rand() { return g_seed = g_seed * 1103515245 + 12345, (g_seed >> 16) & 0x7fff; }

int main(int argc, char** argv)
{
    int cases = argc > 1 ? atoi(argv[1]) : 300;
    FILE* f = fopen(argc > 2 ? argv[2] : "packed.bin", "wb");
    static unsigned char in[300000], packed[400000], unpacked[300000];
    int bad = 0;
    for (int c = 0; c < cases; c++) {
        unsigned n = c < 4 ? c : Rand() % (c % 10 == 0 ? 300000 : 5000);
        int kind = Rand() % 4;
        for (unsigned i = 0; i < n; i++)
            in[i] = kind == 0 ? (unsigned char)Rand()                      // noise
                  : kind == 1 ? (unsigned char)("abcdefgh"[Rand() % 3])    // few symbols
                  : kind == 2 ? (unsigned char)(i / 7 + (Rand() % 50 == 0)) // runs
                  : (unsigned char)(i < 64 ? Rand() : in[i - 1 - Rand() % 64]);
        void* s = T_New(in, n);
        void* d = T_New(packed, sizeof packed);
        T_Compress(s, d);
        unsigned plen = *(unsigned*)((char*)d + 4);
        T_Close(&s);
        T_Close(&d);
        fwrite(&plen, 4, 1, f);
        fwrite(packed, 1, plen, f);
        s = T_New(packed, plen);
        d = T_New(unpacked, sizeof unpacked);
        int r = T_Decompress(s, d);
        unsigned ulen = *(unsigned*)((char*)d + 4);
        if (r == 0) T_Close(&s);
        T_Close(&d);
        if (r != 0 || ulen != n || memcmp(in, unpacked, n)) {
            printf("case %d: n %u packed %u: round trip failed (r %d, %u bytes)\n", c, n, plen, r, ulen);
            bad++;
        }
    }
    fclose(f);
    printf("%d cases, %d round trip failures\n", cases, bad);
    return bad != 0;
}
