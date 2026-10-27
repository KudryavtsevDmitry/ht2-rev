// gzip.cpp - the rest of gzip 1.2.4 linked into king.exe (FUN_62a910 ..
// FUN_62d960): memory streams, the compress/decompress entry points, the
// header (get_method), zip, deflate, unzip, unpack, unlzw, unlzh and the
// I/O helpers of util.c. inflate, trees.c and bits.c are in terrain_grid.cpp.
// The network code packs its messages with Gz_Compress / Gz_Decompress.

#include "king.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
#define GZ_STR(a) ((const char*)KING_ADDR(0x##a))

typedef int (__cdecl* GzWork)(void* in, void* out);
typedef char AsmLabel[1];

// options and state of gzip.c
KING_VAR(int, g_gzAscii, 77afd0)
KING_VAR(int, g_gzToStdout, 77afd4)
KING_VAR(int, g_gzDecompress, 77afd8)
KING_VAR(int, g_gzForce, 77afdc)
KING_VAR(int, g_gzRecursive, 77afe0)
KING_VAR(int, g_gzList, 77afe4)
KING_VAR(int, g_gzVerbose, 77afe8)
KING_VAR(int, g_gzQuiet, 77afec)
KING_VAR(int, g_gzDoLzw, 77aff0)
KING_VAR(int, g_gzTest, 77aff4)
KING_VAR(int, g_gzExitCode, 77aff8)
KING_VAR(void*, g_gzArgs, 77affc)
KING_VAR(int, g_gzRemoveOfname, 77b008)
KING_VAR(int, g_gzInitDone, 77b00c)
KING_VAR(int, g_gzInExit, 77b010)
KING_VAR(DWORD, g_gzCrc, 77b014)
KING_VAR(long, g_gzHeaderBytes, 77b018)
KING_VAR(void*, g_gzEnv, 72e62c)
KING_VAR(void*, g_gzIfd, 72e630)
KING_VAR(void*, g_gzOfd, 736674)
KING_VAR(int, g_gzPartNb, 73667c)
KING_VAR(DWORD, g_gzTimeStamp, 72e520)
KING_VAR(int, g_gzSaveOrigName, 72e524)
KING_VAR(AsmLabel, g_gzOfname, 72e528)            // [260]
KING_VAR(AsmLabel, g_gzOfnameEnd, 72e62c)
KING_VAR(AsmLabel, g_gzIfname, 76ae94)
KING_VAR(const char*, g_gzProgname, 76af9c)
KING_VAR(int, g_gzLastMember, 76ae90)
KING_VAR(long, g_gzBytesOut, 73ae8c)
KING_VAR(long, g_gzBytesIn, 76af98)                // also isize
KING_VAR(int, g_gzNoName, 6897c8)
KING_VAR(int, g_gzNoTime, 6897cc)
KING_VAR(int, g_gzMaxbits, 6897d0)
KING_VAR(int, g_gzMethod, 6897d4)
KING_VAR(int, g_gzLevel, 6897d8)
KING_VAR(GzWork, g_gzWork, 689960)
KING_VAR(WORD, g_gzMagicGzip, 689c8c)
KING_VAR(WORD, g_gzMagicOldGzip, 689c88)
KING_VAR(DWORD, g_gzMagicPkzip, 689c80)
KING_VAR(WORD, g_gzMagicPack, 689c7c)
KING_VAR(WORD, g_gzMagicLzw, 689c78)
KING_VAR(WORD, g_gzMagicLzh, 689c74)
KING_VAR(DWORD, g_gzCrcState, 689c90)
KING_VAR(DWORD, g_gzCrcTable, 689c94)             // [256]
KING_VAR(BYTE, g_gzStderr, 686380)                // the old runtime's stderr

// buffers (shared with terrain_grid.cpp)
KING_VAR(BYTE, g_gzInbuf, 72e634)                 // [0x8000 + 64]
KING_VAR(unsigned, g_gzInsize, 736680)
KING_VAR(unsigned, g_gzInptr, 736684)
KING_VAR(unsigned, g_gzOutcnt, 736688)
KING_VAR(BYTE, g_gzOutbuf, 73668c)                // [16384 + 2048]
KING_VAR(BYTE, g_gzWindow, 75ae90)                // [0x10000]
KING_VAR(WORD, g_gzPrev, 73ae90)                  // also unlzw's tab_prefix, unlzh's left
KING_VAR(WORD, g_gzHead, 74ae90)                  // also unlzh's right
KING_VAR(WORD, g_gzDBuf, 76afd0)                  // unlzh's c_table
KING_VAR(BYTE, g_gzDeStack, 77afce)               // unlzw's stack end (&d_buf[0x7fff])

#define GZ_INBUFSIZ 0x8000
#define GZ_OUTBUFSIZ 16384
#define GZ_WSIZE 0x8000

extern "C" {
int __cdecl OldCrt_fprintf(void* f, const char* format, ...);
void __cdecl Gz_BiInit(int zipfile);
void __cdecl Gz_CtInit(WORD* attr, int* methodp);
int __cdecl Gz_Inflate();
int __cdecl Gz_LongestMatch(unsigned cur_match);
void __cdecl Gz_FillWindow();
DWORD __cdecl Gz_FlushBlock(const char* buf, DWORD stored_len, int eof);
int __cdecl Gz_CtTally(int dist, int lc);
int __cdecl Gz_FillInbuf(int eof_ok);
void __cdecl Gz_FlushWindow();
int __cdecl Gz_Zip(void* in, void* out);
}

#define GZ_STDERR ((void*)&g_gzStderr)

// ---------------------------------------------------------------------------
// memory streams: vtable $L_655260 {read, write, seek, is null}; +4 position,
// +8 size, +12 data, +16 owns (1)

typedef int (__fastcall* StreamIo)(void* self, void*, void* buf, unsigned n);
typedef int (__fastcall* StreamIsNull)(void* self);
KING_VAR(BYTE, g_memStreamVtbl, 655260)

