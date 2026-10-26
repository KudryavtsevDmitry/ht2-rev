// mfc.cpp - the parts of MFC 4.2 (statically linked into king.exe) that the
// game runs, rewritten so that king.exe stops depending on the MFC library.
// The object layouts and vtable slots stay those of MFC 4.2, because the
// game's own classes derive from CWinApp and CFrameWnd.
//
// First part: MFC's thread local and process local storage
// (CThreadSlotData, CThreadLocalObject, CProcessLocalObject) and the global
// locks (AfxLockGlobals).
//
// Building with  nmake ASM_MFC=1  makes king.masm assemble the original
// procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" void* __cdecl memset(void*, int, size_t);

// AfxThrowMemoryException; never returns
ASM_PROC(FUN_6321e9)
static void ThrowMemory() { ((void (__cdecl*)())FUN_6321e9)(); }

// ---------------------------------------------------------------------------
// CThreadSlotData (52 bytes): one TLS index; each thread's CThreadData holds
// an array of per-slot object pointers.

struct SlotData { DWORD flags; HINSTANCE hInst; };   // flags bit 0: slot in use

struct ThreadData {           // CThreadData (CNoTrackObject)
    void* vtbl;
    ThreadData* next;
    int count;
    void** data;
};

struct SlotList {             // CSimpleList
    void* head;
    int nextOffset;
};

struct ThreadSlotData {
    DWORD tlsIndex;
    int alloc;                // entries in slotData
    int rover;                // next slot to try
    int max;                  // highest slot used + 1
    SlotData* slotData;       // GlobalAlloc'd
    SlotList list;            // all threads' ThreadData
    CRITICAL_SECTION sect;
};
CHECK_OFFSET(ThreadSlotData, list, 20);
CHECK_OFFSET(ThreadSlotData, sect, 28);

ASM_VAR(ThreadSlotData*, g_afxThreadData, $L_72de44)
ASM_VAR(ThreadSlotData, g_afxThreadDataStore, $L_72de48)
ASM_VAR(int, g_afxTlsRef, $L_72de40)
ASM_VAR(AsmLabel, g_threadDataVtbl, $L_6545a8)

// virtual destructor (slot 0) with "delete this"
static void DeleteNoTrack(void* p) {
    ((void (__fastcall*)(void*, void*, int))(*(void***)p)[0])(p, 0, 1);
}

// [FUN_63a8fb] CNoTrackObject::operator new: zeroed LocalAlloc
extern "C" void* __stdcall NoTrack_New(size_t size) {
    void* p = LocalAlloc(LPTR, size);
    if (!p) ThrowMemory();
    return p;
}

// [FUN_63a919] CNoTrackObject::operator delete
extern "C" void __stdcall NoTrack_Delete(void* p) {
    if (p) LocalFree(p);
}

// [FUN_63a8a2] CSimpleList::AddHead
extern "C" void __fastcall SimpleList_AddHead(SlotList* self, void*, void* p) {
    F(void*, p, self->nextOffset) = self->head;
    self->head = p;
}

// [FUN_63a8b5] CSimpleList::Remove
extern "C" BOOL __fastcall SimpleList_Remove(SlotList* self, void*, void* p) {
    void* cur = self->head;
    if (!cur) return FALSE;
    if (cur == p) {
        self->head = F(void*, p, self->nextOffset);
        return TRUE;
    }
    while (cur) {
        void* next = F(void*, cur, self->nextOffset);
        if (next == p) {
            F(void*, cur, self->nextOffset) = F(void*, p, self->nextOffset);
            return TRUE;
        }
        cur = next;
    }
    return FALSE;
}

// [FUN_63a92d] CThreadSlotData::CThreadSlotData
extern "C" ThreadSlotData* __fastcall SlotData_Construct(ThreadSlotData* self) {
    self->list.head = 0;
    self->list.nextOffset = 4;
    self->alloc = 0;
    self->rover = 1;
    self->max = 0;
    self->slotData = 0;
    self->tlsIndex = TlsAlloc();
    if (self->tlsIndex == (DWORD)-1) ThrowMemory();
    InitializeCriticalSection(&self->sect);
    return self;
}

