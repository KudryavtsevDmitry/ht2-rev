#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" {
BYTE* g_marineUnusedVertices=0;
DWORD g_marineUnusedVertexCount=0;
DWORD __fastcall MarineMesh_PruneVertices(BYTE*,void*);
DWORD __fastcall Reference_PruneVertices(BYTE*,void*);
DWORD __fastcall MarineMesh_DrainEdges(BYTE*,void*);
DWORD __fastcall Reference_DrainEdges(BYTE*,void*);
}
static BYTE self[64], list[12], vertices[8][12], triangles[8][32], pending[8][12];
static BYTE* table[8];
static DWORD scratch[8], trace[128];
static volatile BYTE expectedSelf[64], expectedVertices[8][12], expectedPending[8][12];
static DWORD expectedTrace[128], expectedScratch[8];
static BYTE* expectedTable[8];
static unsigned count, allocations, failures, edgeCount;
static void Record(DWORD value) { trace[count++]=value; }
extern "C" void* TestMalloc(unsigned bytes) { Record(1); Record(bytes); return scratch; }
void* operator new(size_t size)
{
    Record(2); Record(size);
    unsigned call=allocations++;
    if(failures==2 || (failures==1 && call%2==0)) return 0;
    return pending[call];
}
void operator delete(void* pointer)
{
    Record(3); Record((DWORD)pointer);
    for(unsigned i=0;i<12;i++) ((BYTE*)pointer)[i]=0xdd;
}
extern "C" BYTE* __fastcall TestTopEdge(BYTE* queue,void*) { Record(4); Record((DWORD)queue); return edgeCount?vertices[edgeCount-1]:0; }
extern "C" void TestSplitEdge(BYTE* edge,BYTE* mesh) { Record(5); Record((DWORD)edge); Record((DWORD)mesh); --edgeCount; }
KING_ALIAS("@MarineQueue_TopEdge@8","@TestTopEdge@8")
KING_ALIAS("_FUN_606760","_TestSplitEdge")
static void Setup(unsigned length,unsigned mask,unsigned failure)
{
    count=allocations=0; failures=failure;
    g_marineUnusedVertices=0; g_marineUnusedVertexCount=0;
    for(unsigned i=0;i<128;i++) trace[i]=0;
    for(unsigned i=0;i<64;i++) self[i]=(BYTE)i;
    for(unsigned i=0;i<12;i++) list[i]=0;
    unsigned used=0;
    for(unsigned i=0;i<8;i++) {
        for(unsigned j=0;j<12;j++) vertices[i][j]=pending[i][j]=(BYTE)(i*12+j);
        for(unsigned j=0;j<32;j++) triangles[i][j]=0;
        table[i]=i<length?vertices[i]:0; F(WORD,vertices[i],8)=(WORD)i; scratch[i]=0xdeadbeef;
        if(i<length && (mask&(1u<<i))) {
            BYTE* node=triangles[used++];
            F(void*,node,16)=F(void*,node,20)=F(void*,node,24)=vertices[i];
        }
    }
    for(unsigned i=0;i<used;i++) {
        F(void*,triangles[i],4)=triangles[(i+1)%used]+4;
        F(void*,triangles[i],8)=triangles[(i+used-1)%used]+4;
    }
    F(void*,list,0)=used?triangles[0]:0;
    F(void*,self,12)=table; F(void*,self,32)=list; F(void*,self,28)=list;
    F(WORD,self,36)=(WORD)length;
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    for(unsigned length=0;length<9;length++) for(unsigned mask=0;mask<(1u<<length);mask++) for(unsigned failure=0;failure<3;failure++) {
        Setup(length,mask,failure); Reference_PruneVertices(self,0); unsigned expectedCount=count;
        for(unsigned i=0;i<64;i++) expectedSelf[i]=self[i];
        for(unsigned i=0;i<8;i++) {
            expectedTable[i]=table[i]; expectedScratch[i]=scratch[i];
            for(unsigned j=0;j<12;j++) { expectedVertices[i][j]=vertices[i][j]; expectedPending[i][j]=pending[i][j]; }
        }
        for(unsigned i=0;i<128;i++) expectedTrace[i]=trace[i];
        Setup(length,mask,failure); if(MarineMesh_PruneVertices(self,0)!=0 || count!=expectedCount) ExitProcess(1);
        if(g_marineUnusedVertices || g_marineUnusedVertexCount) ExitProcess(2);
        for(unsigned i=0;i<64;i++) if(expectedSelf[i]!=self[i]) ExitProcess(3);
        for(unsigned i=0;i<8;i++) {
            if(expectedTable[i]!=table[i] || expectedScratch[i]!=scratch[i]) ExitProcess(4);
            for(unsigned j=0;j<12;j++) if(expectedVertices[i][j]!=vertices[i][j] || expectedPending[i][j]!=pending[i][j]) ExitProcess(5);
        }
        for(unsigned i=0;i<128;i++) if(expectedTrace[i]!=trace[i]) ExitProcess(6);
    }
    for(unsigned edges=0;edges<9;edges++) {
        Setup(8,255,0); edgeCount=edges; Reference_DrainEdges(self,0); unsigned expectedCount=count;
        for(unsigned i=0;i<128;i++) expectedTrace[i]=trace[i];
        Setup(8,255,0); edgeCount=edges;
        if(MarineMesh_DrainEdges(self,0)!=0 || count!=expectedCount || edgeCount) ExitProcess(7);
        for(unsigned i=0;i<128;i++) if(expectedTrace[i]!=trace[i]) ExitProcess(8);
    }
    ExitProcess(0);
}