extern "C" {

// [$L_62a9b0] read: up to n bytes from the position
int __fastcall GzStream_Read(BYTE* s, void*, void* buf, unsigned n)
{
    BYTE* data = F(BYTE*, s, 12);
    if (!data) return -1;
    unsigned pos = F(unsigned, s, 4);
    if (pos + n >= F(unsigned, s, 8)) n = F(unsigned, s, 8) - pos;
    memcpy(buf, data + pos, n);
    F(unsigned, s, 4) += n;
    return n;
}

// [$L_62aa00] write: n bytes at the position (no bounds check)
int __fastcall GzStream_Write(BYTE* s, void*, const void* buf, unsigned n)
{
    BYTE* data = F(BYTE*, s, 12);
    if (!data) return -1;
    memcpy(data + F(unsigned, s, 4), buf, n);
    F(unsigned, s, 4) += n;
    return n;
}

// [$L_62aa40] seek (0 set, 1 current, 2 from the end)
int __fastcall GzStream_Seek(BYTE* s, void*, int off, int whence)
{
    if (!F(BYTE*, s, 12)) return -1;
    if (whence == 1) return F(int, s, 4) += off;
    if (whence == 2) return F(int, s, 4) = F(int, s, 8) - off;
    if (whence == 0) return F(int, s, 4) = off;
    return -1;
}

// [$L_62aa90]
int __fastcall GzStream_IsNull(BYTE* s) { return F(BYTE*, s, 12) == 0; }

// [FUN_62a980] constructor
BYTE* __fastcall GzStream_Construct(BYTE* s, void*, BYTE* data, unsigned size)
{
    F(BYTE, s, 16) = 1;
    F(void*, s, 0) = &g_memStreamVtbl;
    F(BYTE*, s, 12) = data;
    F(unsigned, s, 4) = 0;
    F(unsigned, s, 8) = size;
    return s;
}

// [FUN_62aaa0] a new stream over size bytes at data
BYTE* __cdecl Gz_MemStreamNew(BYTE* data, unsigned size)
{
    BYTE* s = (BYTE*)operator new(20);
    return s ? GzStream_Construct(s, 0, data, size) : 0;
}

// [FUN_62a950] deletes *p and clears it
int __cdecl Gz_StreamClose(BYTE** p)
{
    (*(StreamIsNull*)(F(BYTE*, *p, 0) + 12))(*p);
    operator delete(*p);
    *p = 0;
    return 0;
}

// [FUN_62a910] read(fd, buf, n): -1 without a stream
int __cdecl Gz_StreamRead(BYTE* s, void* buf, unsigned n)
{
    if (!s) return -1;
    return (*(StreamIo*)F(BYTE*, s, 0))(s, 0, buf, n);
}

// [FUN_62a930] write(fd, buf, n): n without a stream
int __cdecl Gz_StreamWrite(BYTE* s, void* buf, unsigned n)
{
    if (!s) return n;
    return (*(StreamIo*)(F(BYTE*, s, 0) + 4))(s, 0, buf, n);
}

// ---------------------------------------------------------------------------
// util.c

// [FUN_62b3c0] do_exit
__declspec(noreturn) void __cdecl Gz_DoExit(int code)
{
    if (g_gzInExit) exit(code);
    g_gzInExit = 1;
    if (g_gzEnv) { free(g_gzEnv); g_gzEnv = 0; }
    if (g_gzArgs) { free(g_gzArgs); g_gzArgs = 0; }
    exit(code);
}

// [$L_62b420] abort_gzip
__declspec(noreturn) void __cdecl Gz_Abort()
{
    if (g_gzRemoveOfname) {
        Gz_StreamClose((BYTE**)&g_gzOfd);
        remove(g_gzOfname);
    }
    Gz_DoExit(1);
}

// [FUN_62bad0] error
__declspec(noreturn) void __cdecl Gz_Error(const char* m)
{
    OldCrt_fprintf(GZ_STDERR, GZ_STR(68a0a4), g_gzProgname, g_gzIfname, m);    // "\n%s: %s: %s\n"
    Gz_Abort();
}

// [FUN_62bb00] read_error
__declspec(noreturn) void __cdecl Gz_ReadError()
{
    OldCrt_fprintf(GZ_STDERR, GZ_STR(68a0d0), g_gzProgname);                  // "\n%s: "
    if (errno != 0) perror(g_gzIfname);
    else OldCrt_fprintf(GZ_STDERR, GZ_STR(68a0b4), g_gzIfname);                // unexpected end of file
    Gz_Abort();
}

// [FUN_62bb50] write_error
__declspec(noreturn) void __cdecl Gz_WriteError()
{
    OldCrt_fprintf(GZ_STDERR, GZ_STR(68a0d0), g_gzProgname);
    perror(g_gzOfname);
    Gz_Abort();
}

// [FUN_62b870] updcrc: the CRC-32 so far, restarted by s == 0
DWORD __cdecl Gz_UpdCrc(const BYTE* s, unsigned n)
{
    DWORD c;
    if (!s) {
        c = 0xffffffff;
    } else {
        c = g_gzCrcState;
        for (; n; n--) c = (&g_gzCrcTable)[(c ^ *s++) & 0xff] ^ (c >> 8);
    }
    g_gzCrcState = c;
    return c ^ 0xffffffff;
}

// [FUN_62b8c0] clear_bufs
void __cdecl Gz_ClearBufs()
{
    g_gzOutcnt = 0;
    g_gzInptr = 0;
    g_gzInsize = 0;
    g_gzBytesOut = 0;
    g_gzBytesIn = 0;
}

// [FUN_62b8e0] fill_inbuf: the input buffer refilled, its first byte
// (EOF = -1 when eof_ok)
int __cdecl Gz_FillInbuf(int eof_ok)
{
    g_gzInsize = 0;
    errno = 0;
    do {
        int len = Gz_StreamRead((BYTE*)g_gzIfd, &g_gzInbuf + g_gzInsize, GZ_INBUFSIZ - g_gzInsize);
        if (len == 0 || len == -1) break;
        g_gzInsize += len;
    } while (g_gzInsize < GZ_INBUFSIZ);
    if (g_gzInsize == 0) {
        if (eof_ok) return -1;
        Gz_ReadError();
    }
    g_gzBytesIn += g_gzInsize;
    g_gzInptr = 1;
    return (&g_gzInbuf)[0];
}

// [FUN_62ba40] write_buf: all of cnt bytes
void __cdecl Gz_WriteBuf(void* fd, BYTE* buf, unsigned cnt)
{
    unsigned n;
    while ((n = Gz_StreamWrite((BYTE*)fd, buf, cnt)) != cnt) {
        if (n == (unsigned)-1) Gz_WriteError();
        cnt -= n;
        buf += n;
    }
}

// [FUN_62b9a0] flush_outbuf
void __cdecl Gz_FlushOutbuf()
{
    if (g_gzOutcnt == 0) return;
    Gz_WriteBuf(g_gzOfd, &g_gzOutbuf, g_gzOutcnt);
    g_gzBytesOut += g_gzOutcnt;
    g_gzOutcnt = 0;
}

// [FUN_62b9e0] flush_window: the window's output, with its CRC
void __cdecl Gz_FlushWindow()
{
    if (g_gzOutcnt == 0) return;
    Gz_UpdCrc(&g_gzWindow, g_gzOutcnt);
    if (!g_gzTest) Gz_WriteBuf(g_gzOfd, &g_gzWindow, g_gzOutcnt);
    g_gzBytesOut += g_gzOutcnt;
    g_gzOutcnt = 0;
}

// [FUN_62ba90] basename: after the last '/', '\\' or ':'
char* __cdecl Gz_Basename(char* name)
{
    char* p;
    if ((p = strrchr(name, '/')) != 0) name = p + 1;
    if ((p = strrchr(name, '\\')) != 0) name = p + 1;
    if ((p = strrchr(name, ':')) != 0) name = p + 1;
    return name;
}

// [$L_62b770] file_read: zip's input, counted into the CRC and isize
int __cdecl Gz_FileRead(char* buf, unsigned size)
{
    unsigned len = Gz_StreamRead((BYTE*)g_gzIfd, buf, size);
    if (len == (unsigned)-1 || len == 0) return (int)len;
    g_gzCrc = Gz_UpdCrc((BYTE*)buf, len);
    g_gzBytesIn += len;
    return (int)len;
}

} // extern "C"

static int Gz_GetByte() { return g_gzInptr < g_gzInsize ? (&g_gzInbuf)[g_gzInptr++] : Gz_FillInbuf(0); }
static int Gz_TryByte() { return g_gzInptr < g_gzInsize ? (&g_gzInbuf)[g_gzInptr++] : Gz_FillInbuf(1); }