// [FUN_63ac3b] CThreadSlotData::DeleteValues(pData, hInst): deletes one
// thread's values of the module hInst (all if 0); frees the thread's data
// when no value is left
extern "C" void __fastcall SlotData_DeleteThreadValues(ThreadSlotData* self, void*, ThreadData* data, HINSTANCE hInst) {
    BOOL empty = TRUE;
    for (int i = 1; i < data->count; i++) {
        if (!hInst || self->slotData[i].hInst == hInst) {
            if (data->data[i]) DeleteNoTrack(data->data[i]);
            data->data[i] = 0;
        } else if (data->data[i]) {
            empty = FALSE;
        }
    }
    if (!empty) return;
    EnterCriticalSection(&self->sect);
    SimpleList_Remove(&self->list, 0, data);
    LeaveCriticalSection(&self->sect);
    LocalFree(data->data);
    DeleteNoTrack(data);
    TlsSetValue(self->tlsIndex, 0);
}

// [FUN_63a96f] CThreadSlotData::~CThreadSlotData
extern "C" void __fastcall SlotData_Destruct(ThreadSlotData* self) {
    if (self->tlsIndex != (DWORD)-1) TlsFree(self->tlsIndex);
    for (ThreadData* p = (ThreadData*)self->list.head; p; ) {
        ThreadData* next = p->next;
        SlotData_DeleteThreadValues(self, 0, p, 0);
        p = next;
    }
    if (self->slotData) {
        HGLOBAL h = GlobalHandle(self->slotData);
        GlobalUnlock(h);
        GlobalFree(h);
    }
    DeleteCriticalSection(&self->sect);
}

// [FUN_63a9c6] CThreadSlotData::AllocSlot
extern "C" int __fastcall SlotData_AllocSlot(ThreadSlotData* self) {
    EnterCriticalSection(&self->sect);
    int alloc = self->alloc;
    int slot = self->rover;
    if (slot >= alloc || (self->slotData[slot].flags & 1)) {
        for (slot = 1; slot < alloc && (self->slotData[slot].flags & 1); slot++) {}
        if (slot >= alloc) {
            int newAlloc = alloc + 32;
            HGLOBAL h;
            if (!self->slotData) {
                h = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, newAlloc * sizeof(SlotData));
            } else {
                HGLOBAL old = GlobalHandle(self->slotData);
                GlobalUnlock(old);
                h = GlobalReAlloc(old, newAlloc * sizeof(SlotData), GMEM_MOVEABLE | GMEM_SHARE);
            }
            if (!h) {
                GlobalLock(GlobalHandle(self->slotData));
                LeaveCriticalSection(&self->sect);
                ThrowMemory();
            }
            SlotData* p = (SlotData*)GlobalLock(h);
            memset(p + self->alloc, 0, (newAlloc - self->alloc) * sizeof(SlotData));
            self->slotData = p;
            self->alloc = newAlloc;
        }
    }
    if (slot >= self->max) self->max = slot + 1;
    self->slotData[slot].flags |= 1;
    self->rover = slot + 1;
    LeaveCriticalSection(&self->sect);
    return slot;
}

// [FUN_63aad8] CThreadSlotData::FreeSlot
extern "C" void __fastcall SlotData_FreeSlot(ThreadSlotData* self, void*, int slot) {
    EnterCriticalSection(&self->sect);
    for (ThreadData* p = (ThreadData*)self->list.head; p; p = p->next) {
        if (slot < p->count) {
            if (p->data[slot]) DeleteNoTrack(p->data[slot]);
            p->data[slot] = 0;
        }
    }
    self->slotData[slot].flags &= ~1;
    LeaveCriticalSection(&self->sect);
}