static void Gz_PutByte(BYTE c)
{
    (&g_gzOutbuf)[g_gzOutcnt++] = c;
    if (g_gzOutcnt == GZ_OUTBUFSIZ) Gz_FlushOutbuf();
}

static void Gz_PutShort(unsigned w)
{
    if (g_gzOutcnt < GZ_OUTBUFSIZ - 2) {
        (&g_gzOutbuf)[g_gzOutcnt++] = (BYTE)(w & 0xff);
        (&g_gzOutbuf)[g_gzOutcnt++] = (BYTE)((WORD)w >> 8);
    } else {
        Gz_PutByte((BYTE)(w & 0xff));
        Gz_PutByte((BYTE)((WORD)w >> 8));
    }
}

static void Gz_PutLong(DWORD n)
{
    Gz_PutShort(n & 0xffff);
    Gz_PutShort(n >> 16);
}

static void Gz_PutWindowByte(BYTE c)
{
    (&g_gzWindow)[g_gzOutcnt++] = c;
    if (g_gzOutcnt == GZ_WSIZE) Gz_FlushWindow();
}

static DWORD Gz_LG(const BYTE* p) { return p[0] | (p[1] << 8) | (p[2] << 16) | ((DWORD)p[3] << 24); }

// ---------------------------------------------------------------------------
// deflate.c

KING_VAR(int, g_gzComprLevel, 77b4e4)
KING_VAR(unsigned, g_gzMaxLazyMatch, 77b4d0)
KING_VAR(unsigned, g_gzGoodMatch, 77b4e8)
KING_VAR(int, g_gzNiceMatch, 77b4dc)
KING_VAR(unsigned, g_gzMaxChain, 77b4e0)
KING_VAR(unsigned, g_gzStrstart, 77b4cc)
KING_VAR(long, g_gzBlockStart, 77b4f0)
KING_VAR(unsigned, g_gzLookahead, 77b4f4)
KING_VAR(int, g_gzEofile, 77b4d8)
KING_VAR(unsigned, g_gzInsH, 77b4d4)
KING_VAR(unsigned, g_gzPrevLength, 77b4c8)
KING_VAR(unsigned, g_gzMatchStart, 77b4ec)
typedef int (__cdecl* GzReadBuf)(char* buf, unsigned size);
KING_VAR(GzReadBuf, g_gzReadBuf, 77e3f0)
struct GzConfig { WORD good_length, max_lazy, nice_length, max_chain; };
KING_VAR(GzConfig, g_gzConfig, 68a3a8)            // [10]

#define GZ_MIN_LOOKAHEAD 262
#define GZ_MAX_DIST (GZ_WSIZE - GZ_MIN_LOOKAHEAD)
#define GZ_HASH_MASK 0x7fff
#define GZ_UPDATE_HASH(h, c) (h = (((h) << 5) ^ (c)) & GZ_HASH_MASK)

static unsigned Gz_InsertString(unsigned s)
{
    GZ_UPDATE_HASH(g_gzInsH, (&g_gzWindow)[s + 2]);
    unsigned match_head = (&g_gzHead)[g_gzInsH];
    (&g_gzPrev)[s & (GZ_WSIZE - 1)] = (WORD)match_head;
    (&g_gzHead)[g_gzInsH] = (WORD)s;
    return match_head;
}

static DWORD Gz_FlushBlockAt(int eof)
{
    return Gz_FlushBlock(g_gzBlockStart >= 0 ? (char*)&(&g_gzWindow)[(unsigned)g_gzBlockStart] : (char*)0,
                         (long)g_gzStrstart - g_gzBlockStart, eof);
}

static void Gz_FillLookahead()
{
    while (g_gzLookahead < GZ_MIN_LOOKAHEAD && !g_gzEofile) Gz_FillWindow();
}

extern "C" {

// [FUN_62d2e0] lm_init
void __cdecl Gz_LmInit(int pack_level, WORD* flags)
{
    if (pack_level < 1 || pack_level > 9) Gz_Error(GZ_STR(68a3f8));    // "bad pack level"
    g_gzComprLevel = pack_level;
    memset(&g_gzHead, 0, 0x8000 * sizeof(WORD));
    GzConfig& c = (&g_gzConfig)[pack_level];
    g_gzMaxLazyMatch = c.max_lazy;
    g_gzGoodMatch = c.good_length;
    g_gzNiceMatch = c.nice_length;
    g_gzMaxChain = c.max_chain;
    if (pack_level == 1) *flags |= 4;                // FAST
    else if (pack_level == 9) *flags |= 2;           // SLOW
    g_gzStrstart = 0;
    g_gzBlockStart = 0;
    g_gzLookahead = g_gzReadBuf((char*)&g_gzWindow, 2 * GZ_WSIZE);
    if (g_gzLookahead == 0 || g_gzLookahead == (unsigned)-1) {
        g_gzEofile = 1;
        g_gzLookahead = 0;
        return;
    }
    g_gzEofile = 0;
    Gz_FillLookahead();
    g_gzInsH = 0;
    for (unsigned j = 0; j < 2; j++) GZ_UPDATE_HASH(g_gzInsH, (&g_gzWindow)[j]);
}

// [FUN_62d960] deflate_fast: no lazy matching (levels 1-3)
DWORD __cdecl Gz_DeflateFast()
{
    unsigned match_length = 0;
    g_gzPrevLength = 2;
    while (g_gzLookahead != 0) {
        unsigned hash_head = Gz_InsertString(g_gzStrstart);
        if (hash_head != 0 && g_gzStrstart - hash_head <= GZ_MAX_DIST) {
            match_length = Gz_LongestMatch(hash_head);
            if (match_length > g_gzLookahead) match_length = g_gzLookahead;
        }
        int flush;
        if (match_length >= 3) {
            flush = Gz_CtTally(g_gzStrstart - g_gzMatchStart, match_length - 3);
            g_gzLookahead -= match_length;
            if (match_length <= g_gzMaxLazyMatch) {
                match_length--;
                do {
                    g_gzStrstart++;
                    Gz_InsertString(g_gzStrstart);
                } while (--match_length != 0);
                g_gzStrstart++;
            } else {
                g_gzStrstart += match_length;
                match_length = 0;
                g_gzInsH = (&g_gzWindow)[g_gzStrstart];
                GZ_UPDATE_HASH(g_gzInsH, (&g_gzWindow)[g_gzStrstart + 1]);
            }
        } else {
            flush = Gz_CtTally(0, (&g_gzWindow)[g_gzStrstart]);
            g_gzLookahead--;
            g_gzStrstart++;
        }
        if (flush) {
            Gz_FlushBlockAt(0);
            g_gzBlockStart = g_gzStrstart;
        }
        Gz_FillLookahead();
    }
    return Gz_FlushBlockAt(1);
}

// [FUN_62d650] deflate: lazy matching (levels 4-9)
DWORD __cdecl Gz_Deflate()
{
    int match_available = 0;
    unsigned match_length = 2;
    if (g_gzComprLevel <= 3) return Gz_DeflateFast();
    while (g_gzLookahead != 0) {
        unsigned hash_head = Gz_InsertString(g_gzStrstart);
        g_gzPrevLength = match_length;
        unsigned prev_match = g_gzMatchStart;
        match_length = 2;
        if (hash_head != 0 && g_gzPrevLength < g_gzMaxLazyMatch && g_gzStrstart - hash_head <= GZ_MAX_DIST) {
            match_length = Gz_LongestMatch(hash_head);
            if (match_length > g_gzLookahead) match_length = g_gzLookahead;
            if (match_length == 3 && g_gzStrstart - g_gzMatchStart > 4096) match_length--;
        }
        if (g_gzPrevLength >= 3 && match_length <= g_gzPrevLength) {
            int flush = Gz_CtTally(g_gzStrstart - 1 - prev_match, g_gzPrevLength - 3);
            g_gzLookahead -= g_gzPrevLength - 1;
            g_gzPrevLength -= 2;
            do {
                g_gzStrstart++;
                Gz_InsertString(g_gzStrstart);
            } while (--g_gzPrevLength != 0);
            match_available = 0;
            match_length = 2;
            g_gzStrstart++;
            if (flush) {
                Gz_FlushBlockAt(0);
                g_gzBlockStart = g_gzStrstart;
            }
        } else if (match_available) {
            if (Gz_CtTally(0, (&g_gzWindow)[g_gzStrstart - 1])) {
                Gz_FlushBlockAt(0);
                g_gzBlockStart = g_gzStrstart;
            }
            g_gzStrstart++;
            g_gzLookahead--;
        } else {
            match_available = 1;
            g_gzStrstart++;
            g_gzLookahead--;
        }
        Gz_FillLookahead();
    }
    if (match_available) Gz_CtTally(0, (&g_gzWindow)[g_gzStrstart - 1]);
    return Gz_FlushBlockAt(1);
}

// ---------------------------------------------------------------------------
// zip.c

// [FUN_62b450] zip: the gzip header, the deflated data, CRC and length
int __cdecl Gz_Zip(void* in, void* out)
{
    BYTE flags = 0;
    WORD attr = 0;
    WORD deflate_flags = 0;
    g_gzIfd = in;
    g_gzOfd = out;
    g_gzOutcnt = 0;
    g_gzMethod = 8;
    Gz_PutByte(((BYTE*)&g_gzMagicGzip)[0]);
    Gz_PutByte(((BYTE*)&g_gzMagicGzip)[1]);
    Gz_PutByte(8);
    if (g_gzSaveOrigName) flags |= 8;
    Gz_PutByte(flags);
    Gz_PutLong(g_gzTimeStamp);
    g_gzCrc = Gz_UpdCrc(0, 0);
    Gz_BiInit((int)out);
    Gz_CtInit(&attr, &g_gzMethod);
    Gz_LmInit(g_gzLevel, &deflate_flags);
    Gz_PutByte((BYTE)deflate_flags);
    Gz_PutByte(11);                                 // OS_CODE: Win32
    if (g_gzSaveOrigName) {
        char* p = Gz_Basename(g_gzIfname);
        do {
            Gz_PutByte(*p);
        } while (*p++);
    }
    g_gzHeaderBytes = (long)g_gzOutcnt;
    Gz_Deflate();
    Gz_PutLong(g_gzCrc);
    Gz_PutLong(g_gzBytesIn);
    g_gzHeaderBytes += 8;
    Gz_FlushOutbuf();
    return 0;
}

// [$L_62b7c0] copy: stored input passed through
int __cdecl Gz_Copy(void* in, void* out)
{
    errno = 0;
    while (g_gzInsize != 0 && (int)g_gzInsize != -1) {
        Gz_WriteBuf(out, &g_gzInbuf, g_gzInsize);
        g_gzBytesOut += g_gzInsize;
        g_gzInsize = Gz_StreamRead((BYTE*)in, &g_gzInbuf, GZ_INBUFSIZ);
    }
    if ((int)g_gzInsize == -1 && errno != 0) Gz_ReadError();
    g_gzBytesIn = g_gzBytesOut;
    return 0;
}

// ---------------------------------------------------------------------------
// unzip.c

KING_VAR(int, g_gzDecrypt, 77b4bc)
KING_VAR(int, g_gzPkzip, 77b4c0)
KING_VAR(int, g_gzExtHeader, 77b4c4)

// [FUN_62cd70] check_zipfile: a PKZIP local header, stored or deflated
int __cdecl Gz_CheckZipfile(void* in)
{
    BYTE* h = &g_gzInbuf + g_gzInptr;
    g_gzIfd = in;
    g_gzInptr += 30 + (h[26] | (h[27] << 8)) + (h[28] | (h[29] << 8));
    if (g_gzInptr > g_gzInsize || Gz_LG(h) != 0x04034b50) {
        OldCrt_fprintf(GZ_STDERR, GZ_STR(68a20c), g_gzProgname, g_gzIfname);    // not a valid zip file
        g_gzExitCode = 1;
        return 1;
    }
    g_gzMethod = h[8];
    if (g_gzMethod != 0 && g_gzMethod != 8) {
        OldCrt_fprintf(GZ_STDERR, GZ_STR(68a254), g_gzProgname, g_gzIfname);    // first entry not deflated or stored
        g_gzExitCode = 1;
        return 1;
    }
    if ((g_gzDecrypt = h[6] & 1) != 0) {
        OldCrt_fprintf(GZ_STDERR, GZ_STR(68a22c), g_gzProgname, g_gzIfname);    // encrypted file
        g_gzExitCode = 1;
        return 1;
    }
    g_gzExtHeader = (h[6] >> 3) & 1;
    g_gzPkzip = 1;
    return 0;
}

// [$L_62ce90] unzip: inflated or stored data, its CRC and length checked
int __cdecl Gz_Unzip(void* in, void* out)
{
    DWORD orig_crc = 0, orig_len = 0;
    BYTE buf[16];
    g_gzIfd = in;
    g_gzOfd = out;
    Gz_UpdCrc(0, 0);
    if (g_gzPkzip && !g_gzExtHeader) {
        orig_crc = Gz_LG(&g_gzInbuf + 14);
        orig_len = Gz_LG(&g_gzInbuf + 22);
    }
    if (g_gzMethod == 8) {
        int res = Gz_Inflate();
        if (res == 3) Gz_Error(GZ_STR(68a094));                       // out of memory
        else if (res != 0) Gz_Error(GZ_STR(68a374));                  // format violated
    } else if (g_gzPkzip && g_gzMethod == 0) {
        DWORD n = Gz_LG(&g_gzInbuf + 22);
        DWORD siz = Gz_LG(&g_gzInbuf + 18);
        if (n != siz - (g_gzDecrypt ? 12 : 0)) {
            OldCrt_fprintf(GZ_STDERR, GZ_STR(68a360), n, siz);        // "len %ld, siz %ld\n"
            Gz_Error(GZ_STR(68a334));                                 // length mismatch
        }
        while (n--) Gz_PutWindowByte((BYTE)Gz_GetByte());
        Gz_FlushWindow();
    } else {
        Gz_Error(GZ_STR(68a314));                                     // internal error, invalid method
    }
    if (!g_gzPkzip) {
        for (int n = 0; n < 8; n++) buf[n] = (BYTE)Gz_GetByte();
        orig_crc = Gz_LG(buf);
        orig_len = Gz_LG(buf + 4);
    } else if (g_gzExtHeader) {
        for (int n = 0; n < 16; n++) buf[n] = (BYTE)Gz_GetByte();
        orig_crc = Gz_LG(buf + 4);
        orig_len = Gz_LG(buf + 12);
    }
    if (orig_crc != Gz_UpdCrc(&g_gzOutbuf, 0)) Gz_Error(GZ_STR(68a2f0));     // crc error
    if (orig_len != (DWORD)g_gzBytesOut) Gz_Error(GZ_STR(68a190));           // length error
    if (g_gzPkzip && g_gzInptr + 4 < g_gzInsize && Gz_LG(&g_gzInbuf + g_gzInptr) == 0x04034b50) {
        if (g_gzToStdout) {
            if (!g_gzQuiet) OldCrt_fprintf(GZ_STDERR, GZ_STR(68a2c0), g_gzProgname, g_gzIfname);
            if (g_gzExitCode == 0) g_gzExitCode = 2;
        } else {
            OldCrt_fprintf(GZ_STDERR, GZ_STR(68a290), g_gzProgname, g_gzIfname);
            g_gzExitCode = 1;
            g_gzExtHeader = g_gzPkzip = 0;
            return 1;
        }
    }
    g_gzExtHeader = g_gzPkzip = 0;
    return 0;
}

} // extern "C"