// [FUN_63ab35] CThreadSlotData::SetValue
extern "C" void __fastcall SlotData_SetValue(ThreadSlotData* self, void*, int slot, void* value) {
    ThreadData* data = (ThreadData*)TlsGetValue(self->tlsIndex);
    if (!data || (slot >= data->count && value)) {
        if (!data) {
            data = (ThreadData*)NoTrack_New(sizeof(ThreadData));
            data->vtbl = g_threadDataVtbl;
            data->count = 0;
            data->data = 0;
            EnterCriticalSection(&self->sect);
            SimpleList_AddHead(&self->list, 0, data);
            LeaveCriticalSection(&self->sect);
        }
        void** p = data->data
            ? (void**)LocalReAlloc(data->data, self->max * sizeof(void*), LMEM_MOVEABLE)
            : (void**)LocalAlloc(LMEM_FIXED, self->max * sizeof(void*));
        data->data = p;
        if (!p) ThrowMemory();
        memset(p + data->count, 0, (self->max - data->count) * sizeof(void*));
        data->count = self->max;
        TlsSetValue(self->tlsIndex, data);
    }
    data->data[slot] = value;
}

// [FUN_63acd4] CThreadSlotData::DeleteValues(hInst, bAll): the values of the
// current thread, or of all threads
extern "C" void __fastcall SlotData_DeleteValues(ThreadSlotData* self, void*, HINSTANCE hInst, BOOL all) {
    EnterCriticalSection(&self->sect);
    if (!all) {
        ThreadData* data = (ThreadData*)TlsGetValue(self->tlsIndex);
        if (data) SlotData_DeleteThreadValues(self, 0, data, hInst);
    } else {
        for (ThreadData* p = (ThreadData*)self->list.head; p; ) {
            ThreadData* next = p->next;
            SlotData_DeleteThreadValues(self, 0, p, hInst);
            p = next;
        }
    }
    LeaveCriticalSection(&self->sect);
}

// ---------------------------------------------------------------------------
// CThreadLocalObject { int slot; } and CProcessLocalObject { void* object; }

// [FUN_63ad2d] CThreadLocalObject::GetData: this thread's object, made by
// create() the first time
extern "C" void* __fastcall ThreadLocal_GetData(int* self, void*, void* (__stdcall* create)()) {
    if (*self == 0) {
        if (!g_afxThreadData) g_afxThreadData = SlotData_Construct(&g_afxThreadDataStore);
        *self = SlotData_AllocSlot(g_afxThreadData);
    }
    ThreadSlotData* sd = g_afxThreadData;
    int slot = *self;
    ThreadData* data = (ThreadData*)TlsGetValue(sd->tlsIndex);
    void* value = data && slot < data->count ? data->data[slot] : 0;
    if (!value) {
        value = create();
        SlotData_SetValue(g_afxThreadData, 0, slot, value);
    }
    return value;
}

// [FUN_63ada4] CThreadLocalObject::GetDataNA: 0 if not made yet
extern "C" void* __fastcall ThreadLocal_GetDataNA(int* self) {
    int slot = *self;
    if (!slot || !g_afxThreadData) return 0;
    ThreadData* data = (ThreadData*)TlsGetValue(g_afxThreadData->tlsIndex);
    if (!data || slot >= data->count) return 0;
    return data->data[slot];
}

// [FUN_63add1] CThreadLocalObject::~CThreadLocalObject
extern "C" void __fastcall ThreadLocal_Destruct(int* self) {
    if (*self && g_afxThreadData) SlotData_FreeSlot(g_afxThreadData, 0, *self);
    *self = 0;
}

extern "C" void __stdcall Afx_LockGlobals(int lock);
extern "C" void __stdcall Afx_UnlockGlobals(int lock);

// [FUN_63adef] CProcessLocalObject::GetData
extern "C" void* __fastcall ProcessLocal_GetData(void** self, void*, void* (__stdcall* create)()) {
    if (!*self) {
        Afx_LockGlobals(16);
        if (!*self) *self = create();
        Afx_UnlockGlobals(16);
    }
    return *self;
}

// [FUN_63ae6f] AfxTermLocalData
extern "C" void __stdcall Afx_TermLocalData(HINSTANCE hInst, BOOL all) {
    if (g_afxThreadData) SlotData_DeleteValues(g_afxThreadData, 0, hInst, all);
}