// ---------------------------------------------------------------------------
// unpack.c

KING_VAR(DWORD, g_gzPackOrigLen, 77b444)
KING_VAR(int, g_gzPackMaxLen, 77b440)
KING_VAR(int, g_gzPackLeaves, 77b26c)             // [26]
KING_VAR(BYTE, g_gzPackLiteral, 77b2d4)           // [256]
KING_VAR(int, g_gzPackLitBase, 77b448)            // [26]
KING_VAR(int, g_gzPackParents, 77b3d8)            // [26]
KING_VAR(int, g_gzPackPeekBits, 77b4b4)
KING_VAR(DWORD, g_gzPackBitbuf, 77b3d4)
KING_VAR(int, g_gzPackValid, 77b4b0)

// [FUN_62cb10] read_tree
static void Gz_ReadTree()
{
    g_gzPackOrigLen = 0;
    for (int n = 1; n <= 4; n++) g_gzPackOrigLen = (g_gzPackOrigLen << 8) | (DWORD)Gz_GetByte();
    g_gzPackMaxLen = Gz_GetByte();
    if (g_gzPackMaxLen > 25) Gz_Error(GZ_STR(68a1d8));                       // Huffman code > 32 bits
    int n = 0;
    for (int len = 1; len <= g_gzPackMaxLen; len++) {
        (&g_gzPackLeaves)[len] = Gz_GetByte();
        n += (&g_gzPackLeaves)[len];
    }
    if (n > 256) Gz_Error(GZ_STR(68a1b8));                                    // too many leaves
    (&g_gzPackLeaves)[g_gzPackMaxLen]++;
    int base = 0;
    for (int len = 1; len <= g_gzPackMaxLen; len++) {
        (&g_gzPackLitBase)[len] = base;
        for (n = (&g_gzPackLeaves)[len]; n > 0; n--) (&g_gzPackLiteral)[base++] = (BYTE)Gz_GetByte();
    }
    (&g_gzPackLeaves)[g_gzPackMaxLen]++;
}

// [FUN_62ccc0] build_tree: parents, literal bases and the prefix_len
// lookup (in outbuf)
static void Gz_BuildTree()
{
    int nodes = 0;
    for (int len = g_gzPackMaxLen; len >= 1; len--) {
        nodes >>= 1;
        (&g_gzPackParents)[len] = nodes;
        (&g_gzPackLitBase)[len] -= nodes;
        nodes += (&g_gzPackLeaves)[len];
    }
    g_gzPackPeekBits = g_gzPackMaxLen < 12 ? g_gzPackMaxLen : 12;
    BYTE* prefixp = &g_gzOutbuf + (1 << g_gzPackPeekBits);
    for (int len = 1; len <= g_gzPackPeekBits; len++) {
        int prefixes = (&g_gzPackLeaves)[len] << (g_gzPackPeekBits - len);
        while (prefixes--) *--prefixp = (BYTE)len;
    }
    while (prefixp > &g_gzOutbuf) *--prefixp = 0;
}

static unsigned Gz_PackLook(int bits, unsigned mask)
{
    while (g_gzPackValid < bits) {
        g_gzPackBitbuf = (g_gzPackBitbuf << 8) | (DWORD)Gz_GetByte();
        g_gzPackValid += 8;
    }
    return (g_gzPackBitbuf >> (g_gzPackValid - bits)) & mask;
}

extern "C" {

// [$L_62c920] unpack: pack's Huffman coded data
int __cdecl Gz_Unpack(void* in, void* out)
{
    g_gzIfd = in;
    g_gzOfd = out;
    Gz_ReadTree();
    Gz_BuildTree();
    g_gzPackValid = 0;
    g_gzPackBitbuf = 0;
    unsigned peek_mask = (1 << g_gzPackPeekBits) - 1;
    unsigned eob = (&g_gzPackLeaves)[g_gzPackMaxLen] - 1;
    for (;;) {
        unsigned peek = Gz_PackLook(g_gzPackPeekBits, peek_mask);
        int len = (&g_gzOutbuf)[peek];
        if (len > 0) {
            peek >>= g_gzPackPeekBits - len;
        } else {
            unsigned mask = peek_mask;
            len = g_gzPackPeekBits;
            do {
                len++, mask = (mask << 1) + 1;
                peek = Gz_PackLook(len, mask);
            } while (peek < (unsigned)(&g_gzPackParents)[len]);
        }
        if (peek == eob && len == g_gzPackMaxLen) break;
        Gz_PutWindowByte((&g_gzPackLiteral)[peek + (&g_gzPackLitBase)[len]]);
        g_gzPackValid -= len;
    }
    Gz_FlushWindow();
    if (g_gzPackOrigLen != (DWORD)g_gzBytesOut) Gz_Error(GZ_STR(68a190));    // length error
    return 0;
}

// ---------------------------------------------------------------------------
// unlzw.c

KING_VAR(int, g_gzBlockMode, 68a0e8)

// [$L_62c3b0] unlzw: compress's LZW data
int __cdecl Gz_Unlzw(void* in, void* out)
{
    WORD* tab_prefix = &g_gzPrev;
    BYTE* tab_suffix = &g_gzWindow;
    BYTE* de_stack = &g_gzDeStack;
    BYTE* outbuf = &g_gzOutbuf;
    BYTE* inbuf = &g_gzInbuf;

    g_gzMaxbits = Gz_GetByte();
    g_gzBlockMode = g_gzMaxbits & 0x80;
    if (g_gzMaxbits & 0x60) {
        if (!g_gzQuiet)
            OldCrt_fprintf(GZ_STDERR, GZ_STR(68a168), g_gzProgname, g_gzIfname, g_gzMaxbits & 0x60);    // unknown flags
        if (g_gzExitCode == 0) g_gzExitCode = 2;
    }
    g_gzMaxbits &= 0x1f;
    long maxmaxcode = 1L << g_gzMaxbits;
    if (g_gzMaxbits > 16) {
        OldCrt_fprintf(GZ_STDERR, GZ_STR(68a12c), g_gzProgname, g_gzIfname, g_gzMaxbits, 16);
        g_gzExitCode = 1;
        return 1;
    }
    int rsize = g_gzInsize;
    int n_bits = 9;
    long maxcode = (1L << n_bits) - 1;
    unsigned bitmask = (1 << n_bits) - 1;
    long oldcode = -1;
    int finchar = 0;
    int outpos = 0;
    long posbits = g_gzInptr << 3;
    long free_ent = g_gzBlockMode ? 257 : 256;
    memset(tab_prefix, 0, 256);
    for (int c = 255; c >= 0; --c) tab_suffix[c] = (BYTE)c;
    do {
    resetbuf:
        {
            int o = posbits >> 3;
            int e = g_gzInsize - o;
            for (int i = 0; i < e; ++i) inbuf[i] = inbuf[i + o];
            g_gzInsize = e;
            posbits = 0;
        }
        if (g_gzInsize < 64) {
            if ((rsize = Gz_StreamRead((BYTE*)in, inbuf + g_gzInsize, GZ_INBUFSIZ)) == -1) Gz_ReadError();
            g_gzInsize += rsize;
            g_gzBytesIn += rsize;
        }
        long inbits = rsize != 0 ? ((long)g_gzInsize - g_gzInsize % n_bits) << 3
                                 : ((long)g_gzInsize << 3) - (n_bits - 1);
        while (inbits > posbits) {
            if (free_ent > maxcode) {
                posbits = (posbits - 1) + ((n_bits << 3) - (posbits - 1 + (n_bits << 3)) % (n_bits << 3));
                ++n_bits;
                maxcode = n_bits == g_gzMaxbits ? maxmaxcode : (1L << n_bits) - 1;
                bitmask = (1 << n_bits) - 1;
                goto resetbuf;
            }
            BYTE* p = &inbuf[posbits >> 3];
            long code = (((long)p[0] | ((long)p[1] << 8) | ((long)p[2] << 16)) >> (posbits & 7)) & bitmask;
            posbits += n_bits;
            if (oldcode == -1) {
                if (code >= 256) Gz_Error(GZ_STR(68a11c));                   // corrupt input.
                outbuf[outpos++] = (BYTE)(finchar = (int)(oldcode = code));
                continue;
            }
            if (code == 256 && g_gzBlockMode) {
                memset(tab_prefix, 0, 256);
                free_ent = 256;
                posbits = (posbits - 1) + ((n_bits << 3) - (posbits - 1 + (n_bits << 3)) % (n_bits << 3));
                n_bits = 9;
                maxcode = (1L << n_bits) - 1;
                bitmask = (1 << n_bits) - 1;
                goto resetbuf;
            }
            long incode = code;
            BYTE* stackp = de_stack;
            if (code >= free_ent) {
                if (code > free_ent) {
                    if (!g_gzTest && outpos > 0) {
                        Gz_WriteBuf(out, outbuf, outpos);
                        g_gzBytesOut += outpos;
                    }
                    Gz_Error(g_gzToStdout ? GZ_STR(68a11c) : GZ_STR(68a0ec));
                }
                *--stackp = (BYTE)finchar;
                code = oldcode;
            }
            while ((unsigned long)code >= 256) {
                *--stackp = tab_suffix[code];
                code = tab_prefix[code];
            }
            *--stackp = (BYTE)(finchar = tab_suffix[code]);
            int i;
            if (outpos + (i = (int)(de_stack - stackp)) >= GZ_OUTBUFSIZ) {
                do {
                    if (i > GZ_OUTBUFSIZ - outpos) i = GZ_OUTBUFSIZ - outpos;
                    if (i > 0) {
                        memcpy(outbuf + outpos, stackp, i);
                        outpos += i;
                    }
                    if (outpos >= GZ_OUTBUFSIZ) {
                        if (!g_gzTest) {
                            Gz_WriteBuf(out, outbuf, outpos);
                            g_gzBytesOut += outpos;
                        }
                        outpos = 0;
                    }
                    stackp += i;
                } while ((i = (int)(de_stack - stackp)) > 0);
            } else {
                memcpy(outbuf + outpos, stackp, i);
                outpos += i;
            }
            if ((code = free_ent) < maxmaxcode) {
                tab_prefix[code] = (WORD)oldcode;
                tab_suffix[code] = (BYTE)finchar;
                free_ent = code + 1;
            }
            oldcode = incode;
        }
    } while (rsize != 0);
    if (!g_gzTest && outpos > 0) {
        Gz_WriteBuf(out, outbuf, outpos);
        g_gzBytesOut += outpos;
    }
    return 0;
}

} // extern "C"

// ---------------------------------------------------------------------------
// unlzh.c (c_len = outbuf, c_table = d_buf, left = prev, right = head)

KING_VAR(int, g_lzhJ, 77b03c)
KING_VAR(unsigned, g_lzhBlocksize, 77b040)
KING_VAR(BYTE, g_lzhPtLen, 77b044)                // [19]
KING_VAR(unsigned, g_lzhI, 77b058)
KING_VAR(unsigned, g_lzhSubbitbuf, 77b05c)
KING_VAR(WORD, g_lzhBitbuf, 77b060)
KING_VAR(WORD, g_lzhPtTable, 77b064)              // [256]
KING_VAR(int, g_lzhBitcount, 77b264)
KING_VAR(int, g_lzhDone, 77b268)

#define LZH_NC 510
#define LZH_NT 19
#define LZH_NP 14

// [FUN_62bc30] fillbuf
static void Lzh_FillBuf(int n)
{
    g_lzhBitbuf <<= n;
    while (n > g_lzhBitcount) {
        g_lzhBitbuf |= g_lzhSubbitbuf << (n -= g_lzhBitcount);
        g_lzhSubbitbuf = (unsigned)Gz_TryByte();
        if ((int)g_lzhSubbitbuf == -1) g_lzhSubbitbuf = 0;
        g_lzhBitcount = 8;
    }
    g_lzhBitbuf |= g_lzhSubbitbuf >> (g_lzhBitcount -= n);
}

// [FUN_62be70] getbits
static unsigned Lzh_GetBits(int n)
{
    unsigned x = g_lzhBitbuf >> (16 - n);
    Lzh_FillBuf(n);
    return x;
}