// [$L_63ae89] AfxTlsRelease
extern "C" void __cdecl Afx_TlsRelease() {
    if (g_afxTlsRef != 0 && --g_afxTlsRef != 0) return;
    if (g_afxThreadData) {
        SlotData_Destruct(g_afxThreadData);
        g_afxThreadData = 0;
    }
}

// ---------------------------------------------------------------------------
// AfxLockGlobals: 17 lazily made critical sections

ASM_VAR(int, g_afxCriticalInit, $L_72e074)
ASM_VAR(int, g_afxSingleThreaded, $L_72e070)
ASM_VAR(int, g_afxLockInit, $L_72de7c)                  // int[17]
ASM_VAR(CRITICAL_SECTION, g_afxLockInitLock, $L_72dec0)
ASM_VAR(CRITICAL_SECTION, g_afxResourceLock, $L_72ded8) // CRITICAL_SECTION[17]

// [FUN_63af8b] AfxCriticalInit
extern "C" BOOL __cdecl Afx_CriticalInit() {
    if (!g_afxCriticalInit) {
        g_afxCriticalInit = 1;
        DWORD v = GetVersion();
        if ((BYTE)v < 4 && (v & 0x80000000)) {   // Win32s
            g_afxSingleThreaded = 1;
        } else {
            g_afxSingleThreaded = 0;
            InitializeCriticalSection(&g_afxLockInitLock);
        }
    }
    return g_afxCriticalInit;
}

// [FUN_63afd0] AfxCriticalTerm
extern "C" void __cdecl Afx_CriticalTerm() {
    if (!g_afxCriticalInit) return;
    --g_afxCriticalInit;
    if (g_afxSingleThreaded) return;
    DeleteCriticalSection(&g_afxLockInitLock);
    for (int i = 0; i < 17; i++) {
        if ((&g_afxLockInit)[i]) {
            DeleteCriticalSection(&g_afxResourceLock + i);
            --(&g_afxLockInit)[i];
        }
    }
}

// [FUN_63b01e] AfxLockGlobals
extern "C" void __stdcall Afx_LockGlobals(int lock) {
    if (!g_afxCriticalInit) Afx_CriticalInit();
    if (g_afxSingleThreaded) return;
    if (!(&g_afxLockInit)[lock]) {
        EnterCriticalSection(&g_afxLockInitLock);
        if (!(&g_afxLockInit)[lock]) {
            InitializeCriticalSection(&g_afxResourceLock + lock);
            ++(&g_afxLockInit)[lock];
        }
        LeaveCriticalSection(&g_afxLockInitLock);
    }
    EnterCriticalSection(&g_afxResourceLock + lock);
}

// [FUN_63b08e] AfxUnlockGlobals
extern "C" void __stdcall Afx_UnlockGlobals(int lock) {
    if (!g_afxSingleThreaded) LeaveCriticalSection(&g_afxResourceLock + lock);
}

// ---------------------------------------------------------------------------
// The thread and module states

ASM_VAR(int, g_afxThreadState, $L_72db7c)     // CThreadLocal<_AFX_THREAD_STATE>
ASM_PROC($L_639dac)                           // new _AFX_THREAD_STATE
ASM_PROC($L_639ca6)                           // new AFX_MODULE_THREAD_STATE
extern "C" BYTE* __stdcall AfxGetModuleState_();
KING_ALIAS("_AfxGetModuleState_@0", "?AfxGetModuleState@@YGPAVAFX_MODULE_STATE@@XZ")

// [FUN_63a115] AfxGetThreadState
extern "C" BYTE* __cdecl Afx_GetThreadState() {
    return (BYTE*)ThreadLocal_GetData(&g_afxThreadState, 0, (void* (__stdcall*)())$L_639dac);
}

// [FUN_63a368] AfxGetModuleThreadState
extern "C" BYTE* __cdecl Afx_GetModuleThreadState() {
    return (BYTE*)ThreadLocal_GetData(&F(int, AfxGetModuleState_(), 4208), 0, (void* (__stdcall*)())$L_639ca6);
}