// [FUN_62bfc0] make_table
static void Lzh_MakeTable(int nchar, BYTE* bitlen, int tablebits, WORD* table)
{
    WORD count[17], weight[17], start[18];
    unsigned i, k, len, ch, jutbits, avail, nextcode, mask;
    for (i = 1; i <= 16; i++) count[i] = 0;
    for (i = 0; i < (unsigned)nchar; i++) count[bitlen[i]]++;
    start[1] = 0;
    for (i = 1; i <= 16; i++) start[i + 1] = start[i] + (count[i] << (16 - i));
    if (start[17] != 0) Gz_Error(GZ_STR(68a0dc));                             // "Bad table\n"
    jutbits = 16 - tablebits;
    for (i = 1; i <= (unsigned)tablebits; i++) {
        start[i] >>= jutbits;
        weight[i] = (WORD)(1 << (tablebits - i));
    }
    while (i <= 16) {
        weight[i] = (WORD)(1 << (16 - i));
        i++;
    }
    i = start[tablebits + 1] >> jutbits;
    if (i != 0) {
        k = 1 << tablebits;
        while (i != k) table[i++] = 0;
    }
    avail = nchar;
    mask = 1 << (15 - tablebits);
    for (ch = 0; ch < (unsigned)nchar; ch++) {
        if ((len = bitlen[ch]) == 0) continue;
        nextcode = start[len] + weight[len];
        if (len <= (unsigned)tablebits) {
            for (i = start[len]; i < nextcode; i++) table[i] = (WORD)ch;
        } else {
            k = start[len];
            WORD* p = &table[k >> jutbits];
            i = len - tablebits;
            while (i != 0) {
                if (*p == 0) {
                    (&g_gzHead)[avail] = (&g_gzPrev)[avail] = 0;
                    *p = (WORD)avail++;
                }
                p = k & mask ? &(&g_gzHead)[*p] : &(&g_gzPrev)[*p];
                k <<= 1;
                i--;
            }
            *p = (WORD)ch;
        }
        start[len] = (WORD)nextcode;
    }
}

// [FUN_62bea0] read_pt_len
static void Lzh_ReadPtLen(int nn, int nbit, int i_special)
{
    int i, c, n = Lzh_GetBits(nbit);
    if (n == 0) {
        c = Lzh_GetBits(nbit);
        for (i = 0; i < nn; i++) (&g_lzhPtLen)[i] = 0;
        for (i = 0; i < 256; i++) (&g_lzhPtTable)[i] = (WORD)c;
        return;
    }
    i = 0;
    while (i < n) {
        c = g_lzhBitbuf >> 13;
        if (c == 7) {
            unsigned mask = 1 << 12;
            while (mask & g_lzhBitbuf) {
                mask >>= 1;
                c++;
            }
        }
        Lzh_FillBuf(c < 7 ? 3 : c - 3);
        (&g_lzhPtLen)[i++] = (BYTE)c;
        if (i == i_special) {
            c = Lzh_GetBits(2);
            while (--c >= 0) (&g_lzhPtLen)[i++] = 0;
        }
    }
    while (i < nn) (&g_lzhPtLen)[i++] = 0;
    Lzh_MakeTable(nn, &g_lzhPtLen, 8, &g_lzhPtTable);
}

// [FUN_62c1d0] read_c_len
static void Lzh_ReadCLen()
{
    int i, c, n = Lzh_GetBits(9);
    if (n == 0) {
        c = Lzh_GetBits(9);
        for (i = 0; i < LZH_NC; i++) (&g_gzOutbuf)[i] = 0;
        for (i = 0; i < 4096; i++) (&g_gzDBuf)[i] = (WORD)c;
        return;
    }
    i = 0;
    while (i < n) {
        c = (&g_lzhPtTable)[g_lzhBitbuf >> 8];
        if (c >= LZH_NT) {
            unsigned mask = 1 << 7;
            do {
                c = g_lzhBitbuf & mask ? (&g_gzHead)[c] : (&g_gzPrev)[c];
                mask >>= 1;
            } while (c >= LZH_NT);
        }
        Lzh_FillBuf((&g_lzhPtLen)[c]);
        if (c <= 2) {
            if (c == 0) c = 1;
            else if (c == 1) c = Lzh_GetBits(4) + 3;
            else c = Lzh_GetBits(9) + 20;
            while (--c >= 0) (&g_gzOutbuf)[i++] = 0;
        } else {
            (&g_gzOutbuf)[i++] = (BYTE)(c - 2);
        }
    }
    while (i < LZH_NC) (&g_gzOutbuf)[i++] = 0;
    Lzh_MakeTable(LZH_NC, &g_gzOutbuf, 12, &g_gzDBuf);
}

// [FUN_62bdc0] decode_c
static unsigned Lzh_DecodeC()
{
    if (g_lzhBlocksize == 0) {
        g_lzhBlocksize = Lzh_GetBits(16);
        if (g_lzhBlocksize == 0) return LZH_NC;
        Lzh_ReadPtLen(LZH_NT, 5, 3);
        Lzh_ReadCLen();
        Lzh_ReadPtLen(LZH_NP, 4, -1);
    }
    g_lzhBlocksize--;
    unsigned j = (&g_gzDBuf)[g_lzhBitbuf >> 4];
    if (j >= LZH_NC) {
        unsigned mask = 1 << 3;
        do {
            j = g_lzhBitbuf & mask ? (&g_gzHead)[j] : (&g_gzPrev)[j];
            mask >>= 1;
        } while (j >= LZH_NC);
    }
    Lzh_FillBuf((&g_gzOutbuf)[j]);
    return j;
}

// [FUN_62c330] decode_p
static unsigned Lzh_DecodeP()
{
    unsigned j = (&g_lzhPtTable)[g_lzhBitbuf >> 8];
    if (j >= LZH_NP) {
        unsigned mask = 1 << 7;
        do {
            j = g_lzhBitbuf & mask ? (&g_gzHead)[j] : (&g_gzPrev)[j];
            mask >>= 1;
        } while (j >= LZH_NP);
    }
    Lzh_FillBuf((&g_lzhPtLen)[j]);
    if (j != 0) j = (1 << (j - 1)) + Lzh_GetBits(j - 1);
    return j;
}

// [FUN_62bcd0] decode: up to count bytes into buffer
static unsigned Lzh_Decode(unsigned count, BYTE* buffer)
{
    unsigned r = 0;
    while (--g_lzhJ >= 0) {
        buffer[r] = buffer[g_lzhI];
        g_lzhI = (g_lzhI + 1) & 8191;
        if (++r == count) return r;
    }
    for (;;) {
        unsigned c = Lzh_DecodeC();
        if (c == LZH_NC) {
            g_lzhDone = 1;
            return r;
        }
        if (c <= 255) {
            buffer[r] = (BYTE)c;
            if (++r == count) return r;
        } else {
            g_lzhJ = c - 253;
            g_lzhI = (r - Lzh_DecodeP() - 1) & 8191;
            while (--g_lzhJ >= 0) {
                buffer[r] = buffer[g_lzhI];
                g_lzhI = (g_lzhI + 1) & 8191;
                if (++r == count) return r;
            }
        }
    }
}

extern "C" {

// [$L_62bb80] unlzh: LZH data ([FUN_62bbe0] decode_start, [FUN_62bc00]
// huf_decode_start, [FUN_62bc10] init_getbits)
int __cdecl Gz_Unlzh(void* in, void* out)
{
    g_gzIfd = in;
    g_gzOfd = out;
    g_lzhBitbuf = 0;
    g_lzhSubbitbuf = 0;
    g_lzhBitcount = 0;
    Lzh_FillBuf(16);
    g_lzhBlocksize = 0;
    g_lzhJ = 0;
    g_lzhDone = 0;
    while (!g_lzhDone) {
        unsigned n = Lzh_Decode(8192, &g_gzWindow);
        if (!g_gzTest && n > 0) Gz_WriteBuf(out, &g_gzWindow, n);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// gzip.c

// [FUN_62ac50] get_method: the header read, work set; the method, -1 or -2
int __cdecl Gz_GetMethod(void* in)
{
    BYTE magic[2];
    if (g_gzForce && g_gzToStdout) {
        magic[0] = (BYTE)Gz_TryByte();
        magic[1] = (BYTE)Gz_TryByte();
    } else {
        magic[0] = (BYTE)Gz_GetByte();
        magic[1] = (BYTE)Gz_GetByte();
    }
    g_gzMethod = -1;
    g_gzPartNb++;
    g_gzHeaderBytes = 0;
    g_gzLastMember = 0;
    if (memcmp(magic, &g_gzMagicGzip, 2) == 0 || memcmp(magic, &g_gzMagicOldGzip, 2) == 0) {
        g_gzMethod = Gz_GetByte();
        if (g_gzMethod != 8) {
            OldCrt_fprintf(GZ_STDERR, GZ_STR(689c3c), g_gzProgname, g_gzIfname, g_gzMethod);    // unknown method
            g_gzExitCode = 1;
            return -1;
        }
        g_gzWork = Gz_Unzip;
        BYTE flags = (BYTE)Gz_GetByte();
        if (flags & 0x20) {
            OldCrt_fprintf(GZ_STDERR, GZ_STR(689c08), g_gzProgname, g_gzIfname);               // encrypted
            g_gzExitCode = 1;
            return -1;
        }
        if (flags & 2) {
            OldCrt_fprintf(GZ_STDERR, GZ_STR(689bc4), g_gzProgname, g_gzIfname);               // multi-part
            g_gzExitCode = 1;
            if (g_gzForce <= 1) return -1;
        }
        if (flags & 0xc0) {
            OldCrt_fprintf(GZ_STDERR, GZ_STR(689b90), g_gzProgname, g_gzIfname, flags);        // flags 0x%x
            g_gzExitCode = 1;
            if (g_gzForce <= 1) return -1;
        }
        DWORD stamp = (DWORD)Gz_GetByte();
        stamp |= (DWORD)Gz_GetByte() << 8;
        stamp |= (DWORD)Gz_GetByte() << 16;
        stamp |= (DWORD)Gz_GetByte() << 24;
        if (stamp != 0 && !g_gzNoTime) g_gzTimeStamp = stamp;
        Gz_GetByte();                                // extra flags
        Gz_GetByte();                                // OS type
        if (flags & 2) {
            unsigned part = (unsigned)Gz_GetByte();
            part |= ((unsigned)Gz_GetByte()) << 8;
            if (g_gzVerbose) OldCrt_fprintf(GZ_STDERR, GZ_STR(689b78), g_gzProgname, g_gzIfname, part);
        }
        if (flags & 4) {
            unsigned len = (unsigned)Gz_GetByte();
            len |= ((unsigned)Gz_GetByte()) << 8;
            if (g_gzVerbose) OldCrt_fprintf(GZ_STDERR, GZ_STR(689b4c), g_gzProgname, g_gzIfname, len);
            while (len--) Gz_GetByte();
        }
        if (flags & 8) {
            if (g_gzNoName || (g_gzToStdout && !g_gzList) || g_gzPartNb > 1) {
                while (Gz_GetByte() != 0) {}
            } else {
                char* p = Gz_Basename(g_gzOfname);
                char* base = p;
                for (;;) {
                    *p = (char)Gz_GetByte();
                    if (*p++ == '\0') break;
                    if (p >= g_gzOfnameEnd) Gz_Error(GZ_STR(689b24));    // file name too large
                }
                if (!g_gzList && base) g_gzList = 0;
            }
        }
        if (flags & 0x10) {
            while (Gz_GetByte() != 0) {}
        }
        if (g_gzPartNb == 1) g_gzHeaderBytes = g_gzInptr + 8;
    } else if (memcmp(magic, &g_gzMagicPkzip, 2) == 0 && g_gzInptr == 2
               && memcmp(&g_gzInbuf, &g_gzMagicPkzip, 4) == 0) {
        g_gzInptr = 0;
        g_gzWork = Gz_Unzip;
        if (Gz_CheckZipfile(in) != 0) return -1;
        g_gzLastMember = 1;
    } else if (memcmp(magic, &g_gzMagicPack, 2) == 0) {
        g_gzWork = Gz_Unpack;
        g_gzMethod = 2;
    } else if (memcmp(magic, &g_gzMagicLzw, 2) == 0) {
        g_gzWork = Gz_Unlzw;
        g_gzMethod = 1;
        g_gzLastMember = 1;
    } else if (memcmp(magic, &g_gzMagicLzh, 2) == 0) {
        g_gzWork = Gz_Unlzh;
        g_gzMethod = 3;
        g_gzLastMember = 1;
    } else if (g_gzForce && g_gzToStdout && !g_gzList) {
        g_gzMethod = 0;
        g_gzWork = Gz_Copy;
        g_gzInptr = 0;
        g_gzLastMember = 1;
    }
    if (g_gzMethod >= 0) return g_gzMethod;
    if (g_gzPartNb == 1) {
        OldCrt_fprintf(GZ_STDERR, GZ_STR(689b04), g_gzProgname, g_gzIfname);                  // not in gzip format
        g_gzExitCode = 1;
        return -1;
    }
    if (!g_gzQuiet) OldCrt_fprintf(GZ_STDERR, GZ_STR(689acc), g_gzProgname, g_gzIfname);       // trailing garbage
    if (g_gzExitCode == 0) g_gzExitCode = 2;
    return -2;
}

// [FUN_62aad0] gzip's option defaults
static void Gz_Defaults()
{
    g_gzAscii = 0;
    g_gzToStdout = 0;
    g_gzDecompress = 0;
    g_gzForce = 0;
    g_gzNoName = -1;
    g_gzNoTime = -1;
    g_gzRecursive = 0;
    g_gzList = 0;
    g_gzVerbose = 0;
    g_gzQuiet = 0;
    g_gzDoLzw = 0;
    g_gzTest = 0;
    g_gzMaxbits = 16;
    g_gzMethod = 8;
    g_gzLevel = 6;
    g_gzExitCode = 0;
    g_gzRemoveOfname = 0;
}

// [FUN_62ab40]
static void Gz_InitOnce()
{
    if (!g_gzInitDone) g_gzInitDone = 1;
}

// [FUN_62ab60] compresses stream in into stream out
int __cdecl Gz_Compress(void* in, void* out)
{
    Gz_InitOnce();
    Gz_Defaults();
    g_gzDecompress = 0;
    g_gzIfd = in;
    g_gzOfd = out;
    Gz_ClearBufs();
    g_gzPartNb = 0;
    if (Gz_Zip(in, out) != 0) g_gzMethod = -1;
    return 0;
}

// [FUN_62abc0] decompresses stream in into stream out; -1 (in deleted)
// when in has no known header
int __cdecl Gz_Decompress(void* in, void* out)
{
    Gz_InitOnce();
    Gz_Defaults();
    g_gzDecompress = 1;
    g_gzIfd = in;
    g_gzOfd = out;
    Gz_ClearBufs();
    g_gzPartNb = 0;
    g_gzMethod = Gz_GetMethod(in);
    if (g_gzMethod < 0) {
        Gz_StreamClose((BYTE**)&in);
        return -1;
    }
    if (g_gzWork(g_gzIfd, g_gzOfd) != 0) g_gzMethod = -1;
    return 0;
}

} // extern "C"
