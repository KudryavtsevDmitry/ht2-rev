// mfc.cpp - the parts of MFC 4.2 (statically linked into king.exe) that the
// game runs, rewritten so that king.exe stops depending on the MFC library.
// The object layouts and vtable slots stay those of MFC 4.2, because the
// game's own classes derive from CWinApp and CFrameWnd.
//
// Contents: MFC's thread local and process local storage
// (CThreadSlotData, CThreadLocalObject, CProcessLocalObject) and the global
// locks (AfxLockGlobals).

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" void* __cdecl memset(void*, int, size_t);
extern "C" void* __cdecl malloc(size_t);
extern "C" void __cdecl free(void*);
extern "C" void* __cdecl memcpy(void*, const void*, size_t);

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

KING_VAR(ThreadSlotData*, g_afxThreadData, 72de44)
KING_VAR(ThreadSlotData, g_afxThreadDataStore, 72de48)
KING_VAR(int, g_afxTlsRef, 72de40)
KING_VAR(AsmLabel, g_threadDataVtbl, 6545a8)

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

KING_VAR(int, g_afxCriticalInit, 72e074)
KING_VAR(int, g_afxSingleThreaded, 72e070)
KING_VAR(int, g_afxLockInit, 72de7c)                  // int[17]
KING_VAR(CRITICAL_SECTION, g_afxLockInitLock, 72dec0)
KING_VAR(CRITICAL_SECTION, g_afxResourceLock, 72ded8) // CRITICAL_SECTION[17]

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
// Declarations of the collections and handle maps further down

struct Plex { Plex* next; };
struct Assoc { Assoc* next; void* key; void* value; };
struct PtrMap {               // CMapPtrToPtr
    void* vtbl;
    Assoc** table;
    UINT tableSize;
    int count;
    Assoc* freeList;
    Plex* blocks;
    int blockSize;
};
CHECK_OFFSET(PtrMap, blockSize, 24);

extern "C" void* __fastcall PtrMap_GetValueAt(PtrMap* self, void*, void* key);
extern "C" void** __fastcall PtrMap_At(PtrMap* self, void*, void* key);
extern "C" BOOL __fastcall PtrMap_RemoveKey(PtrMap* self, void*, void* key);
extern "C" void* __stdcall Afx_SetNewHandler(void* handler);
extern "C" BYTE* __fastcall HandleMap_Construct(BYTE* self, void*, BYTE* cls, int offset, int handles);
extern "C" BYTE* __fastcall HandleMap_FromHandle(BYTE* self, void*, HANDLE h);
extern "C" void __fastcall HandleMap_DeleteTemp(BYTE* self);
extern "C" BYTE* __stdcall Dc_FromHandle(HDC h);
extern "C" BYTE* __fastcall Cdc_Construct(BYTE* self);
extern "C" void __fastcall Cdc_Destruct(BYTE* self);
extern "C" int __stdcall Afx_LoadString(UINT id, char* buf, UINT max);
extern "C" BOOL __fastcall Wnd_IsWindowEnabled(BYTE* self);
extern "C" void __stdcall Afx_HandleActivate(BYTE* wnd, WPARAM state, BYTE* other);
extern "C" HDC __fastcall Cdc_Detach(BYTE* self);
extern "C" BYTE* __fastcall Wnd_GetParentFrame(BYTE* self);

// ---------------------------------------------------------------------------
// The thread and module states
//
// _AFX_THREAD_STATE (280 bytes, per thread): +4 m_pModuleState,
//   +12 m_pSafetyPoolBuffer, +44 m_hHookOldCbtFilter, +48 m_hHookOldMsgFilter,
//   +204 m_pWndPark.
// AFX_MODULE_STATE (4212 bytes, the base module's): +4 m_pCurrentWinApp,
//   +8 m_hCurrentInstanceHandle, +12 m_hCurrentResourceHandle,
//   +16 m_lpszCurrentAppName, +20 m_bDLL, +48 m_bUserCtrl,
//   +52 m_szUnregisterList (registered window classes, '\n' separated),
//   +4208 CThreadLocal<AFX_MODULE_THREAD_STATE>.
// AFX_MODULE_THREAD_STATE (60 bytes): +4 m_pCurrentWinThread, +8 m_frameList,
//   +16 m_nTempMapLock, +20..+36 the handle maps.

KING_VAR(int, g_afxThreadState, 72db7c)       // CThreadLocal<_AFX_THREAD_STATE>
KING_VAR(void*, g_afxBaseModuleState, 72db78) // CProcessLocal<_AFX_BASE_MODULE_STATE>
KING_VAR(AsmLabel, g_threadStateVtbl, 654284)
KING_VAR(AsmLabel, g_moduleThreadStateVtbl, 654294)
KING_VAR(AsmLabel, g_moduleStateVtbl, 65428c)
KING_VAR(AsmLabel, g_baseModuleStateVtbl, 65429c)
extern "C" void FUN_632629(); KING_ALIAS("_FUN_632629", "_Afx_ThrowMemoryException")                            // AFX_MODULE_THREAD_STATE's handler

// [FUN_63a06f] _AFX_THREAD_STATE::_AFX_THREAD_STATE
extern "C" BYTE* __fastcall ThreadState_Construct(BYTE* self) {
    F(int, self, 212) = -1;
    F(int, self, 260) = -1;
    F(void*, self, 0) = g_threadStateVtbl;
    return self;
}

// [$L_639dac] creates a thread's _AFX_THREAD_STATE
extern "C" BYTE* __cdecl ThreadState_New() {
    BYTE* p = (BYTE*)NoTrack_New(280);
    return p ? ThreadState_Construct(p) : 0;
}

// [FUN_63a239] AFX_MODULE_THREAD_STATE::AFX_MODULE_THREAD_STATE
extern "C" BYTE* __fastcall ModuleThreadState_Construct(BYTE* self) {
    F(void*, self, 8) = 0;           // m_frameList: CFrameWnd::m_pNextFrameWnd at +84
    F(int, self, 12) = 84;
    F(void*, self, 0) = g_moduleThreadStateVtbl;
    F(void*, self, 40) = (void*)FUN_632629;
    return self;
}

// [$L_639ca6] creates a thread's AFX_MODULE_THREAD_STATE
extern "C" BYTE* __cdecl ModuleThreadState_New() {
    BYTE* p = (BYTE*)NoTrack_New(60);
    return p ? ModuleThreadState_Construct(p) : 0;
}

// [FUN_63a146] AFX_MODULE_STATE::AFX_MODULE_STATE(bDLL)
extern "C" BYTE* __fastcall ModuleState_Construct(BYTE* self, void*, BOOL dll) {
    F(int, self, 28) = 0;
    F(int, self, 32) = 0;
    F(int, self, 36) = 0;
    F(int, self, 40) = 0;
    F(int, self, 4156) = 0;
    F(int, self, 4160) = 0;
    F(int, self, 4172) = -1;
    F(int, self, 4176) = 0;
    F(int, self, 4196) = 0;
    F(int, self, 4200) = 0;
    F(void*, self, 0) = g_moduleStateVtbl;
    F(int, self, 40) = 28;
    F(int, self, 32) = 20;
    F(int, self, 24) = 0;
    F(BYTE, self, 20) = (BYTE)dll;
    F(int, self, 48) = 1;
    F(int, self, 4160) = 24;
    return self;
}

// [$L_63a3b4] creates the _AFX_BASE_MODULE_STATE
extern "C" BYTE* __cdecl BaseModuleState_New() {
    BYTE* p = (BYTE*)NoTrack_New(4212);
    if (!p) return 0;
    ModuleState_Construct(p, 0, TRUE);
    F(void*, p, 0) = g_baseModuleStateVtbl;
    return p;
}

// [FUN_63a115] AfxGetThreadState
extern "C" BYTE* __cdecl Afx_GetThreadState() {
    return (BYTE*)ThreadLocal_GetData(&g_afxThreadState, 0, (void* (__stdcall*)())ThreadState_New);
}

// [FUN_63a342] AfxGetModuleState
extern "C" BYTE* __stdcall Afx_GetModuleState() {
    BYTE* m = F(BYTE*, Afx_GetThreadState(), 4);
    if (!m) m = (BYTE*)ProcessLocal_GetData(&g_afxBaseModuleState, 0, (void* (__stdcall*)())BaseModuleState_New);
    return m;
}

// [FUN_63a368] AfxGetModuleThreadState
extern "C" BYTE* __cdecl Afx_GetModuleThreadState() {
    return (BYTE*)ThreadLocal_GetData(&F(int, Afx_GetModuleState(), 4208), 0, (void* (__stdcall*)())ModuleThreadState_New);
}

// [FUN_632992] AfxGetThread: the current CWinThread, else the app
extern "C" BYTE* __stdcall Afx_GetThread() {
    BYTE* t = F(BYTE*, Afx_GetModuleThreadState(), 4);
    if (!t) t = F(BYTE*, Afx_GetModuleState(), 4);
    return t;
}

// [FUN_639967] AfxOleGetUserCtrl
extern "C" BOOL __stdcall Afx_OleGetUserCtrl() { return F(BOOL, Afx_GetModuleState(), 48); }
KING_ALIAS("?AfxOleGetUserCtrl@@YGHXZ", "_Afx_OleGetUserCtrl@0")

// CWinThread::m_lpfnOleTermOrFreeLib (+84)
typedef void (AFXAPI* OleTermFn)(BOOL, BOOL);

// [FUN_63b4d3] AfxPostQuitMessage
extern "C" void __stdcall Afx_PostQuitMessage(int code) {
    BYTE* t = Afx_GetThread();
    if (t && F(OleTermFn, t, 84)) F(OleTermFn, t, 84)(TRUE, TRUE);
    PostQuitMessage(code);
}
KING_ALIAS("?AfxPostQuitMessage@@YGXH@Z", "_Afx_PostQuitMessage@4")

// [FUN_637385] AfxGlobalFree: frees a locked global block
extern "C" void __stdcall Afx_GlobalFree(HGLOBAL h) {
    if (!h) return;
    for (UINT n = GlobalFlags(h) & GMEM_LOCKCOUNT; n; n--) GlobalUnlock(h);
    GlobalFree(h);
}

// ---------------------------------------------------------------------------
// Temporary handle maps

CRT_PROC(FUN_620815, "__msize")                 // _msize

// [FUN_6367e0] AfxLockTempMaps
extern "C" void __cdecl Afx_LockTempMaps() { ++F(int, Afx_GetModuleThreadState(), 16); }

// [FUN_6367e9] AfxUnlockTempMaps: deletes the temporary CWnd/CDC/... objects
// when the last lock goes; refills the safety pool
extern "C" BOOL __stdcall Afx_UnlockTempMaps(BOOL deleteTemps) {
    BYTE* mts = Afx_GetModuleThreadState();
    if (F(int, mts, 16) != 0 && --F(int, mts, 16) == 0) {
        if (deleteTemps) {
            if (deleteTemps != -1) {
                BYTE* t = Afx_GetThread();
                if (t && F(OleTermFn, t, 84)) F(OleTermFn, t, 84)(FALSE, FALSE);
            }
            HandleMap_DeleteTemp(F(BYTE*, mts, 32));
            HandleMap_DeleteTemp(F(BYTE*, mts, 28));
            HandleMap_DeleteTemp(F(BYTE*, mts, 24));
            HandleMap_DeleteTemp(F(BYTE*, mts, 20));
            HandleMap_DeleteTemp(F(BYTE*, mts, 36));
        }
        BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
        BYTE* ts = Afx_GetThreadState();
        if (app) {
            void*& pool = F(void*, ts, 12);
            size_t want = F(size_t, app, 184);
            if ((!pool || CallC<size_t>(FUN_620815, pool) < want) && want) {
                size_t old = 0;
                if (pool) {
                    old = CallC<size_t>(FUN_620815, pool);
                    free(pool);
                }
                pool = malloc(want);
                if (!pool && old) pool = malloc(old);
            }
        }
    }
    return F(int, mts, 16) != 0;
}

// ---------------------------------------------------------------------------
// CCmdTarget, CWinThread, CWinApp

KING_VAR(AsmLabel, g_winThreadVtbl, 653570)
KING_VAR(AsmLabel, g_winAppVtbl, 6534a8)
KING_VAR(void*, g_afx72c160, 72c160)
KING_VAR(void*, g_afx72c164, 72c164)
CRT_PROC(FUN_62003b, "__strdup")                // _strdup

static char* StrDup(const char* s) { return CallC<char*>(FUN_62003b, s); }

// [FUN_63317c] CCmdTarget::CCmdTarget
extern "C" BYTE* __fastcall CmdTarget_Construct(BYTE* self) {
    F(int, self, 4) = 1;            // m_dwRef
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;           // m_xDispatch
    F(int, self, 20) = 1;
    F(int, self, 24) = 0;
    return self;
}

// [FUN_633196] CCmdTarget::~CCmdTarget
extern "C" void __fastcall CmdTarget_Destruct(BYTE* self) {
    void** dispatch = F(void**, self, 16);
    if (dispatch) ((void (__thiscall*)(void*))dispatch[7])(self + 16);   // Disconnect
}

// [FUN_639d74] CWinThread::CommonConstruct
extern "C" void __fastcall WinThread_CommonConstruct(BYTE* self) {
    F(void*, self, 28) = 0;         // m_pMainWnd
    F(void*, self, 32) = 0;         // m_pActiveWnd
    F(HANDLE, self, 40) = 0;        // m_hThread
    F(DWORD, self, 44) = 0;         // m_nThreadID
    F(int, self, 52) = 0;
    F(int, self, 100) = 0;
    GetCursorPos(&F(POINT, self, 92));
    F(int, self, 88) = 0;
    F(void*, self, 84) = 0;         // m_lpfnOleTermOrFreeLib
    F(BOOL, self, 36) = TRUE;       // m_bAutoDelete
}

// [FUN_639d37] CWinThread::CWinThread()
extern "C" BYTE* __fastcall WinThread_Construct(BYTE* self) {
    CmdTarget_Construct(self);
    F(void*, self, 0) = g_winThreadVtbl;
    F(void*, self, 76) = 0;         // m_pThreadParams
    F(void*, self, 80) = 0;         // m_pfnThreadProc
    WinThread_CommonConstruct(self);
    return self;
}

// [FUN_639cd7] CWinThread::CWinThread(pfnThreadProc, pParam)
extern "C" BYTE* __fastcall WinThread_ConstructProc(BYTE* self, void*, void* proc, void* param) {
    CmdTarget_Construct(self);
    F(void*, self, 80) = proc;
    F(void*, self, 0) = g_winThreadVtbl;
    F(void*, self, 76) = param;
    WinThread_CommonConstruct(self);
    return self;
}

// [FUN_63b4f6] CWinThread::~CWinThread
extern "C" void __fastcall WinThread_Destruct(BYTE* self) {
    F(void*, self, 0) = g_winThreadVtbl;
    if (F(HANDLE, self, 40)) CloseHandle(F(HANDLE, self, 40));
    BYTE* mts = Afx_GetModuleThreadState();
    if (F(BYTE*, mts, 4) == self) F(BYTE*, mts, 4) = 0;
    CmdTarget_Destruct(self);
}

// [FUN_639994] CWinApp::CWinApp(lpszAppName)
extern "C" BYTE* __fastcall WinApp_Construct(BYTE* self, void*, const char* appName) {
    WinThread_Construct(self);
    F(void*, self, 0) = g_winAppVtbl;
    F(char*, self, 120) = appName ? StrDup(appName) : 0;   // m_pszAppName
    BYTE* ms = Afx_GetModuleState();
    F(BYTE*, ThreadLocal_GetData(&F(int, ms, 4208), 0, (void* (__stdcall*)())ModuleThreadState_New), 4) = self;
    F(HANDLE, self, 40) = GetCurrentThread();
    F(DWORD, self, 44) = GetCurrentThreadId();
    F(BYTE*, ms, 4) = self;
    static const int zero[] = { 104, 140, 144, 124, 136, 168, 128, 112, 172, 160, 164, 148, 152, 180, 188, 132 };
    for (int off : zero) F(int, self, off) = 0;
    F(WORD, self, 178) = 0;
    F(WORD, self, 176) = 0;
    F(int, self, 184) = 512;        // m_nSafetyPoolSize
    return self;
}

// [FUN_639ade] CWinApp::~CWinApp
extern "C" void __fastcall WinApp_Destruct(BYTE* self) {
    F(void*, self, 0) = g_winAppVtbl;
    if (F(void*, self, 128)) CallVirt(F(void*, self, 128), 4, 1);     // delete m_pDocManager
    if (F(void*, self, 168)) CallVirt(F(void*, self, 168), 20, 1);    // m_pRecentFileList
    if (!F(BYTE, Afx_GetModuleState(), 20)) {
        if (g_afx72c164) { CallVirt(g_afx72c164, 4, 1); g_afx72c164 = 0; }
        if (g_afx72c160) { CallVirt(g_afx72c160, 4, 1); g_afx72c160 = 0; }
    }
    if (F(HGLOBAL, self, 148)) Afx_GlobalFree(F(HGLOBAL, self, 148));   // m_hDevMode
    if (F(HGLOBAL, self, 152)) Afx_GlobalFree(F(HGLOBAL, self, 152));   // m_hDevNames
    if (F(ATOM, self, 176)) GlobalDeleteAtom(F(ATOM, self, 176));
    if (F(ATOM, self, 178)) GlobalDeleteAtom(F(ATOM, self, 178));
    if (F(void*, self, 172)) CallVirt(F(void*, self, 172), 4, 1);
    BYTE* ms = Afx_GetModuleState();
    if (F(char*, ms, 16) == F(char*, self, 120)) F(char*, ms, 16) = 0;
    if (F(BYTE*, ms, 4) == self) F(BYTE*, ms, 4) = 0;
    free(F(char*, self, 120));      // m_pszAppName
    free(F(char*, self, 124));      // m_pszRegistryKey
    free(F(char*, self, 136));      // m_pszExeName
    free(F(char*, self, 140));      // m_pszHelpFilePath
    free(F(char*, self, 144));      // m_pszProfileName
    F(HANDLE, self, 40) = 0;        // the main thread's handle isn't closed
    WinThread_Destruct(self);
}

// ---------------------------------------------------------------------------
// Threads

ASM_PROC($L_632864)                             // _AfxThreadEntry
CRT_PROC(FUN_620066, "__beginthreadex")         // _beginthreadex
CRT_PROC(FUN_62016d, "__endthreadex")           // _endthreadex
KING_VAR(int, g_ctl3dThreadState, 72dd08)     // CThreadLocal<_AFX_CTL3D_THREAD>
KING_VAR(void*, g_ctl3dState, 72dd0c)         // CProcessLocal<_AFX_CTL3D_STATE>
extern "C" void $L_639de0(); KING_ALIAS("_$L_639de0", "_Ctl3dState_New")                             // new _AFX_CTL3D_STATE
extern "C" BYTE* __cdecl Ctl3dThread_New();       // [$L_639df5] new _AFX_CTL3D_THREAD
extern "C" void $L_632f34(); KING_ALIAS("_$L_632f34", "_Afx_MsgFilterHook@12")                             // _AfxMsgFilterHook

// [FUN_632b5c] CWinThread::CreateThread: starts the thread suspended and
// waits until _AfxThreadEntry has set it up
extern "C" BOOL __fastcall WinThread_CreateThread(BYTE* self, void*, DWORD flags, UINT stackSize, LPSECURITY_ATTRIBUTES sa) {
    struct Startup {              // _AFX_THREAD_STARTUP
        BYTE* threadState;
        BYTE* thread;
        DWORD createFlags;
        int unused;
        HANDLE event;
        HANDLE event2;
        BOOL error;
    } st;
    memset(&st, 0, sizeof st);
    st.threadState = Afx_GetThreadState();
    st.thread = self;
    st.event = CreateEventA(0, TRUE, FALSE, 0);
    st.event2 = CreateEventA(0, TRUE, FALSE, 0);
    st.createFlags = flags;
    if (st.event) {
        if (st.event2) {
            HANDLE h = CallC<HANDLE>(FUN_620066, sa, stackSize, (void*)$L_632864, &st, flags | CREATE_SUSPENDED, &F(DWORD, self, 44));
            F(HANDLE, self, 40) = h;
            if (!h) return FALSE;
            ResumeThread(h);
            WaitForSingleObject(st.event, INFINITE);
            CloseHandle(st.event);
            if (flags & CREATE_SUSPENDED) SuspendThread(F(HANDLE, self, 40));
            if (!st.error) {
                SetEvent(st.event2);
                return TRUE;
            }
            WaitForSingleObject(F(HANDLE, self, 40), INFINITE);
            CloseHandle(F(HANDLE, self, 40));
            F(HANDLE, self, 40) = 0;
            CloseHandle(st.event2);
            return FALSE;
        }
        CloseHandle(st.event);
    }
    if (st.event2) CloseHandle(st.event2);
    return FALSE;
}

// [FUN_6329a7] AfxBeginThread (worker thread form)
extern "C" BYTE* __stdcall Afx_BeginThread(void* proc, void* param, int priority, UINT stackSize, DWORD flags, LPSECURITY_ATTRIBUTES sa) {
    BYTE* p = (BYTE*)operator new(104);
    BYTE* t = p ? WinThread_ConstructProc(p, 0, proc, param) : 0;
    if (!WinThread_CreateThread(t, 0, flags | CREATE_SUSPENDED, stackSize, sa)) {
        CallVirt(t, 120);           // Delete
        return 0;
    }
    SetThreadPriority(F(HANDLE, t, 40), priority);
    if (!(flags & CREATE_SUSPENDED)) ResumeThread(F(HANDLE, t, 40));
    return t;
}

// [FUN_632acc] AfxTermThread
extern "C" void __stdcall Afx_TermThread(HINSTANCE hInst) {
    Afx_LockTempMaps();
    Afx_UnlockTempMaps(-1);
    if (!hInst) {
        BYTE* ts = (BYTE*)ThreadLocal_GetDataNA(&g_afxThreadState);
        if (ts && F(void*, ts, 204)) {
            CallVirt(F(void*, ts, 204), 88);          // DestroyWindow
            if (F(void*, ts, 204)) CallVirt(F(void*, ts, 204), 4, 1);
            F(void*, ts, 204) = 0;
        }
    }
    if (g_afxThreadData) SlotData_DeleteValues(g_afxThreadData, 0, hInst, FALSE);
}

// [FUN_632a28] AfxEndThread
extern "C" void __stdcall Afx_EndThread(UINT exitCode, BOOL deleteThread) {
    BYTE* mts = Afx_GetModuleThreadState();
    BYTE* t = F(BYTE*, mts, 4);
    if (t) {
        if (F(OleTermFn, t, 84)) F(OleTermFn, t, 84)(TRUE, FALSE);
        if (deleteThread) CallVirt(t, 120);          // Delete
        F(BYTE*, mts, 4) = 0;
    }
    Afx_TermThread(0);
    CallC(FUN_62016d, exitCode);
}

// [FUN_632a6d] AfxInitThread: message filter hook and CTL3D (not for the
// base module, which counts as a DLL)
extern "C" void __cdecl Afx_InitThread() {
    if (F(BYTE, Afx_GetModuleState(), 20)) return;
    BYTE* ts = Afx_GetThreadState();
    F(HHOOK, ts, 48) = SetWindowsHookExA(WH_MSGFILTER, (HOOKPROC)$L_632f34, 0, GetCurrentThreadId());
    BYTE* ctl = (BYTE*)ProcessLocal_GetData(&g_ctl3dState, 0, (void* (__stdcall*)())$L_639de0);
    typedef BOOL (WINAPI* Ctl3dFn)(HINSTANCE);
    if (F(Ctl3dFn, ctl, 20)) F(Ctl3dFn, ctl, 20)(F(HINSTANCE, Afx_GetModuleState(), 8));
    ThreadLocal_GetData(&g_ctl3dThreadState, 0, (void* (__stdcall*)())Ctl3dThread_New);
}

// ---------------------------------------------------------------------------
// WinMain

CRT_PROC(FUN_620d0e, "__mbsrchr")               // _tcsrchr
CRT_PROC(FUN_620585, "__mbschr")                // _tcschr
CRT_PROC(FUN_620903, "__mbsinc")                // _tcsinc
KING_VAR(char, g_extHlp, 6546dc)
KING_VAR(char, g_extIni, 6546d4)

// [FUN_63b450] AfxGetFileName: the file name part of a path
extern "C" UINT __stdcall Afx_GetFileName(const char* path, char* title, UINT max) {
    const char* name = path;
    for (const char* p = path; *p; ) {
        if (*p == '\\' || *p == '/' || *p == ':') name = CallC<const char*>(FUN_620903, p);
        p = CallC<const char*>(FUN_620903, p);
    }
    if (!title) return lstrlenA(name) + 1;
    lstrcpynA(title, name, max);
    return 0;
}

// [FUN_63b333] CWinApp::SetCurrentHandles: instance, exe name, title,
// help and profile file names
extern "C" void __fastcall WinApp_SetCurrentHandles(BYTE* self) {
    BYTE* ms = Afx_GetModuleState();
    HINSTANCE h = F(HINSTANCE, self, 104);
    F(HINSTANCE, ms, 8) = h;
    F(HINSTANCE, ms, 12) = h;
    char path[260], exe[260], title[256];
    GetModuleFileNameA(h, path, 260);
    char* dot = CallC<char*>(FUN_620d0e, path, '.');
    *dot = 0;
    Afx_GetFileName(path, exe, 260);
    if (!F(char*, self, 136)) F(char*, self, 136) = StrDup(exe);
    if (!F(char*, self, 120))
        F(char*, self, 120) = StrDup(Afx_LoadString(0xE000, title, 256) ? title : F(char*, self, 136));
    F(char*, ms, 16) = F(char*, self, 120);
    if (!F(char*, self, 140)) {
        lstrcpyA(dot, &g_extHlp);
        F(char*, self, 140) = StrDup(path);
        *dot = 0;
    }
    if (!F(char*, self, 144)) {
        lstrcatA(exe, &g_extIni);
        F(char*, self, 144) = StrDup(exe);
    }
}

// [FUN_63b2d0] AfxWinInit
extern "C" BOOL __stdcall Afx_WinInit(HINSTANCE h, HINSTANCE prev, char* cmdLine, int show) {
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    BYTE* ms = Afx_GetModuleState();
    F(HINSTANCE, ms, 8) = h;
    F(HINSTANCE, ms, 12) = h;
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app) {
        F(HINSTANCE, app, 104) = h;
        F(HINSTANCE, app, 108) = prev;
        F(char*, app, 112) = cmdLine;
        F(int, app, 116) = show;
        WinApp_SetCurrentHandles(app);
    }
    if (!F(BYTE, Afx_GetModuleState(), 20)) Afx_InitThread();
    return TRUE;
}

// [FUN_63b54f] AfxWinTerm
extern "C" void __cdecl Afx_WinTerm() {
    BYTE* ms = Afx_GetModuleState();
    Afx_LockGlobals(1);
    char* name = (char*)ms + 52;
    while (*name) {
        char* end = CallC<char*>(FUN_620585, name, '\n');
        *end = 0;
        UnregisterClassA(name, F(HINSTANCE, Afx_GetModuleState(), 8));
        name = end + 1;
    }
    F(char, ms, 52) = 0;
    Afx_UnlockGlobals(1);
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app && F(OleTermFn, app, 84)) F(OleTermFn, app, 84)(TRUE, FALSE);
    BYTE* ts = Afx_GetThreadState();
    if (F(void*, ts, 204) && CallVirt<BOOL>(F(void*, ts, 204), 184)) F(void*, ts, 204) = 0;
    if (!F(BYTE, Afx_GetModuleState(), 20)) {
        if (F(HHOOK, ts, 48)) { UnhookWindowsHookEx(F(HHOOK, ts, 48)); F(HHOOK, ts, 48) = 0; }
        if (F(HHOOK, ts, 44)) { UnhookWindowsHookEx(F(HHOOK, ts, 44)); F(HHOOK, ts, 44) = 0; }
    }
}

// [FUN_636b51] AfxWinMain: InitApplication, InitInstance, Run
extern "C" int __stdcall Afx_WinMain(HINSTANCE h, HINSTANCE prev, char* cmdLine, int show) {
    int result = -1;
    BYTE* thread = Afx_GetThread();
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (Afx_WinInit(h, prev, cmdLine, show) && (!app || CallVirt<BOOL>(app, 132))) {
        if (CallVirt<BOOL>(thread, 80)) {
            result = CallVirt<int>(thread, 84);                 // Run
        } else {
            if (F(void*, thread, 28)) CallVirt(F(void*, thread, 28), 88);   // m_pMainWnd->DestroyWindow
            result = CallVirt<int>(thread, 104);                // ExitInstance
        }
    }
    Afx_WinTerm();
    return result;
}

// ---------------------------------------------------------------------------
// CWnd: the handle map, attaching, window procedures and message dispatch
//
// CWnd (60 bytes): +28 m_hWnd, +36 m_nFlags, +40 m_pfnSuper, +48 m_pDropTarget,
//   +52 m_pCtrlCont, +56 m_pCtrlSite.
// Message maps: AFX_MSGMAP { pBaseMap; lpEntries }, entries of 24 bytes
//   { nMessage, nCode, nID, nLastID, nSig, pfn }, ended by nSig 0.

struct MsgMapEntry { UINT message, code, id, lastId; UINT_PTR sig; void (*pfn)(); };
struct MsgMap { const MsgMap* base; const MsgMapEntry* entries; };
struct MsgCache { UINT message; const MsgMapEntry* entry; const MsgMap* map; };

KING_VAR(MsgCache, g_msgCache, 72c170)        // MsgCache[512]
KING_VAR(AsmLabel, g_wndVtbl, 653b00)
KING_VAR(char, g_oldWndProcProp, 6539f8)      // "AfxOldWndProc423"
KING_VAR(char, g_imeClass, 653bb8)            // "ime"
KING_VAR(int, g_afxDBCS, 72e0e4)
KING_VAR(int, g_afxCtl3dOff, 72e0d4)
KING_VAR(AsmLabel, g_wndTop, 72da58)          // the static wndTop, wndBottom,
KING_VAR(AsmLabel, g_wndBottom, 72da18)       // wndTopMost and wndNoTopMost,
KING_VAR(AsmLabel, g_wndTopMost, 72d9d8)      // never destroyed
KING_VAR(AsmLabel, g_wndNoTopMost, 72d998)
KING_VAR(AsmLabel, g_tempWndClass, 653708)    // CTempWnd's CRuntimeClass
extern "C" void $L_6373bb(); KING_ALIAS("_$L_6373bb", "_Afx_CriticalNewHandler")                             // new handler that throws
extern "C" void FUN_633a2e(); KING_ALIAS("_FUN_633a2e", "_Afx_PreInitDialog@12")                            // _AfxPreInitDialog
extern "C" void FUN_633a51(); KING_ALIAS("_FUN_633a51", "_Afx_PostInitDialog@12")                            // _AfxPostInitDialog
extern "C" void FUN_61c701(); KING_ALIAS("_FUN_61c701", "@Wnd_GetOwner@4")                            // CWnd::GetParentOwner part
extern "C" void FUN_61c4c1(); KING_ALIAS("_FUN_61c4c1", "_Afx_GetMainWnd")                            // AfxGetMainWnd
extern "C" void FUN_636ade(); KING_ALIAS("_FUN_636ade", "_Menu_FromHandle@4")                            // CMenu::FromHandle
extern "C" void FUN_639948(); KING_ALIAS("_FUN_639948", "_Afx_OleCanExitApp")                            // AfxOleCanExitApp
extern "C" void $L_633fa2(); KING_ALIAS("_$L_633fa2", "_Afx_GrayBackgroundWndProc@16")                             // _AfxActivationWndProc (CTL3D)

#define HWND_OF(w) F(HWND, w, 28)

// [FUN_633cf0] afxMapHWND: this thread's HWND -> CWnd map
extern "C" BYTE* __stdcall Afx_MapHWND(BOOL create) {
    BYTE* mts = Afx_GetModuleThreadState();
    if (!F(BYTE*, mts, 20) && create) {
        void* old = Afx_SetNewHandler((void*)$L_6373bb);
        BYTE* p = (BYTE*)operator new(72);
        F(BYTE*, mts, 20) = p ? HandleMap_Construct(p, 0, (BYTE*)g_tempWndClass, 28, 1) : 0;
        Afx_SetNewHandler(old);
    }
    return F(BYTE*, mts, 20);
}

// [FUN_633810] lets the parent's control container attach a control site
extern "C" void __fastcall Wnd_AttachControlSite(BYTE* self, void*, BYTE* map) {
    if (!self || F(void*, self, 56)) return;
    BYTE* parent = (BYTE*)PtrMap_GetValueAt((PtrMap*)(map + 4), 0, GetParent(HWND_OF(self)));
    if (parent && F(void*, parent, 52)) CallVirt(F(void*, parent, 52), 140, self);
}

// [FUN_633d62] CWnd::FromHandle: permanent or temporary CWnd
extern "C" BYTE* __stdcall Wnd_FromHandle(HWND h) {
    BYTE* map = Afx_MapHWND(TRUE);
    BYTE* w = HandleMap_FromHandle(map, 0, h);
    Wnd_AttachControlSite(w, 0, map);
    return w;
}

// [FUN_633d89] CWnd::FromHandlePermanent
extern "C" BYTE* __stdcall Wnd_FromHandlePermanent(HWND h) {
    BYTE* map = Afx_MapHWND(FALSE);
    return map ? (BYTE*)PtrMap_GetValueAt((PtrMap*)(map + 4), 0, h) : 0;
}

// [FUN_633da7] CWnd::Attach
extern "C" BOOL __fastcall Wnd_Attach(BYTE* self, void*, HWND h) {
    if (!h) return FALSE;
    BYTE* map = Afx_MapHWND(TRUE);
    HWND_OF(self) = h;
    *(BYTE**)PtrMap_At((PtrMap*)(map + 4), 0, h) = self;
    Wnd_AttachControlSite(self, 0, map);
    return TRUE;
}

// [FUN_633de0] CWnd::Detach
extern "C" HWND __fastcall Wnd_Detach(BYTE* self) {
    HWND h = HWND_OF(self);
    if (h) {
        BYTE* map = Afx_MapHWND(FALSE);
        if (map) PtrMap_RemoveKey((PtrMap*)(map + 4), 0, h);
        HWND_OF(self) = 0;
    }
    F(void*, self, 56) = 0;
    return h;
}

// [FUN_63395b] CWnd::CWnd
extern "C" BYTE* __fastcall Wnd_Construct(BYTE* self) {
    CmdTarget_Construct(self);
    F(void*, self, 0) = g_wndVtbl;
    memset(self + 28, 0, 32);
    return self;
}

// [FUN_634592] CWnd::DestroyWindow
extern "C" BOOL __fastcall Wnd_DestroyWindow(BYTE* self) {
    if (!HWND_OF(self)) return FALSE;
    BYTE* map = Afx_MapHWND(FALSE);
    BYTE* permanent = (BYTE*)PtrMap_GetValueAt((PtrMap*)(map + 4), 0, HWND_OF(self));
    BOOL r = F(void*, self, 56) ? CallVirt<BOOL>(F(void*, self, 56), 80) : DestroyWindow(HWND_OF(self));
    if (!permanent) Wnd_Detach(self);
    return r;
}

// [FUN_6343d4] CWnd::~CWnd
extern "C" void __fastcall Wnd_Destruct(BYTE* self) {
    F(void*, self, 0) = g_wndVtbl;
    if (HWND_OF(self) && self != (BYTE*)g_wndTop && self != (BYTE*)g_wndBottom
        && self != (BYTE*)g_wndTopMost && self != (BYTE*)g_wndNoTopMost)
        Wnd_DestroyWindow(self);
    if (F(void*, self, 52)) CallVirt(F(void*, self, 52), 4, 1);
    BYTE* site = F(BYTE*, self, 56);
    if (site && F(BYTE*, site, 36) == self) F(BYTE*, site, 36) = 0;
    CmdTarget_Destruct(self);
}

// [FUN_633786] CWnd::ShowWindow
extern "C" BOOL __fastcall Wnd_ShowWindow(BYTE* self, void*, int cmd) {
    if (!F(void*, self, 56)) return ShowWindow(HWND_OF(self), cmd);
    return CallVirt<BOOL>(F(void*, self, 56), 160, cmd);
}

// [FUN_633737] CWnd::SetWindowPos
extern "C" BOOL __fastcall Wnd_SetWindowPos(BYTE* self, void*, BYTE* after, int x, int y, int cx, int cy, UINT flags) {
    if (!F(void*, self, 56))
        return SetWindowPos(HWND_OF(self), after ? HWND_OF(after) : 0, x, y, cx, cy, flags);
    return CallVirt<BOOL>(F(void*, self, 56), 156, after, x, y, cx, cy, flags);
}

// [$L_6345df] CWnd::DefWindowProc
extern "C" LRESULT __fastcall Wnd_DefWindowProc(BYTE* self, void*, UINT msg, WPARAM w, LPARAM l) {
    WNDPROC proc = F(WNDPROC, self, 40);
    if (!proc) proc = *CallVirt<WNDPROC*>(self, 128);              // GetSuperWndProcAddr
    if (!proc) return DefWindowProcA(HWND_OF(self), msg, w, l);
    return CallWindowProcA(proc, HWND_OF(self), msg, w, l);
}

// [FUN_633cc9] CWnd::Default: the current message to DefWindowProc
extern "C" LRESULT __fastcall Wnd_Default(BYTE* self) {
    BYTE* ts = Afx_GetThreadState();
    return CallVirt<LRESULT>(self, 160, F(UINT, ts, 56), F(WPARAM, ts, 60), F(LPARAM, ts, 64));
}

// [FUN_633c94] AfxGetCurrentMessage
extern "C" MSG* __cdecl Afx_GetCurrentMessage() {
    BYTE* ts = Afx_GetThreadState();
    F(DWORD, ts, 68) = GetMessageTime();
    DWORD pos = GetMessagePos();
    F(int, ts, 72) = (short)LOWORD(pos);
    F(int, ts, 76) = (short)HIWORD(pos);
    return &F(MSG, ts, 52);
}

// [FUN_634450] CWnd::OnDestroy part: deletes the control container
extern "C" void __fastcall Wnd_OnDestroy(BYTE* self) {
    if (F(void*, self, 52)) CallVirt(F(void*, self, 52), 4, 1);
    F(void*, self, 52) = 0;
    Wnd_Default(self);
}

// [$L_63446e] CWnd::OnNcDestroy
extern "C" void __fastcall Wnd_OnNcDestroy(BYTE* self) {
    BYTE* thread = Afx_GetThread();
    if (thread) {
        if (F(BYTE*, thread, 28) == self) {                 // m_pMainWnd
            if (!F(BYTE, Afx_GetModuleState(), 20)
                && (thread != F(BYTE*, Afx_GetModuleState(), 4) || CallC<BOOL>(FUN_639948)))
                Afx_PostQuitMessage(0);
            F(BYTE*, thread, 28) = 0;
        }
        if (F(BYTE*, thread, 32) == self) F(BYTE*, thread, 32) = 0;
    }
    if (F(void*, self, 48)) {
        CallVirt(F(void*, self, 48), 80);                   // m_pDropTarget->Revoke
        F(void*, self, 48) = 0;
    }
    if (F(void*, self, 52)) CallVirt(F(void*, self, 52), 4, 1);
    F(void*, self, 52) = 0;
    if (F(BYTE, self, 36) & 1) {                            // WF_TOOLTIPS
        BYTE* tip = F(BYTE*, Afx_GetThreadState(), 204);
        if (tip && HWND_OF(tip)) {
            DWORD ti[11];                                   // TOOLINFO of MFC 4.2
            memset(ti, 0, sizeof ti);
            ti[0] = 40;
            ti[1] = 1;                                      // TTF_IDISHWND
            ti[2] = (DWORD)HWND_OF(self);
            ti[3] = (DWORD)HWND_OF(self);
            SendMessageA(HWND_OF(tip), 1029, 0, (LPARAM)ti); // TTM_DELTOOL
        }
    }
    LONG proc = GetWindowLongA(HWND_OF(self), GWL_WNDPROC);
    Wnd_Default(self);
    if (GetWindowLongA(HWND_OF(self), GWL_WNDPROC) == proc) {
        WNDPROC super = *CallVirt<WNDPROC*>(self, 128);
        if (super) SetWindowLongA(HWND_OF(self), GWL_WNDPROC, (LONG)super);
    }
    Wnd_Detach(self);
    CallVirt(self, 164);                                    // PostNcDestroy
}

// [FUN_63462a] calls the module's hook at +4148 (OLE), if any
extern "C" BOOL __fastcall Wnd_ModuleHook(BYTE* self, void*, void* arg) {
    typedef void (__stdcall* Hook)(void*, BYTE*);
    Hook hook = F(Hook, Afx_GetModuleState(), 4148);
    if (hook) hook(arg, self);
    return FALSE;
}

// [FUN_635288] _AfxGetParentOwner
extern "C" HWND __stdcall Afx_GetParentOwner(HWND h) {
    BYTE* w = Wnd_FromHandlePermanent(h);
    if (w) {
        BYTE* p = CallThis<BYTE*>(FUN_61c701, w);
        return p ? HWND_OF(p) : 0;
    }
    if (GetWindowLongA(h, GWL_STYLE) & WS_CHILD) return GetParent(h);
    return GetWindow(h, GW_OWNER);
}

// [FUN_6352cd] CWnd::GetTopLevelParent
extern "C" BYTE* __fastcall Wnd_GetTopLevelParent(BYTE* self) {
    if (!self || !HWND_OF(self)) return 0;
    HWND h = HWND_OF(self);
    for (HWND p; (p = Afx_GetParentOwner(h)) != 0; ) h = p;
    return Wnd_FromHandle(h);
}

// [FUN_633b2b] _AfxHandleSetCursor: a click on a disabled window brings its
// last active popup to the front
extern "C" BOOL __stdcall Afx_HandleSetCursor(BYTE* wnd, int hitTest, UINT msg) {
    if (hitTest != HTERROR || (msg != WM_LBUTTONDOWN && msg != WM_MBUTTONDOWN && msg != WM_RBUTTONDOWN))
        return FALSE;
    BYTE* top = Wnd_GetTopLevelParent(wnd);
    if (!top) return FALSE;
    BYTE* last = Wnd_FromHandle(GetLastActivePopup(HWND_OF(top)));
    if (!last || last == Wnd_FromHandle(GetForegroundWindow()) || !Wnd_IsWindowEnabled(last))
        return FALSE;
    SetForegroundWindow(HWND_OF(last));
    return TRUE;
}

// [FUN_634b21] AfxFindMessageEntry
extern "C" const MsgMapEntry* __stdcall Afx_FindMessageEntry(const MsgMapEntry* e, UINT msg, UINT code, UINT id) {
    for (; e->sig; e++)
        if (e->message == msg && e->code == code && id >= e->id && id <= e->lastId) return e;
    return 0;
}

// [FUN_634ba5] CWnd::OnWndMsg: finds the message map entry (through a cache)
// and calls the handler with the arguments its signature (AfxSig) asks for
extern "C" BOOL __fastcall Wnd_OnWndMsg(BYTE* self, void*, UINT msg, WPARAM w, LPARAM l, LRESULT* result) {
    LRESULT r = 0;
    const MsgMapEntry* e;
    if (msg == WM_COMMAND) {
        if (!CallVirt<BOOL>(self, 120, w, l)) return FALSE;         // OnCommand
        r = 1;
        goto done;
    }
    if (msg == WM_NOTIFY) {
        if (!((NMHDR*)l)->hwndFrom) return FALSE;
        if (!CallVirt<BOOL>(self, 124, w, l, &r)) return FALSE;     // OnNotify
        goto done;
    }
    if (msg == WM_ACTIVATE) Afx_HandleActivate(self, w, Wnd_FromHandle((HWND)l));
    if (msg == WM_SETCURSOR && Afx_HandleSetCursor(self, (short)LOWORD(l), HIWORD(l))) {
        r = 1;
        goto done;
    }
    {
        const MsgMap* map = CallVirt<const MsgMap*>(self, 40);       // GetMessageMap
        UINT hash = ((UINT)map & 511) ^ (msg & 511);
        Afx_LockGlobals(7);
        MsgCache* cache = &g_msgCache + hash;
        if (msg == cache->message && map == cache->map) {
            e = cache->entry;
            Afx_UnlockGlobals(7);
            if (!e) return FALSE;
            if (msg >= 0xC000) goto registered;
        } else {
            cache->message = msg;
            cache->map = map;
            for (; map; map = map->base) {
                if (msg < 0xC000) {
                    e = Afx_FindMessageEntry(map->entries, msg, 0, 0);
                    if (e) {
                        cache->entry = e;
                        Afx_UnlockGlobals(7);
                        goto dispatch;
                    }
                } else {
                    for (e = map->entries; (e = Afx_FindMessageEntry(e, 0xC000, 0, 0)) != 0; e++) {
                        if (*(UINT*)e->sig == msg) {
                            cache->entry = e;
                            Afx_UnlockGlobals(7);
                            goto registered;
                        }
                    }
                }
            }
            cache->entry = 0;
            Afx_UnlockGlobals(7);
            return FALSE;
        }
    }
dispatch:
    {
        void (*pfn)() = e->pfn;
        UINT sig = (UINT)e->sig;
        if (e->id == WM_SETTINGCHANGE) sig = (BYTE)GetVersion() < 4 ? 31 : 47;
        WORD wl = LOWORD(w), wh = HIWORD(w), ll = LOWORD(l), lh = HIWORD(l);
        switch (sig) {
        case 1: r = CallThis<LRESULT>(pfn, self, Dc_FromHandle((HDC)w)); break;
        case 2: r = CallThis<LRESULT>(pfn, self, w); break;
        case 3: case 8:
            r = CallThis<LRESULT>(pfn, self, Wnd_FromHandle((HWND)w), (int)(short)ll, (UINT)lh); break;
        case 4: {                                   // WM_CTLCOLOR: AFX_CTLCOLOR* in lParam
            DWORD* cc = (DWORD*)l;
            BYTE dc[16], tmp[60];
            Cdc_Construct(dc);
            F(DWORD, dc, 4) = cc[1];
            Wnd_Construct(tmp);
            HWND_OF(tmp) = (HWND)cc[0];
            BYTE* wnd = Wnd_FromHandlePermanent((HWND)cc[0]);
            if (!wnd) {
                if (F(BYTE*, self, 52)) {
                    void* site = PtrMap_GetValueAt((PtrMap*)(F(BYTE*, self, 52) + 32), 0, (HWND)cc[0]);
                    if (site) F(void*, tmp, 56) = site;
                }
                wnd = tmp;
            }
            r = CallThis<LRESULT>(pfn, self, dc, wnd, (UINT)cc[2]);
            F(DWORD, dc, 4) = 0;
            HWND_OF(tmp) = 0;
            Wnd_Destruct(tmp);
            Cdc_Destruct(dc);
            break;
        }
        case 5: {
            DWORD* cc = (DWORD*)l;
            BYTE dc[16];
            Cdc_Construct(dc);
            F(DWORD, dc, 4) = cc[1];
            r = CallThis<LRESULT>(pfn, self, dc, (UINT)cc[2]);
            F(DWORD, dc, 4) = 0;
            Cdc_Destruct(dc);
            break;
        }
        case 6: r = CallThis<LRESULT>(pfn, self, (UINT)wl, Wnd_FromHandle((HWND)l), (UINT)wh); break;
        case 7: r = CallThis<LRESULT>(pfn, self, (UINT)wl, (UINT)wh); break;
        case 9: case 42: r = CallThis<LRESULT>(pfn, self, l); break;
        case 10: case 33: r = CallThis<LRESULT>(pfn, self, w, l); break;
        case 11: r = CallThis<LRESULT>(pfn, self, (UINT)wl, (UINT)wh, CallStd<void*>(FUN_636ade, l)); break;
        case 12: CallThis(pfn, self); break;
        case 13: CallThis(pfn, self, w); break;
        case 14: case 18: case 37: case 47: CallThis(pfn, self, w, l); break;
        case 15: CallThis(pfn, self, (int)(short)ll, (int)(short)lh); break;
        case 16: case 17: CallThis(pfn, self, w, (UINT)ll, (UINT)lh); break;
        case 19: {
            BYTE* deact = Wnd_FromHandle((HWND)w);
            BYTE* act = Wnd_FromHandle((HWND)l);
            CallThis(pfn, self, (BOOL)(HWND_OF(self) == (HWND)l), act, deact);
            break;
        }
        case 20: CallThis(pfn, self, Dc_FromHandle((HDC)w)); break;
        case 21: CallThis(pfn, self, CallStd<void*>(FUN_636ade, w)); break;
        case 22: CallThis(pfn, self, CallStd<void*>(FUN_636ade, w), (UINT)ll, (UINT)lh); break;
        case 23: CallThis(pfn, self, Wnd_FromHandle((HWND)w)); break;
        case 24: CallThis(pfn, self, Wnd_FromHandle((HWND)w), (UINT)ll, (UINT)lh); break;
        case 25: CallThis(pfn, self, Wnd_FromHandle((HWND)w), (int)(short)ll, (int)(short)lh); break;
        case 26: CallThis(pfn, self, Wnd_FromHandle((HWND)w), l); break;
        case 27: CallThis(pfn, self, w, Wnd_FromHandle((HWND)l)); break;
        case 28: CallThis(pfn, self, (UINT)wl, Wnd_FromHandle((HWND)l), (UINT)wh); break;
        case 29: CallThis(pfn, self, (int)(short)wl, (int)(short)wh, Wnd_FromHandle((HWND)l)); break;
        case 30: CallThis(pfn, self, (int)(short)wl, (int)(short)wh); break;
        case 31: case 36: CallThis(pfn, self, l); break;
        case 32: case 43: CallThis(pfn, self, w, l); r = 1; break;
        case 34: r = CallThis<LRESULT>(pfn, self, (int)(short)ll, (int)(short)lh); break;
        case 35: r = CallThis<LRESULT>(pfn, self); break;
        case 44: CallThis(pfn, self, Wnd_FromHandle((HWND)l)); break;
        case 45: r = CallThis<LRESULT>(pfn, self, Wnd_FromHandle((HWND)w), l); break;
        case 46:
            r = CallThis<LRESULT>(pfn, self, (UINT)wl, (UINT)wh, (UINT)ll, (UINT)lh);
            if (!r) return FALSE;
            break;
        case 48: CallThis(pfn, self, (UINT)wl, (UINT)wh, l); break;
        case 49: CallThis(pfn, self, w, (int)(short)ll, (int)(short)lh); break;
        default: break;                             // 38..41 (WM_NOTIFY forms) and unknown
        }
        goto done;
    }
registered:
    r = CallThis<LRESULT>(e->pfn, self, w, l);
done:
    if (result) *result = r;
    return TRUE;
}

// [$L_634b61] CWnd::WindowProc
extern "C" LRESULT __fastcall Wnd_WindowProc(BYTE* self, void*, UINT msg, WPARAM w, LPARAM l) {
    LRESULT r = 0;
    if (!CallVirt<BOOL>(self, 156, msg, w, l, &r))            // OnWndMsg
        r = CallVirt<LRESULT>(self, 160, msg, w, l);           // DefWindowProc
    return r;
}

// [FUN_633ba1..$L_633c72] AfxCallWndProc: keeps the message as the thread's
// current one while the window handles it
extern "C" LRESULT __stdcall Afx_CallWndProc(BYTE* wnd, HWND h, UINT msg, WPARAM w, LPARAM l) {
    BYTE* ts = Afx_GetThreadState();
    DWORD saved[7];
    memcpy(saved, ts + 52, sizeof saved);
    F(HWND, ts, 52) = h;
    F(UINT, ts, 56) = msg;
    F(WPARAM, ts, 60) = w;
    F(LPARAM, ts, 64) = l;
    if (msg == WM_DESTROY && F(void*, wnd, 52)) CallVirt(F(void*, wnd, 52), 92, 0);
    DWORD style = 0;
    BYTE rect[16];
    if (msg == WM_INITDIALOG) CallStd(FUN_633a2e, wnd, rect, &style);
    LRESULT r = CallVirt<LRESULT>(wnd, 152, msg, w, l);        // WindowProc
    if (msg == WM_INITDIALOG) CallStd(FUN_633a51, wnd, rect, style);
    memcpy(ts + 52, saved, sizeof saved);
    return r;
}

// [$L_633e0e] AfxWndProc: the window procedure of all MFC windows
extern "C" LRESULT __stdcall Afx_WndProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    if (msg == 0x360) return 1;                                 // WM_QUERYAFXWNDPROC
    return Afx_CallWndProc(Wnd_FromHandlePermanent(h), h, msg, w, l);
}

// [FUN_633e3d] AfxGetAfxWndProc
extern "C" WNDPROC __cdecl Afx_GetAfxWndProc() { return Afx_WndProc; }

// [FUN_633e43] _AfxActivationWndProc: subclass of windows MFC didn't make
// (dialog boxes and the like) for activation and the cursor
extern "C" LRESULT __stdcall Afx_ActivationWndProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    WNDPROC old = (WNDPROC)GetPropA(h, &g_oldWndProcProp);
    LRESULT r = 0;
    BOOL callDefault = TRUE;
    switch (msg) {
    case WM_INITDIALOG: {
        BYTE* wnd = Wnd_FromHandle(h);
        DWORD style;
        BYTE rect[16];
        CallStd(FUN_633a2e, wnd, rect, &style);
        r = CallWindowProcA(old, h, WM_INITDIALOG, w, l);
        CallStd(FUN_633a51, wnd, rect, style);
        return r;
    }
    case WM_NCDESTROY:
        SetWindowLongA(h, GWL_WNDPROC, (LONG)old);
        RemovePropA(h, &g_oldWndProcProp);
        GlobalDeleteAtom(GlobalFindAtomA(&g_oldWndProcProp));
        break;
    case WM_ACTIVATE: {
        BYTE* other = Wnd_FromHandle((HWND)l);
        Afx_HandleActivate(Wnd_FromHandle(h), w, other);
        break;
    }
    case WM_SETCURSOR:
        callDefault = !Afx_HandleSetCursor(Wnd_FromHandle(h), (short)LOWORD(l), HIWORD(l));
        break;
    }
    if (callDefault) r = CallWindowProcA(old, h, msg, w, l);
    return r;
}

// [$L_63401e] _AfxCbtFilterHook: subclasses each window as it is created;
// the CWnd being created (m_pWndInit) gets AfxWndProc
extern "C" LRESULT __stdcall Afx_CbtFilterHook(int code, WPARAM w, LPARAM l) {
    BYTE* ts = Afx_GetThreadState();
    if (code != HCBT_CREATEWND) return CallNextHookEx(F(HHOOK, ts, 44), code, w, l);
    CREATESTRUCTA* cs = ((CBT_CREATEWNDA*)l)->lpcs;
    BYTE* init = F(BYTE*, ts, 20);                  // m_pWndInit
    BOOL dll = F(BYTE, Afx_GetModuleState(), 20);
    HWND h = (HWND)w;
    if (!init && ((cs->style & WS_CHILD) || dll)) goto done;
    if (g_afxDBCS) {
        if (GetClassLongA(h, GCL_STYLE) & CS_IME) goto done;
        const char* cls = cs->lpszClass;
        char name[5];
        if (!HIWORD(cls)) {
            name[0] = 0;
            GlobalGetAtomNameA((ATOM)(UINT_PTR)cls, name, 5);
            cls = name;
        }
        if (!lstrcmpiA(cls, &g_imeClass)) goto done;
    }
    if (init) {
        Wnd_Attach(init, 0, h);
        CallVirt(init, 80);                         // PreSubclassWindow
        WNDPROC* oldProc = CallVirt<WNDPROC*>(init, 128);       // GetSuperWndProcAddr
        BYTE* ctl = (BYTE*)g_ctl3dState;
        typedef BOOL (WINAPI* Ctl3dSubclass)(HWND, DWORD);
        LRESULT ctl3d;
        if (!g_afxCtl3dOff && !dll && ctl && F(Ctl3dSubclass, ctl, 32)
            && (ctl3d = Afx_CallWndProc(init, h, 0x36F, 0, 0)) != 0) {    // WM_QUERY3DCONTROLS
            WNDPROC afxProc = Afx_GetAfxWndProc();
            BOOL subclassed = (WNDPROC)GetWindowLongA(h, GWL_WNDPROC) == afxProc;
            F(Ctl3dSubclass, ctl, 32)(h, (DWORD)ctl3d);
            if (!subclassed) *oldProc = (WNDPROC)SetWindowLongA(h, GWL_WNDPROC, (LONG)afxProc);
        } else {
            WNDPROC afxProc = Afx_GetAfxWndProc();
            WNDPROC prev = (WNDPROC)SetWindowLongA(h, GWL_WNDPROC, (LONG)afxProc);
            if (prev != afxProc) *oldProc = prev;
        }
        F(BYTE*, ts, 20) = 0;
    } else {
        LONG prev = GetWindowLongA(h, GWL_WNDPROC);
        if (prev && !GetPropA(h, &g_oldWndProcProp)) {
            SetPropA(h, &g_oldWndProcProp, (HANDLE)prev);
            if ((LONG)GetPropA(h, &g_oldWndProcProp) == prev) {
                GlobalAddAtomA(&g_oldWndProcProp);
                SetWindowLongA(h, GWL_WNDPROC, F(int, ts, 40) ? (LONG)$L_633fa2 : (LONG)Afx_ActivationWndProc);
            }
        }
    }
done:
    LRESULT r = CallNextHookEx(F(HHOOK, ts, 44), HCBT_CREATEWND, w, l);
    if (dll) {
        UnhookWindowsHookEx(F(HHOOK, ts, 44));
        F(HHOOK, ts, 44) = 0;
    }
    return r;
}

// [FUN_6353af] CWnd::SendMessageToDescendants
extern "C" void __stdcall Wnd_SendMessageToDescendants(HWND h, UINT msg, WPARAM w, LPARAM l, BOOL deep, BOOL onlyPerm) {
    for (HWND child = GetTopWindow(h); child; child = GetWindow(child, GW_HWNDNEXT)) {
        if (onlyPerm) {
            BYTE* wnd = Wnd_FromHandlePermanent(child);
            if (wnd) Afx_CallWndProc(wnd, HWND_OF(wnd), msg, w, l);
        } else {
            SendMessageA(child, msg, w, l);
        }
        if (deep && GetTopWindow(child)) Wnd_SendMessageToDescendants(child, msg, w, l, deep, onlyPerm);
    }
}

// [FUN_63562b] CWnd::WalkPreTranslateTree
extern "C" BOOL __stdcall Wnd_WalkPreTranslateTree(HWND stop, MSG* m) {
    for (HWND h = m->hwnd; h; h = GetParent(h)) {
        BYTE* wnd = Wnd_FromHandlePermanent(h);
        if (wnd && CallVirt<BOOL>(wnd, 144, m)) return TRUE;  // PreTranslateMessage
        if (h == stop) break;
    }
    return FALSE;
}

// ---------------------------------------------------------------------------
// CWinThread: the message pump and idle processing

// [$L_633121] CWinThread::GetMainWnd
extern "C" BYTE* __fastcall WinThread_GetMainWnd(BYTE* self) {
    if (F(BYTE*, self, 32)) return F(BYTE*, self, 32);     // m_pActiveWnd
    if (F(BYTE*, self, 28)) return F(BYTE*, self, 28);     // m_pMainWnd
    return Wnd_FromHandle(GetActiveWindow());
}

// [$L_63313c] CWinThread::PumpMessage
extern "C" BOOL __fastcall WinThread_PumpMessage(BYTE* self) {
    MSG* m = &F(MSG, self, 48);
    if (!GetMessageA(m, 0, 0, 0)) return FALSE;
    if (m->message != 0x36A && !CallVirt<BOOL>(self, 88, m)) {   // WM_KICKIDLE, PreTranslateMessage
        TranslateMessage(m);
        DispatchMessageA(m);
    }
    return TRUE;
}

// [FUN_632e37] CWinThread::DispatchThreadMessageEx: thread messages (no
// window) through the thread's message map
extern "C" BOOL __fastcall WinThread_DispatchThreadMessageEx(BYTE* self, void*, MSG* m) {
    for (const MsgMap* map = CallVirt<const MsgMap*>(self, 40); map; map = map->base) {
        const MsgMapEntry* e;
        if (m->message < 0xC000) {
            e = Afx_FindMessageEntry(map->entries, m->message, 0, 0);
            if (e) {
                CallThis(e->pfn, self, m->wParam, m->lParam);
                return TRUE;
            }
        } else {
            for (e = map->entries; (e = Afx_FindMessageEntry(e, 0xC000, 0, 0)) != 0; e++) {
                if (*(UINT*)e->sig == m->message) {
                    CallThis(e->pfn, self, m->wParam, m->lParam);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

// [$L_632eb2] CWinThread::PreTranslateMessage
extern "C" BOOL __fastcall WinThread_PreTranslateMessage(BYTE* self, void*, MSG* m) {
    if (!m->hwnd && WinThread_DispatchThreadMessageEx(self, 0, m)) return TRUE;
    BYTE* main = CallC<BYTE*>(FUN_61c4c1);
    if (Wnd_WalkPreTranslateTree(main ? HWND_OF(main) : 0, m)) return TRUE;
    if (main && Wnd_GetTopLevelParent(Wnd_FromHandle(m->hwnd)) != main)
        return CallVirt<BOOL>(main, 144, m);
    return FALSE;
}

// [$L_632ccf] CWinThread::IsIdleMessage: mouse moves that didn't move,
// paint and the caret blink don't restart idle processing
extern "C" BOOL __fastcall WinThread_IsIdleMessage(BYTE* self, void*, MSG* m) {
    if (m->message == WM_MOUSEMOVE || m->message == WM_NCMOUSEMOVE) {
        if (F(LONG, self, 92) == m->pt.x && F(LONG, self, 96) == m->pt.y && m->message == F(UINT, self, 100))
            return FALSE;
        F(LONG, self, 92) = m->pt.x;
        F(LONG, self, 96) = m->pt.y;
        F(UINT, self, 100) = m->message;
        return TRUE;
    }
    return m->message != WM_PAINT && m->message != 0x118;   // WM_SYSTIMER
}

// [$L_632c3e] CWinThread::Delete
extern "C" void __fastcall WinThread_Delete(BYTE* self) {
    if (F(BOOL, self, 36) && self) CallVirt(self, 4, 1);
}

// [FUN_632d2c] CWinThread::OnIdle: idle UI updates of the main and frame
// windows (count 0), then the temporary maps (count 1)
extern "C" BOOL __fastcall WinThread_OnIdle(BYTE* self, void*, LONG count) {
    const UINT IDLEUPDATE = 0x363;                  // WM_IDLEUPDATECMDUI
    if (count <= 0) {
        BYTE* main = F(BYTE*, self, 28);
        if (main && HWND_OF(main) && IsWindowVisible(HWND_OF(main))) {
            Afx_CallWndProc(main, HWND_OF(main), IDLEUPDATE, TRUE, 0);
            Wnd_SendMessageToDescendants(HWND_OF(main), IDLEUPDATE, TRUE, 0, TRUE, TRUE);
        }
        BYTE* mts = (BYTE*)ThreadLocal_GetData(&F(int, Afx_GetModuleState(), 4208), 0, (void* (__stdcall*)())ModuleThreadState_New);
        for (BYTE* frame = F(BYTE*, mts, 8); frame; frame = F(BYTE*, frame, 84)) {
            if (!HWND_OF(frame) || frame == main) continue;
            int& delay = F(int, frame, 136);        // m_nShowDelay
            if (delay == SW_HIDE) Wnd_ShowWindow(frame, 0, SW_HIDE);
            if (IsWindowVisible(HWND_OF(frame)) || delay >= 0) {
                Afx_CallWndProc(frame, HWND_OF(frame), IDLEUPDATE, TRUE, 0);
                Wnd_SendMessageToDescendants(HWND_OF(frame), IDLEUPDATE, TRUE, 0, TRUE, TRUE);
            }
            if (delay > 0) Wnd_ShowWindow(frame, 0, delay);
            delay = -1;
        }
    } else {
        BYTE* mts = (BYTE*)ThreadLocal_GetData(&F(int, Afx_GetModuleState(), 4208), 0, (void* (__stdcall*)())ModuleThreadState_New);
        if (F(int, mts, 16) == 0) {
            Afx_LockTempMaps();
            Afx_UnlockTempMaps(TRUE);
        }
    }
    return count < 0;
}

// ---------------------------------------------------------------------------
// Collections: CPlex, CPtrList, CMapPtrToPtr
//
// CMapPtrToPtr (28 bytes): +4 m_pHashTable, +8 m_nHashTableSize, +12 m_nCount,
//   +16 m_pFreeList, +20 m_pBlocks, +24 m_nBlockSize; associations
//   { pNext, key, value }, hashed by key >> 4.


KING_VAR(AsmLabel, g_ptrListVtbl, 6546c8)
KING_VAR(AsmLabel, g_ptrMapVtbl, 65461c)
KING_VAR(void*, g_afxNewHandler, 685e30)
KING_VAR(AsmLabel, g_winAppClass, 653418)

// [FUN_632348] CPlex::Create: a block of n elements, linked in front of head
extern "C" Plex* __stdcall Plex_Create(Plex** head, UINT n, UINT size) {
    Plex* p = (Plex*)operator new(n * size + sizeof(Plex));
    p->next = *head;
    *head = p;
    return p;
}

// [FUN_632368] CPlex::FreeDataChain
extern "C" void __fastcall Plex_FreeDataChain(Plex* self) {
    while (self) {
        Plex* next = self->next;
        operator delete(self);
        self = next;
    }
}

// [FUN_632241] CPtrList::RemoveAll
extern "C" void __fastcall PtrList_RemoveAll(BYTE* self) {
    F(int, self, 12) = 0;
    F(void*, self, 16) = 0;
    F(void*, self, 8) = 0;
    F(void*, self, 4) = 0;
    Plex_FreeDataChain(F(Plex*, self, 20));
    F(Plex*, self, 20) = 0;
}

// [FUN_632261] CPtrList::~CPtrList
extern "C" void __fastcall PtrList_Destruct(BYTE* self) {
    F(void*, self, 0) = g_ptrListVtbl;
    PtrList_RemoveAll(self);
}

// [FUN_63237e] CMapPtrToPtr::CMapPtrToPtr
extern "C" PtrMap* __fastcall PtrMap_Construct(PtrMap* self, void*, int blockSize) {
    self->table = 0;
    self->count = 0;
    self->freeList = 0;
    self->blocks = 0;
    self->vtbl = g_ptrMapVtbl;
    self->tableSize = 17;
    self->blockSize = blockSize;
    return self;
}

// [FUN_6323c1] CMapPtrToPtr::InitHashTable
extern "C" void __fastcall PtrMap_InitHashTable(PtrMap* self, void*, UINT size, BOOL alloc) {
    if (self->table) {
        operator delete(self->table);
        self->table = 0;
    }
    if (alloc) {
        self->table = (Assoc**)operator new(size * sizeof(Assoc*));
        memset(self->table, 0, size * sizeof(Assoc*));
    }
    self->tableSize = size;
}

// [FUN_632406] CMapPtrToPtr::RemoveAll
extern "C" void __fastcall PtrMap_RemoveAll(PtrMap* self) {
    if (self->table) {
        operator delete(self->table);
        self->table = 0;
    }
    self->count = 0;
    self->freeList = 0;
    Plex_FreeDataChain(self->blocks);
    self->blocks = 0;
}

// [FUN_632431] CMapPtrToPtr::~CMapPtrToPtr
extern "C" void __fastcall PtrMap_Destruct(PtrMap* self) {
    self->vtbl = g_ptrMapVtbl;
    PtrMap_RemoveAll(self);
}

// [FUN_63245a] CMapPtrToPtr::NewAssoc
extern "C" Assoc* __fastcall PtrMap_NewAssoc(PtrMap* self) {
    if (!self->freeList) {
        Plex* block = Plex_Create(&self->blocks, self->blockSize, sizeof(Assoc));
        Assoc* a = (Assoc*)(block + 1) + self->blockSize - 1;
        for (int i = self->blockSize; i > 0; i--, a--) {
            a->next = self->freeList;
            self->freeList = a;
        }
    }
    Assoc* a = self->freeList;
    self->freeList = a->next;
    self->count++;
    a->key = 0;
    a->value = 0;
    return a;
}

// [FUN_6324a7] CMapPtrToPtr::FreeAssoc
extern "C" void __fastcall PtrMap_FreeAssoc(PtrMap* self, void*, Assoc* a) {
    a->next = self->freeList;
    self->freeList = a;
    if (--self->count == 0) PtrMap_RemoveAll(self);
}

// [FUN_6324c0] CMapPtrToPtr::GetAssocAt
extern "C" Assoc* __fastcall PtrMap_GetAssocAt(PtrMap* self, void*, void* key, UINT* hash) {
    *hash = ((UINT)key >> 4) % self->tableSize;
    if (!self->table) return 0;
    for (Assoc* a = self->table[*hash]; a; a = a->next)
        if (a->key == key) return a;
    return 0;
}

// [FUN_6324f2] the value of key, or 0
extern "C" void* __fastcall PtrMap_GetValueAt(PtrMap* self, void*, void* key) {
    if (!self->table) return 0;
    for (Assoc* a = self->table[((UINT)key >> 4) % self->tableSize]; a; a = a->next)
        if (a->key == key) return a->value;
    return 0;
}

// [FUN_632525] CMapPtrToPtr::operator[]
extern "C" void** __fastcall PtrMap_At(PtrMap* self, void*, void* key) {
    UINT hash;
    Assoc* a = PtrMap_GetAssocAt(self, 0, key, &hash);
    if (!a) {
        if (!self->table) PtrMap_InitHashTable(self, 0, self->tableSize, TRUE);
        a = PtrMap_NewAssoc(self);
        a->key = key;
        a->next = self->table[hash];
        self->table[hash] = a;
    }
    return &a->value;
}

// [FUN_632575] CMapPtrToPtr::RemoveKey
extern "C" BOOL __fastcall PtrMap_RemoveKey(PtrMap* self, void*, void* key) {
    if (!self->table) return FALSE;
    Assoc** prev = &self->table[((UINT)key >> 4) % self->tableSize];
    for (Assoc* a = *prev; a; prev = &a->next, a = a->next) {
        if (a->key == key) {
            *prev = a->next;
            PtrMap_FreeAssoc(self, 0, a);
            return TRUE;
        }
    }
    return FALSE;
}

// [FUN_6325b7] CMapPtrToPtr::GetNextAssoc (pos -1: before the start)
extern "C" void __fastcall PtrMap_GetNextAssoc(PtrMap* self, void*, Assoc** pos, void** key, void** value) {
    Assoc* a = *pos;
    if (a == (Assoc*)-1) {
        for (UINT i = 0; i < self->tableSize; i++)
            if ((a = self->table[i]) != 0) break;
    }
    Assoc* next = a->next;
    if (!next) {
        for (UINT i = ((UINT)a->key >> 4) % self->tableSize + 1; i < self->tableSize; i++)
            if ((next = self->table[i]) != 0) break;
    }
    *pos = next;
    *key = a->key;
    *value = a->value;
}

// [FUN_632631] AfxSetNewHandler
extern "C" void* __stdcall Afx_SetNewHandler(void* handler) {
    void* old = g_afxNewHandler;
    g_afxNewHandler = handler;
    return old;
}

// [$L_632677] CWinApp::GetRuntimeClass
extern "C" void* __fastcall WinApp_GetRuntimeClass(BYTE*) { return g_winAppClass; }

// ---------------------------------------------------------------------------
// CHandleMap: the permanent and temporary C++ objects of HWNDs, HDCs, ...
//
// CHandleMap (72 bytes): +4 m_permanentMap, +32 m_temporaryMap,
//   +60 m_pClass, +64 m_nOffset (of the handles in the object), +68 m_nHandles.

KING_VAR(AsmLabel, g_handleMapVtbl, 6545ec)
KING_VAR(AsmLabel, g_tempDcClass, 6542c8)
KING_VAR(AsmLabel, g_tempGdiClass, 6542a0)

// [FUN_6366e5] CRuntimeClass::CreateObject
extern "C" void* __fastcall RuntimeClass_CreateObject(BYTE* self) {
    typedef void* (__stdcall* Create)();
    return F(Create, self, 12) ? F(Create, self, 12)() : 0;
}

// [FUN_6368e3] CHandleMap::CHandleMap
extern "C" BYTE* __fastcall HandleMap_Construct(BYTE* self, void*, BYTE* cls, int offset, int handles) {
    PtrMap_Construct((PtrMap*)(self + 4), 0, 10);
    PtrMap_Construct((PtrMap*)(self + 32), 0, 4);
    F(void*, self, 0) = g_handleMapVtbl;
    PtrMap_InitHashTable((PtrMap*)(self + 32), 0, 7, FALSE);
    F(BYTE*, self, 60) = cls;
    F(int, self, 64) = offset;
    F(int, self, 68) = handles;
    return self;
}

// [FUN_636948] CHandleMap::FromHandle: the permanent object, else a
// temporary one made for the handle
extern "C" BYTE* __fastcall HandleMap_FromHandle(BYTE* self, void*, HANDLE h) {
    if (!h) return 0;
    BYTE* obj = (BYTE*)PtrMap_GetValueAt((PtrMap*)(self + 4), 0, h);
    if (obj) return obj;
    obj = (BYTE*)PtrMap_GetValueAt((PtrMap*)(self + 32), 0, h);
    if (!obj) {
        void* old = Afx_SetNewHandler((void*)$L_6373bb);
        obj = (BYTE*)RuntimeClass_CreateObject(F(BYTE*, self, 60));
        if (!obj) ThrowMemory();
        *PtrMap_At((PtrMap*)(self + 32), 0, h) = obj;
        Afx_SetNewHandler(old);
    }
    HANDLE* handles = (HANDLE*)(obj + F(int, self, 64));
    handles[0] = h;
    if (F(int, self, 68) == 2) handles[1] = h;
    return obj;
}

// [FUN_636a07] CHandleMap::DeleteTemp
extern "C" void __fastcall HandleMap_DeleteTemp(BYTE* self) {
    if (!self) return;
    PtrMap* temp = (PtrMap*)(self + 32);
    Assoc* pos = temp->count ? (Assoc*)-1 : 0;
    while (pos) {
        void* h;
        BYTE* obj;
        PtrMap_GetNextAssoc(temp, 0, &pos, &h, (void**)&obj);
        HANDLE* handles = (HANDLE*)(obj + F(int, self, 64));
        handles[0] = 0;
        if (F(int, self, 68) == 2) handles[1] = 0;
        if (obj) CallVirt(obj, 4, 1);
    }
    PtrMap_RemoveAll(temp);
}

static BYTE* NewHandleMap(BYTE*& slot, BOOL create, void* cls, int offset, int handles) {
    if (!slot && create) {
        void* old = Afx_SetNewHandler((void*)$L_6373bb);
        BYTE* p = (BYTE*)operator new(72);
        slot = p ? HandleMap_Construct(p, 0, (BYTE*)cls, offset, handles) : 0;
        Afx_SetNewHandler(old);
    }
    return slot;
}

// [FUN_636c0a] afxMapHDC
extern "C" BYTE* __stdcall Afx_MapHDC(BOOL create) {
    return NewHandleMap(F(BYTE*, Afx_GetModuleThreadState(), 28), create, g_tempDcClass, 4, 2);
}

// [FUN_6370d1] afxMapHGDIOBJ
extern "C" BYTE* __stdcall Afx_MapHGDIOBJ(BOOL create) {
    return NewHandleMap(F(BYTE*, Afx_GetModuleThreadState(), 32), create, g_tempGdiClass, 4, 1);
}

// [FUN_636c7c] CDC::FromHandle
extern "C" BYTE* __stdcall Dc_FromHandle(HDC h) { return HandleMap_FromHandle(Afx_MapHDC(TRUE), 0, h); }

// ---------------------------------------------------------------------------
// CString, CDC, CGdiObject

KING_VAR(BYTE*, g_afxDataNil, 685fc4)
KING_VAR(AsmLabel, g_alloc64, 72db30)        // CFixedAlloc for strings up to 64,
KING_VAR(AsmLabel, g_alloc128, 72db08)       // 128, 256 and 512 characters
KING_VAR(AsmLabel, g_alloc256, 72dae0)
KING_VAR(AsmLabel, g_alloc512, 72dab8)
KING_VAR(AsmLabel, g_dcVtbl, 6543a8)
extern "C" void FUN_61cb1e(); KING_ALIAS("_FUN_61cb1e", "@FixedAlloc_Free@12")                           // CFixedAlloc::Free

// [FUN_636290] CString::FreeData (CStringData: nRefs, nDataLength, nAllocLength)
extern "C" void __fastcall String_FreeData(BYTE* data) {
    switch (F(int, data, 8)) {
    case 64:  CallThis(FUN_61cb1e, g_alloc64, data); break;
    case 128: CallThis(FUN_61cb1e, g_alloc128, data); break;
    case 256: CallThis(FUN_61cb1e, g_alloc256, data); break;
    case 512: CallThis(FUN_61cb1e, g_alloc512, data); break;
    default:  operator delete(data); break;
    }
}

// [FUN_6363a1] CString::~CString
extern "C" void __fastcall String_Destruct(char** self) {
    BYTE* data = (BYTE*)*self - 12;
    if (data != g_afxDataNil && InterlockedDecrement((LONG*)data) <= 0)
        String_FreeData((BYTE*)*self - 12);
}

// [FUN_636bda] CDC::CDC
extern "C" BYTE* __fastcall Cdc_Construct(BYTE* self) {
    F(void*, self, 0) = g_dcVtbl;
    F(HDC, self, 4) = 0;            // m_hDC
    F(HDC, self, 8) = 0;            // m_hAttribDC
    F(BOOL, self, 12) = 0;          // m_bPrinting
    return self;
}

// [FUN_636cf9] CDC::~CDC
extern "C" void __fastcall Cdc_Destruct(BYTE* self) {
    F(void*, self, 0) = g_dcVtbl;
    if (F(HDC, self, 4)) DeleteDC(Cdc_Detach(self));
}

// [FUN_637185] CGdiObject::Detach
extern "C" HGDIOBJ __fastcall GdiObject_Detach(BYTE* self) {
    HGDIOBJ h = F(HGDIOBJ, self, 4);
    if (h) {
        BYTE* map = Afx_MapHGDIOBJ(FALSE);
        if (map) PtrMap_RemoveKey((PtrMap*)(map + 4), 0, F(HGDIOBJ, self, 4));
    }
    F(HGDIOBJ, self, 4) = 0;
    return h;
}

// [FUN_6371af] CGdiObject::DeleteObject
extern "C" BOOL __fastcall GdiObject_DeleteObject(BYTE* self) {
    if (!F(HGDIOBJ, self, 4)) return FALSE;
    return DeleteObject(GdiObject_Detach(self));
}

// [FUN_6372f5] AfxDeleteObject
extern "C" void __stdcall Afx_DeleteObject(HGDIOBJ* p) {
    if (*p) {
        DeleteObject(*p);
        *p = 0;
    }
}

// ---------------------------------------------------------------------------
// CFrameWnd and CWinApp leftovers

KING_VAR(AsmLabel, g_frameWndVtbl, 654184)
KING_VAR(char, g_settingsSection, 653490)
KING_VAR(char, g_previewPagesEntry, 653480)
extern "C" void FUN_6371de(); KING_ALIAS("_FUN_6371de", "_Afx_IsComboBoxControl@8")                           // _AfxIsComboBoxControl
extern "C" void FUN_639958(); KING_ALIAS("_FUN_639958", "_Afx_OleSetUserCtrl@4")                           // AfxOleSetUserCtrl
extern "C" void FUN_638fc2(); KING_ALIAS("_FUN_638fc2", "@FrameWnd_SetMessageTextId@12")                           // CFrameWnd::OnUpdateFrameTitle part
extern "C" void FUN_6377ac(); KING_ALIAS("_FUN_6377ac", "@WinApp_WriteProfileInt@20")                           // CWinApp::WriteProfileInt
ASM_PROC(FUN_63b4a6)                           // a CNoTrackObject's destructor

// [FUN_63730e] AfxCancelModes: closes an open combo box drop-down
extern "C" void __stdcall Afx_CancelModes(HWND receiver) {
    HWND focus = GetFocus();
    if (!focus || focus == receiver) return;
    if (!CallStd<BOOL>(FUN_6371de, focus, 3)) {          // CBS_DROPDOWNLIST
        focus = GetParent(focus);
        if (focus == receiver || !CallStd<BOOL>(FUN_6371de, focus, 2)) return;   // CBS_DROPDOWN
    }
    if (receiver && (GetWindowLongA(receiver, GWL_STYLE) & WS_CHILD)) {
        receiver = GetParent(receiver);
        if (receiver == GetDesktopWindow()) return;
    }
    SendMessageA(focus, CB_SHOWDROPDOWN, FALSE, 0);
}

// [FUN_637a16] CFrameWnd::RemoveFrameWnd: from the thread's frame list
extern "C" void __fastcall FrameWnd_RemoveFrameWnd(BYTE* self) {
    BYTE* mts = (BYTE*)ThreadLocal_GetData(&F(int, Afx_GetModuleState(), 4208), 0, (void* (__stdcall*)())ModuleThreadState_New);
    SimpleList_Remove((SlotList*)(mts + 8), 0, self);
}

// [FUN_637972] CFrameWnd::~CFrameWnd
extern "C" void __fastcall FrameWnd_Destruct(BYTE* self) {
    F(void*, self, 0) = g_frameWndVtbl;
    FrameWnd_RemoveFrameWnd(self);
    BYTE* ts = Afx_GetThreadState();
    if (F(BYTE*, ts, 196) == self) F(BYTE*, Afx_GetThreadState(), 196) = 0;
    if (F(void*, self, 164)) operator delete(F(void*, self, 164));
    String_Destruct(&F(char*, self, 172));             // m_strTitle
    PtrList_Destruct(self + 108);
    Wnd_Destruct(self);
}

// [$L_637a60] CFrameWnd::GetHelpId-like: the active view's value, else +72
extern "C" DWORD __fastcall FrameWnd_GetActiveId(BYTE* self) {
    DWORD id = F(DWORD, self, 72);
    BYTE* view = CallVirt<BYTE*>(self, 188);
    if (view) {
        DWORD v = CallVirt<DWORD>(view, 168);
        if (v) id = v;
    }
    return id;
}

// [$L_637a84] CFrameWnd::PreTranslateMessage
extern "C" BOOL __fastcall FrameWnd_PreTranslateMessage(BYTE* self, void*, MSG* m) {
    if (m->message == WM_LBUTTONDOWN || m->message == WM_NCLBUTTONDOWN) Afx_CancelModes(m->hwnd);
    if (Wnd_ModuleHook(self, 0, m)) return TRUE;
    if (F(void*, self, 104) && CallVirt<BOOL>(F(void*, self, 104), 84, m)) return TRUE;
    if (m->message >= WM_KEYFIRST && m->message <= WM_KEYLAST) {
        HACCEL accel = CallVirt<HACCEL>(self, 232);        // GetDefaultAccelerator
        if (accel && TranslateAcceleratorA(HWND_OF(self), accel, m)) return TRUE;
    }
    return FALSE;
}

// [$L_637afb] CFrameWnd::PostNcDestroy: delete this
extern "C" void __fastcall FrameWnd_PostNcDestroy(BYTE* self) {
    if (self) CallVirt(self, 4, 1);
}

// [FUN_63740d] CWinApp::HideApplication
extern "C" void __fastcall WinApp_HideApplication(BYTE* self) {
    BYTE* main = F(BYTE*, self, 28);
    Wnd_ShowWindow(main, 0, SW_HIDE);
    ShowOwnedPopups(HWND_OF(F(BYTE*, self, 28)), FALSE);
    Wnd_SetWindowPos(F(BYTE*, self, 28), 0, (BYTE*)g_wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// [$L_63749e] CWinApp::SaveAllModified
extern "C" BOOL __fastcall WinApp_SaveAllModified(BYTE* self) {
    BYTE* docs = F(BYTE*, self, 128);
    return docs ? CallVirt<BOOL>(docs, 32) : TRUE;
}

// [FUN_6374d9] CWinApp::CloseAllDocuments
extern "C" void __fastcall WinApp_CloseAllDocuments(BYTE* self, void*, BOOL endSession) {
    BYTE* docs = F(BYTE*, self, 128);
    if (docs) CallVirt(docs, 36, endSession);
}

// [$L_6383b8] CFrameWnd::OnClose
extern "C" void __fastcall FrameWnd_OnClose(BYTE* self) {
    typedef BOOL (CALLBACK* CloseProc)(BYTE*);
    if (F(CloseProc, self, 156) && !F(CloseProc, self, 156)(self)) return;
    BYTE* doc = CallVirt<BYTE*>(self, 188);                 // GetActiveDocument
    if (doc && !CallVirt<BOOL>(doc, 140, self)) return;    // CanCloseFrame
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app && F(BYTE*, app, 28) == self) {
        if (!doc && !CallVirt<BOOL>(app, 136)) return;     // SaveAllModified
        WinApp_HideApplication(app);
        WinApp_CloseAllDocuments(app, 0, FALSE);
        if (!CallC<BOOL>(FUN_639948)) {
            CallStd(FUN_639958, FALSE);
            return;
        }
        if (!F(BYTE, Afx_GetModuleState(), 20) && !F(BYTE*, app, 28)) {
            Afx_PostQuitMessage(0);
            return;
        }
    }
    if (doc && F(BOOL, doc, 72)) {                          // m_bAutoDelete
        void* pos = CallVirt<void*>(doc, 96);               // GetFirstViewPosition
        BOOL other = FALSE;
        while (pos) {
            BYTE* view = CallVirt<BYTE*>(doc, 100, &pos);   // GetNextView
            if (Wnd_GetParentFrame(view) != self) {
                other = TRUE;
                break;
            }
        }
        if (!other) {
            CallVirt(doc, 124);                             // OnCloseDocument
            return;
        }
        CallVirt(doc, 148, self);                           // PreCloseFrame
    }
    CallVirt(self, 88);                                     // DestroyWindow
}

// [FUN_638a72] the frame's +152
extern "C" BYTE* __fastcall FrameWnd_Get152(BYTE* self) { return F(BYTE*, self, 152); }

// [$L_638a94] its +60
extern "C" DWORD __fastcall FrameWnd_Get152Field60(BYTE* self) {
    BYTE* p = FrameWnd_Get152(self);
    return p ? F(DWORD, p, 60) : 0;
}

// [$L_6395dd] CFrameWnd's delayed updates (m_nIdleFlags at +184)
extern "C" void __fastcall FrameWnd_OnIdleUpdate(BYTE* self) {
    if (F(BYTE, self, 184) & 1) CallVirt(self, 228, F(DWORD, self, 168));
    if (F(BYTE, self, 184) & 2) CallVirt(self, 224, 1);
    DWORD flags = F(DWORD, self, 184);
    if (flags & 8) {
        CallVirt(self, 200, flags & 4);
        UpdateWindow(HWND_OF(self));
    }
    if (F(DWORD, self, 144) != F(DWORD, self, 148)) CallThis(FUN_638fc2, self, F(DWORD, self, 144));
    F(DWORD, self, 184) = 0;
}

// [FUN_639c14] CWinApp::SaveStdProfileSettings
extern "C" void __fastcall WinApp_SaveStdProfileSettings(BYTE* self) {
    if (F(void*, self, 168)) CallVirt(F(void*, self, 168), 16);   // m_pRecentFileList->WriteList
    if (F(int, self, 180)) CallThis(FUN_6377ac, self, &g_settingsSection, &g_previewPagesEntry, F(int, self, 180));
}

// [$L_639c44] CWinApp::ExitInstance
extern "C" int __fastcall WinApp_ExitInstance(BYTE* self) {
    BYTE* cmd = F(BYTE*, self, 172);
    if (!(cmd && F(int, cmd, 16) == 5) && !F(BYTE, Afx_GetModuleState(), 20))
        WinApp_SaveStdProfileSettings(self);
    typedef void (__cdecl* TermFn)();
    if (F(TermFn, self, 188)) F(TermFn, self, 188)();
    return F(int, self, 56);                                // m_msgCur.wParam
}

// [$L_639979] a CNoTrackObject's deleting destructor
extern "C" BYTE* __fastcall NoTrack_DeletingDestruct(BYTE* self, void*, UINT flags) {
    CallThis(FUN_63b4a6, self);
    if (flags & 1) NoTrack_Delete(self);
    return self;
}

// [FUN_63274d] CWinApp::OnIdle: the thread's idle work, then the document
// templates' OnIdle
extern "C" BOOL __fastcall WinApp_OnIdle(BYTE* self, void*, LONG count) {
    if (count <= 0) {
        WinThread_OnIdle(self, 0, count);
        BYTE* docs = F(BYTE*, self, 128);
        if (docs) {
            void* pos = CallVirt<void*>(docs, 16);          // GetFirstDocTemplatePosition
            while (pos) CallVirt(CallVirt<BYTE*>(F(BYTE*, self, 128), 20, &pos), 136);
        }
    } else if (count == 1) {
        WinThread_OnIdle(self, 0, count);
    }
    return count < 1;
}

// ---------------------------------------------------------------------------
// CString buffers and LoadString

KING_VAR(char*, g_afxPchNil, 685fc0)          // the empty string's characters
extern "C" void FUN_61ca8f(); KING_ALIAS("_FUN_61ca8f", "@FixedAlloc_Alloc@4")                           // CFixedAlloc::Alloc
extern "C" void FUN_636471(); KING_ALIAS("_FUN_636471", "@String_GetBuffer@12")                           // CString::GetBuffer
extern "C" void FUN_6364c0(); KING_ALIAS("_FUN_6364c0", "@String_ReleaseBuffer@12")                           // CString::ReleaseBuffer

// [FUN_63620e] CString::AllocBuffer: CStringData { nRefs, nDataLength,
// nAllocLength } followed by the characters
extern "C" void __fastcall String_AllocBuffer(char** self, void*, int len) {
    if (len == 0) {
        *self = g_afxPchNil;
        return;
    }
    BYTE* data;
    if (len <= 64) { data = CallThis<BYTE*>(FUN_61ca8f, g_alloc64); F(int, data, 8) = 64; }
    else if (len <= 128) { data = CallThis<BYTE*>(FUN_61ca8f, g_alloc128); F(int, data, 8) = 128; }
    else if (len <= 256) { data = CallThis<BYTE*>(FUN_61ca8f, g_alloc256); F(int, data, 8) = 256; }
    else if (len <= 512) { data = CallThis<BYTE*>(FUN_61ca8f, g_alloc512); F(int, data, 8) = 512; }
    else { data = (BYTE*)operator new(len + 13); F(int, data, 8) = len; }
    F(int, data, 0) = 1;
    data[12 + len] = 0;
    F(int, data, 4) = len;
    *self = (char*)data + 12;
}

// [FUN_6362d8] CString::Release
extern "C" void __fastcall String_Release(char** self) {
    BYTE* data = (BYTE*)*self - 12;
    if (data == g_afxDataNil) return;
    if (InterlockedDecrement((LONG*)data) <= 0) String_FreeData((BYTE*)*self - 12);
    *self = g_afxPchNil;
}

// [FUN_636378] CString::AllocBeforeWrite
extern "C" void __fastcall String_AllocBeforeWrite(char** self, void*, int len) {
    BYTE* data = (BYTE*)*self - 12;
    if (F(int, data, 0) > 1 || len > F(int, data, 8)) {
        String_Release(self);
        String_AllocBuffer(self, 0, len);
    }
}

// [FUN_63641d] CString::AssignCopy
extern "C" void __fastcall String_AssignCopy(char** self, void*, int len, const char* src) {
    String_AllocBeforeWrite(self, 0, len);
    memcpy(*self, src, len);
    F(int, *self, -8) = len;
    (*self)[len] = 0;
}

// [FUN_63644a] CString::operator=(LPCTSTR)
extern "C" char** __fastcall String_Assign(char** self, void*, const char* s) {
    String_AssignCopy(self, 0, s ? lstrlenA(s) : 0, s);
    return self;
}

// [FUN_6365c1] AfxLoadString
extern "C" int __stdcall Afx_LoadString(UINT id, char* buf, UINT max) {
    int n = LoadStringA(F(HINSTANCE, Afx_GetModuleState(), 12), id, buf, max);
    if (!n) *buf = 0;
    return n;
}

// [FUN_63653d] CString::LoadString
extern "C" BOOL __fastcall String_LoadString(char** self, void*, UINT id) {
    char buf[256];
    int n = Afx_LoadString(id, buf, 256);
    if ((UINT)(256 - n) > 2) {
        String_Assign(self, 0, buf);
        return n > 0;
    }
    int size = 256;
    do {
        size += 256;
        n = Afx_LoadString(id, CallThis<char*>(FUN_636471, self, size - 1), size);
    } while (size - n <= 2);
    CallThis(FUN_6364c0, self, -1);
    return n > 0;
}

// ---------------------------------------------------------------------------
// CDC and CGdiObject handles

// [FUN_636c91] CDC::Attach
extern "C" BOOL __fastcall Cdc_Attach(BYTE* self, void*, HDC h) {
    if (!h) return FALSE;
    BYTE* map = Afx_MapHDC(TRUE);
    F(HDC, self, 4) = h;
    *(BYTE**)PtrMap_At((PtrMap*)(map + 4), 0, h) = self;
    CallVirt(self, 12, F(HDC, self, 4));                    // SetAttribDC
    return TRUE;
}

// [FUN_636cc8] CDC::Detach
extern "C" HDC __fastcall Cdc_Detach(BYTE* self) {
    HDC h = F(HDC, self, 4);
    if (h) {
        BYTE* map = Afx_MapHDC(FALSE);
        if (map) PtrMap_RemoveKey((PtrMap*)(map + 4), 0, F(HDC, self, 4));
    }
    CallVirt(self, 20);                                     // ReleaseAttribDC
    F(HDC, self, 4) = 0;
    return h;
}

// [FUN_636eb3] CDC::SetMapMode
extern "C" int __fastcall Cdc_SetMapMode(BYTE* self, void*, int mode) {
    int r = 0;
    if (F(HDC, self, 4) != F(HDC, self, 8)) r = SetMapMode(F(HDC, self, 4), mode);
    if (F(HDC, self, 8)) r = SetMapMode(F(HDC, self, 8), mode);
    return r;
}

// [FUN_637158] CGdiObject::Attach
extern "C" BOOL __fastcall GdiObject_Attach(BYTE* self, void*, HGDIOBJ h) {
    if (!h) return FALSE;
    BYTE* map = Afx_MapHGDIOBJ(TRUE);
    F(HGDIOBJ, self, 4) = h;
    *(BYTE**)PtrMap_At((PtrMap*)(map + 4), 0, h) = self;
    return TRUE;
}

// [FUN_637143] CGdiObject::FromHandle
extern "C" BYTE* __stdcall GdiObject_FromHandle(HGDIOBJ h) {
    return HandleMap_FromHandle(Afx_MapHGDIOBJ(TRUE), 0, h);
}

// [FUN_636dbc] CDC::SelectGdiObject
extern "C" BYTE* __stdcall Cdc_SelectGdiObject(HDC dc, HGDIOBJ h) {
    return GdiObject_FromHandle(SelectObject(dc, h));
}

// ---------------------------------------------------------------------------
// Window classes and window creation

KING_VAR(HCURSOR, g_afxArrowCursor, 72e0b8)
KING_VAR(char, g_classWnd, 653a0c)           // "AfxWnd42s"
KING_VAR(char, g_classOleControl, 653a50)    // "AfxOleControl42s"
KING_VAR(char, g_classControlBar, 653a18)    // "AfxControlBar42s"
KING_VAR(char, g_classMDIFrame, 653a2c)      // "AfxMDIFrame42s"
KING_VAR(char, g_classFrameOrView, 653a3c)   // "AfxFrameOrView42s"
extern "C" void FUN_635dea(); KING_ALIAS("_FUN_635dea", "_Afx_InitCommonControls@8")                           // _AfxInitCommonControls

// [FUN_63489e] AfxRegisterClass
extern "C" BOOL __stdcall Afx_RegisterClass(WNDCLASSA* wc) {
    WNDCLASSA tmp;
    if (GetClassInfoA(wc->hInstance, wc->lpszClassName, &tmp)) return TRUE;
    if (!RegisterClassA(wc)) return FALSE;
    if (F(BYTE, Afx_GetModuleState(), 20)) {
        Afx_LockGlobals(1);
        char* list = (char*)Afx_GetModuleState() + 52;
        lstrcatA(list, wc->lpszClassName);
        char nl[2] = { '\n', 0 };
        lstrcatA(list, nl);
        Afx_UnlockGlobals(1);
    }
    return TRUE;
}

// [FUN_635da9] _AfxRegisterWithIcon
extern "C" BOOL __stdcall Afx_RegisterWithIcon(WNDCLASSA* wc, const char* name, UINT icon) {
    wc->lpszClassName = name;
    wc->hIcon = LoadIconA(F(HINSTANCE, Afx_GetModuleState(), 12), MAKEINTRESOURCEA(icon));
    if (!wc->hIcon) wc->hIcon = LoadIconA(0, IDI_APPLICATION);
    return Afx_RegisterClass(wc);
}

// [FUN_635e61] AfxEndDeferRegisterClass: registers MFC's window classes
// (and common controls) on first use
extern "C" BOOL __stdcall Afx_EndDeferRegisterClass(DWORD toRegister) {
    BYTE* ms = Afx_GetModuleState();
    toRegister &= ~F(DWORD, ms, 24);
    if (!toRegister) return TRUE;
    WNDCLASSA wc;
    memset(&wc, 0, sizeof wc);
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = F(HINSTANCE, Afx_GetModuleState(), 8);
    wc.hCursor = g_afxArrowCursor;
    struct { DWORD size, icc; } init = { 8, 0 };   // INITCOMMONCONTROLSEX
    DWORD done = 0;
    if (toRegister & 1) {
        wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wc.lpszClassName = &g_classWnd;
        if (Afx_RegisterClass(&wc)) done = 1;
    }
    if (toRegister & 32) {
        wc.style |= 0x8B;
        wc.lpszClassName = &g_classOleControl;
        if (Afx_RegisterClass(&wc)) done |= 32;
    }
    if (toRegister & 2) {
        wc.style = 0;
        wc.lpszClassName = &g_classControlBar;
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        if (Afx_RegisterClass(&wc)) done |= 2;
    }
    if (toRegister & 4) {
        wc.style = CS_DBLCLKS;
        wc.hbrBackground = 0;
        if (Afx_RegisterWithIcon(&wc, &g_classMDIFrame, 31233)) done |= 4;
    }
    if (toRegister & 8) {
        wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        if (Afx_RegisterWithIcon(&wc, &g_classFrameOrView, 31234)) done |= 8;
    }
    if (toRegister & 16) {
        init.icc = 255;
        done |= CallStd<DWORD>(FUN_635dea, &init, 0x3FC0);
        toRegister = (toRegister & 0xFFFF0000) | (toRegister & 0xC03F);
    }
    // the common control groups: flag -> ICC_ class bits
    static const DWORD groups[][2] = {
        { 0x40, 0x10 }, { 0x80, 0x2 }, { 0x100, 0x8 }, { 0x200, 0x20 }, { 0x400, 0x1 },
        { 0x800, 0x40 }, { 0x1000, 0x4 }, { 0x2000, 0x80 }, { 0x4000, 0x800 },
        { 0x8000, 0x400 }, { 0x10000, 0x200 }, { 0x20000, 0x100 },
    };
    for (auto& g : groups) {
        if (toRegister & g[0]) {
            init.icc = g[1];
            done |= CallStd<DWORD>(FUN_635dea, &init, g[0]);
        }
    }
    F(DWORD, ms, 24) |= done;
    if ((F(DWORD, ms, 24) & 0x3FC0) == 0x3FC0) {
        F(DWORD, ms, 24) |= 16;
        done |= 16;
    }
    return (done & toRegister) == toRegister;
}

// [FUN_634214] AfxHookWindowCreate: the CBT hook gives the next window to pWnd
extern "C" void __stdcall Afx_HookWindowCreate(BYTE* wnd) {
    BYTE* ts = Afx_GetThreadState();
    if (F(BYTE*, ts, 20) == wnd) return;
    if (!F(HHOOK, ts, 44)) {
        F(HHOOK, ts, 44) = SetWindowsHookExA(WH_CBT, Afx_CbtFilterHook, 0, GetCurrentThreadId());
        if (!F(HHOOK, ts, 44)) ThrowMemory();
    }
    F(BYTE*, ts, 20) = wnd;
}

// [FUN_634260] AfxUnhookWindowCreate: FALSE if the window was never hooked
extern "C" BOOL __cdecl Afx_UnhookWindowCreate() {
    BYTE* ts = Afx_GetThreadState();
    if (F(BYTE, Afx_GetModuleState(), 20) && F(HHOOK, ts, 44)) {
        UnhookWindowsHookEx(F(HHOOK, ts, 44));
        F(HHOOK, ts, 44) = 0;
    }
    if (F(BYTE*, ts, 20)) {
        F(BYTE*, ts, 20) = 0;
        return FALSE;
    }
    return TRUE;
}

// [FUN_6342a2] CWnd::CreateEx
extern "C" BOOL __fastcall Wnd_CreateEx(BYTE* self, void*, DWORD exStyle, const char* cls, const char* name, DWORD style,
                                        int x, int y, int cx, int cy, HWND parent, HMENU menu, void* param) {
    CREATESTRUCTA cs;
    cs.dwExStyle = exStyle;
    cs.lpszClass = cls;
    cs.lpszName = name;
    cs.style = style;
    cs.x = x;
    cs.y = y;
    cs.cx = cx;
    cs.cy = cy;
    cs.hwndParent = parent;
    cs.hMenu = menu;
    cs.hInstance = F(HINSTANCE, Afx_GetModuleState(), 8);
    cs.lpCreateParams = param;
    if (!CallVirt<BOOL>(self, 92, &cs)) {                  // PreCreateWindow
        CallVirt(self, 164);                                // PostNcDestroy
        return FALSE;
    }
    Afx_HookWindowCreate(self);
    HWND h = CreateWindowExA(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy,
                             cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);
    if (!Afx_UnhookWindowCreate()) CallVirt(self, 164);
    return h != 0;
}

// [FUN_633999] CWnd::CWnd(HWND)
extern "C" BYTE* __fastcall Wnd_ConstructHwnd(BYTE* self, void*, HWND h) {
    CmdTarget_Construct(self);
    F(void*, self, 0) = g_wndVtbl;
    memset(self + 28, 0, 32);
    HWND_OF(self) = h;
    return self;
}

// [FUN_6336ab] CWnd::GetStyle
extern "C" DWORD __fastcall Wnd_GetStyle(BYTE* self) {
    if (F(void*, self, 56)) return CallVirt<DWORD>(F(void*, self, 56), 112);
    return GetWindowLongA(HWND_OF(self), GWL_STYLE);
}

// [FUN_6337ad] CWnd::IsWindowEnabled
extern "C" BOOL __fastcall Wnd_IsWindowEnabled(BYTE* self) {
    if (F(void*, self, 56)) return CallVirt<BOOL>(F(void*, self, 56), 164);
    return IsWindowEnabled(HWND_OF(self));
}

// [FUN_63524a] CWnd::GetParentFrame
extern "C" BYTE* __fastcall Wnd_GetParentFrame(BYTE* self) {
    if (!self || !HWND_OF(self)) return 0;
    HWND h = HWND_OF(self);
    for (;;) {
        BYTE* w = Wnd_FromHandle(GetParent(h));
        if (!w) return 0;
        if (CallVirt<BOOL>(w, 176)) return w;              // IsFrameWnd
        h = HWND_OF(w);
    }
}

// [FUN_6352f5] CWnd::GetTopLevelFrame
extern "C" BYTE* __fastcall Wnd_GetTopLevelFrame(BYTE* self) {
    if (!self || !HWND_OF(self)) return 0;
    BYTE* frame = CallVirt<BOOL>(self, 176) ? self : Wnd_GetParentFrame(self);
    if (frame)
        for (BYTE* p; (p = Wnd_GetParentFrame(frame)) != 0; ) frame = p;
    return frame;
}

// [FUN_633aca] _AfxHandleActivate: tells the top level parent when
// activation moves to another top level window
extern "C" void __stdcall Afx_HandleActivate(BYTE* wnd, WPARAM state, BYTE* other) {
    if (Wnd_GetStyle(wnd) & WS_CHILD) return;
    BYTE* top = Wnd_GetTopLevelParent(wnd);
    if (top == Wnd_GetTopLevelParent(other)) return;
    HWND pair[2] = { HWND_OF(wnd), other ? HWND_OF(other) : 0 };
    SendMessageA(HWND_OF(top), 0x36E, state, (LPARAM)pair);  // WM_ACTIVATETOPLEVEL
}

// [FUN_637cbe] _AfxIsDescendant (through parents and owners)
extern "C" BOOL __stdcall Afx_IsDescendant(HWND parent, HWND child) {
    do {
        if (parent == child) return TRUE;
        child = Afx_GetParentOwner(child);
    } while (child);
    return FALSE;
}

// [FUN_634649] cancels tool tips and the status bar's menu help
extern "C" void __stdcall Afx_CancelToolTips(BOOL keys) {
    BYTE* ts = Afx_GetThreadState();
    BYTE* tip = F(BYTE*, ts, 204);
    if (tip && HWND_OF(tip)) SendMessageA(HWND_OF(tip), 1025, 0, 0);     // TTM_ACTIVATE
    BYTE* status = F(BYTE*, ts, 264);
    if (keys && status && GetKeyState(VK_LBUTTON) >= 0) CallVirt(status, 220, -1);
}

// ---------------------------------------------------------------------------
// CFrameWnd creation

extern "C" void FUN_632202(); KING_ALIAS("_FUN_632202", "@PtrList_Construct@12")                           // CPtrList::CPtrList

// [FUN_6379f2] CFrameWnd::AddFrameWnd
extern "C" void __fastcall FrameWnd_AddFrameWnd(BYTE* self) {
    BYTE* mts = (BYTE*)ThreadLocal_GetData(&F(int, Afx_GetModuleState(), 4208), 0, (void* (__stdcall*)())ModuleThreadState_New);
    SimpleList_AddHead((SlotList*)(mts + 8), 0, self);
}

// [FUN_637894] CFrameWnd::CFrameWnd
extern "C" BYTE* __fastcall FrameWnd_Construct(BYTE* self) {
    Wnd_Construct(self);
    CallThis(FUN_632202, self + 108, 10);                   // m_listControlBars
    F(char*, self, 172) = g_afxPchNil;                      // m_strTitle
    F(int, self, 64) = -1;
    F(void*, self, 0) = g_frameWndVtbl;
    F(int, self, 60) = 1;
    static const int zero[] = { 156, 68, 72, 140, 144, 148, 152, 160, 164, 104, 168, 184 };
    for (int off : zero) F(int, self, off) = 0;
    SetRectEmpty(&F(RECT, self, 88));
    F(int, self, 136) = -1;                                 // m_nShowDelay
    F(int, self, 80) = 0;
    F(int, self, 76) = 0;
    F(int, self, 84) = 0;
    F(int, self, 176) = 0;
    F(int, self, 180) = 0;
    FrameWnd_AddFrameWnd(self);
    return self;
}

// [FUN_6380a5] CFrameWnd::Create
extern "C" BOOL __fastcall FrameWnd_Create(BYTE* self, void*, const char* cls, const char* name, DWORD style,
                                           const RECT* rect, BYTE* parent, const char* menuName, DWORD exStyle, void* context) {
    HMENU menu = 0;
    if (menuName) {
        menu = LoadMenuA(F(HINSTANCE, Afx_GetModuleState(), 12), menuName);
        if (!menu) {
            CallVirt(self, 164);                            // PostNcDestroy
            return FALSE;
        }
    }
    String_Assign(&F(char*, self, 172), 0, name);
    HWND hParent = parent ? HWND_OF(parent) : 0;
    if (!Wnd_CreateEx(self, 0, exStyle, cls, name, style, rect->left, rect->top,
                      rect->right - rect->left, rect->bottom - rect->top, hParent, menu, context)) {
        if (menu) DestroyMenu(menu);
        return FALSE;
    }
    return TRUE;
}

// [FUN_6381e6] CFrameWnd::OnCreate
extern "C" int __fastcall FrameWnd_OnCreate(BYTE* self, void*, void* cs, void* context) {
    if (Wnd_Default(self) == -1 || !CallVirt<BOOL>(self, 220, cs, context)) return -1;   // OnCreateHelper
    PostMessageA(HWND_OF(self), 0x362, 0xE001, 0);         // WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE
    CallVirt(self, 200, 1);                                 // RecalcLayout
    return 0;
}

// [FUN_637fb7] CWnd::NotifyFloatingWindows
extern "C" void __fastcall Wnd_NotifyFloatingWindows(BYTE* self, void*, DWORD flags) {
    BYTE* parent = (Wnd_GetStyle(self) & WS_CHILD) ? self : Wnd_GetTopLevelFrame(self);
    if (flags & 12) {                                       // FS_ACTIVATE | FS_DEACTIVATE
        BOOL enabled = Wnd_IsWindowEnabled(parent);
        if (!(flags & 8) && enabled && parent != self) {
            F(BYTE, self, 37) |= 2;
            SendMessageA(HWND_OF(parent), WM_NCACTIVATE, TRUE, 0);
            F(BYTE, self, 37) &= ~2;
        } else {
            SendMessageA(HWND_OF(parent), WM_NCACTIVATE, FALSE, 0);
        }
    }
    for (HWND h = GetWindow(GetDesktopWindow(), GW_CHILD); h; h = GetWindow(h, GW_HWNDNEXT))
        if (Afx_IsDescendant(HWND_OF(parent), h)) SendMessageA(h, 0x36D, flags, 0);    // WM_FLOATSTATUS
}

// ---------------------------------------------------------------------------
// CTL3D and the dialog colours

KING_VAR(char, g_ctl3dDll, 6544e4)           // "CTL3D32.DLL"
KING_VAR(void*, g_winState, 72c15c)          // CProcessLocal<_AFX_WIN_STATE>
extern "C" void $L_63a003(); KING_ALIAS("_$L_63a003", "_WinState_New")                            // new _AFX_WIN_STATE
ASM_PROC(FUN_6371c5)                           // AfxThrowResourceException

// [FUN_63a662] CWinApp::Enable3dControls
extern "C" BOOL __fastcall WinApp_Enable3dControls(BYTE* self) {
    if (g_afxCtl3dOff) return TRUE;
    BYTE* ctl = (BYTE*)ProcessLocal_GetData(&g_ctl3dState, 0, (void* (__stdcall*)())$L_639de0);
    if (!F(BOOL, ctl, 4)) {
        HMODULE lib = LoadLibraryA(&g_ctl3dDll);
        F(HMODULE, ctl, 8) = lib;
        if (lib) {
            static const int ordinals[] = { 12, 13, 16, 24, 6, 21, 22, 3, 25 };
            for (int i = 0; i < 9; i++)
                F(FARPROC, ctl, 12 + 4 * i) = GetProcAddress(F(HMODULE, ctl, 8), MAKEINTRESOURCEA(ordinals[i]));
        }
        typedef BOOL (WINAPI* Ctl3dFn)(HINSTANCE);
        if (!F(void*, ctl, 12) || !F(void*, ctl, 20) || !F(void*, ctl, 28) || !F(void*, ctl, 32) || !F(void*, ctl, 16)
            || !F(Ctl3dFn, ctl, 12)(F(HINSTANCE, Afx_GetModuleState(), 8))) {
            HMODULE h = F(HMODULE, ctl, 8);
            for (int off = 12; off <= 44; off += 4) F(void*, ctl, off) = 0;
            if (h) {
                FreeLibrary(h);
                F(HMODULE, ctl, 8) = 0;
            }
        }
        F(BOOL, ctl, 4) = TRUE;
    }
    if (!F(HMODULE, ctl, 8)) return FALSE;
    typedef BOOL (WINAPI* Ctl3dFn)(HINSTANCE);
    return F(Ctl3dFn, ctl, 20)(F(HINSTANCE, Afx_GetModuleState(), 8));     // Ctl3dAutoSubclass
}

// [FUN_63a7bb] CWinApp::SetDialogBkColor
extern "C" void __fastcall WinApp_SetDialogBkColor(BYTE* self, void*, COLORREF bk, COLORREF text) {
    if (!F(BYTE, Afx_GetModuleState(), 20)) {
        BYTE* ts = Afx_GetThreadState();
        if (!F(HHOOK, ts, 44)) {
            F(HHOOK, ts, 44) = SetWindowsHookExA(WH_CBT, Afx_CbtFilterHook, 0, GetCurrentThreadId());
            if (!F(HHOOK, ts, 44)) ThrowMemory();
        }
    }
    BYTE* ws = (BYTE*)ProcessLocal_GetData(&g_winState, 0, (void* (__stdcall*)())$L_63a003);
    Afx_DeleteObject(&F(HGDIOBJ, ws, 4));
    F(HBRUSH, ws, 4) = CreateSolidBrush(bk);
    F(COLORREF, ws, 8) = text;
    if (!F(HBRUSH, ws, 4)) CallC(FUN_6371c5);
}

// ---------------------------------------------------------------------------
// CCmdUI, more CWnd wrappers and the static wndTop/wndBottom/wndTopMost/
// wndNoTopMost objects

extern "C" int __cdecl atexit(void (__cdecl* fn)());
KING_VAR(AsmLabel, g_cmdUIVtbl, 6536f8)
KING_VAR(HBITMAP, g_hbmMenuDot, 72e0c8)       // afxData.hbmMenuDot
KING_VAR(AsmLabel, g_dotPattern, 653624)      // 5 rows of the radio dot
KING_VAR(BYTE, g_wndStaticsDone, 72da54)      // destroyed-flags of the four
KING_VAR(UINT, g_msgDragList, 72da94)
KING_VAR(AsmLabel, g_dragListMsgName, 653ae4)  // "commctrl_DragListMsg"
KING_VAR(AsmLabel, g_cmdTargetMap6536b8, 6536b8)

// [$L_633421] CCmdTarget map getter ($L_6536b8)
extern "C" void* __fastcall CmdTarget_GetMap6536b8(BYTE* self) { return g_cmdTargetMap6536b8; }

// [$L_633459] returns TRUE
extern "C" BOOL __fastcall CmdTarget_True(BYTE* self) { return TRUE; }

KING_VAR(AsmLabel, g_cmdTargetMessageMap, 6536d0)
KING_VAR(AsmLabel, g_cmdTargetInterfaceMap, 6536e0)

// [$L_63345d] CCmdTarget::GetMessageMap
extern "C" void* __fastcall CmdTarget_GetMessageMap(BYTE* self) { return g_cmdTargetMessageMap; }

// [$L_633463] returns FALSE (one argument)
extern "C" BOOL __fastcall CmdTarget_False1(BYTE* self, void*, int) { return FALSE; }

// [$L_633468] CCmdTarget::GetInterfaceMap
extern "C" void* __fastcall CmdTarget_GetInterfaceMap(BYTE* self) { return g_cmdTargetInterfaceMap; }

// [FUN_63a37f] CTypeLibCache::Unlock: releases the type info (+28) and the
// type library (+8) with the last lock (+32)
extern "C" void __fastcall TypeLibCache_Unlock(BYTE* self) {
    if (InterlockedDecrement(&F(LONG, self, 32))) return;
    if (F(void*, self, 28)) {
        ((ULONG (__stdcall*)(void*))(*F(void***, self, 28))[2])(F(void*, self, 28));     // Release
        F(void*, self, 28) = 0;
    }
    if (F(void*, self, 8)) {
        ((ULONG (__stdcall*)(void*))(*F(void***, self, 8))[2])(F(void*, self, 8));
        F(void*, self, 8) = 0;
    }
}

// [$L_633427] CCmdTarget::OnFinalRelease
extern "C" void __fastcall CmdTarget_OnFinalRelease(BYTE* self) {
    Afx_LockGlobals(13);
    BYTE* cache = CallVirt<BYTE*>(self, 32);                       // GetTypeLibCache
    if (cache) TypeLibCache_Unlock(cache);
    Afx_UnlockGlobals(13);
    if (self) CallVirt(self, 4, 1);
}

// CCmdUI: +4 m_nID, +8 m_nIndex, +12 m_pMenu, +16 m_pSubMenu, +20 m_pOther,
// +24 m_bEnableChanged, +28 m_bContinueRouting, +32 m_nIndexMax,
// +36 m_pParentMenu

// [FUN_63346e] CCmdUI::CCmdUI
extern "C" BYTE* __fastcall CmdUI_Construct(BYTE* self) {
    F(void*, self, 0) = g_cmdUIVtbl;
    F(int, self, 32) = 0;
    F(int, self, 8) = 0;
    F(int, self, 4) = 0;
    F(int, self, 36) = 0;
    F(int, self, 16) = 0;
    F(int, self, 12) = 0;
    F(int, self, 20) = 0;
    F(int, self, 28) = 0;
    F(int, self, 24) = 0;
    return self;
}

extern "C" BYTE* __fastcall Wnd_SetFocus(BYTE* self);
extern "C" void __fastcall Wnd_EnableWindow(BYTE* self, void*, BOOL enable);
extern "C" void __stdcall Afx_SetWindowText(HWND h, const char* text);
extern "C" void __cdecl Afx_LoadDotBitmap();

// [$L_633494] CCmdUI::Enable
extern "C" void __fastcall CmdUI_Enable(BYTE* self, void*, BOOL on) {
    BYTE* menu = F(BYTE*, self, 12);
    if (menu) {
        if (F(void*, self, 16)) return;
        EnableMenuItem(F(HMENU, menu, 4), F(UINT, self, 8), MF_BYPOSITION | (on ? MF_ENABLED : MF_DISABLED | MF_GRAYED));
    } else {
        if (!on) {
            BYTE* other = F(BYTE*, self, 20);
            if (GetFocus() == HWND_OF(other)) {
                BYTE* parent = Wnd_FromHandle(GetParent(HWND_OF(other)));
                BYTE* o = F(BYTE*, self, 20);
                Wnd_SetFocus(Wnd_FromHandle(GetNextDlgTabItem(HWND_OF(parent), o ? HWND_OF(o) : 0, FALSE)));
            }
        }
        Wnd_EnableWindow(F(BYTE*, self, 20), 0, on);
    }
    F(BOOL, self, 24) = TRUE;
}

// [$L_633524] CCmdUI::SetCheck
extern "C" void __fastcall CmdUI_SetCheck(BYTE* self, void*, int check) {
    BYTE* menu = F(BYTE*, self, 12);
    if (menu) {
        if (!F(void*, self, 16))
            CheckMenuItem(F(HMENU, menu, 4), F(UINT, self, 8), MF_BYPOSITION | (check ? MF_CHECKED : MF_UNCHECKED));
        return;
    }
    HWND h = HWND_OF(F(BYTE*, self, 20));
    if (SendMessageA(h, WM_GETDLGCODE, 0, 0) & DLGC_BUTTON)
        SendMessageA(HWND_OF(F(BYTE*, self, 20)), BM_SETCHECK, check, 0);
}

// [$L_633586] CCmdUI::SetRadio
extern "C" void __fastcall CmdUI_SetRadio(BYTE* self, void*, BOOL on) {
    CallVirt(self, 4, on != 0);                                    // SetCheck
    if (!F(BYTE*, self, 12) || F(void*, self, 16)) return;
    if (!g_hbmMenuDot) {
        Afx_LoadDotBitmap();
        if (!g_hbmMenuDot) return;
    }
    SetMenuItemBitmaps(F(HMENU, F(BYTE*, self, 12), 4), F(UINT, self, 8), MF_BYPOSITION, 0, g_hbmMenuDot);
}

// [$L_6335dc] CCmdUI::SetText
extern "C" void __fastcall CmdUI_SetText(BYTE* self, void*, const char* text) {
    BYTE* menu = F(BYTE*, self, 12);
    if (!menu) {
        Afx_SetWindowText(HWND_OF(F(BYTE*, self, 20)), text);
        return;
    }
    if (F(void*, self, 16)) return;
    UINT state = GetMenuState(F(HMENU, menu, 4), F(UINT, self, 8), MF_BYPOSITION);
    ModifyMenuA(F(HMENU, F(BYTE*, self, 12), 4), F(UINT, self, 8), (state & 0xFFFFF6FB) | MF_BYPOSITION,
                F(UINT, self, 4), text);
}

// [FUN_633633] CCmdUI::DoUpdate(pTarget, bDisableIfNoHandler)
extern "C" BOOL __fastcall CmdUI_DoUpdate(BYTE* self, void*, BYTE* target, BOOL disableIfNoHandler) {
    UINT id = F(UINT, self, 4);
    if (!id || (WORD)id == 0xFFFF) return TRUE;
    F(BOOL, self, 24) = FALSE;
    BOOL r = CallVirt<BOOL>(target, 12, id, -1, self, (void*)0);  // OnCmdMsg(CN_UPDATE_COMMAND_UI)
    if (disableIfNoHandler && !F(BOOL, self, 24)) {
        void* info[2];
        info[0] = 0;
        BOOL handler = CallVirt<BOOL>(target, 12, F(UINT, self, 4), 0, self, (void*)info);
        CallVirt(self, 0, handler);                                // Enable
    }
    return r;
}

// [$L_633694] forwards to the control container (+52), vtbl+84
extern "C" int __fastcall Wnd_ContainerCall84(BYTE* self, void*, int a, int b, int c) {
    return CallVirt<int>(F(void*, self, 52), 84, a, b, c);
}

// [FUN_6336c5] CWnd::GetExStyle
extern "C" DWORD __fastcall Wnd_GetExStyle(BYTE* self) {
    if (F(void*, self, 56)) return CallVirt<DWORD>(F(void*, self, 56), 116);
    return GetWindowLongA(HWND_OF(self), GWL_EXSTYLE);
}

// [FUN_6339c5] _AfxModifyStyle(hWnd, nStyleOffset, dwRemove, dwAdd, nFlags)
extern "C" BOOL __stdcall Afx_ModifyStyle(HWND h, int offset, DWORD remove, DWORD add, UINT flags) {
    DWORD style = GetWindowLongA(h, offset);
    DWORD changed = (style & ~remove) | add;
    if (style == changed) return FALSE;
    SetWindowLongA(h, offset, changed);
    if (flags) SetWindowPos(h, 0, 0, 0, 0, 0, flags | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    return TRUE;
}

// [FUN_633a14] CWnd::ModifyStyleEx(hWnd, dwRemove, dwAdd, nFlags)
extern "C" BOOL __stdcall Wnd_ModifyStyleExStatic(HWND h, DWORD remove, DWORD add, UINT flags) {
    return Afx_ModifyStyle(h, GWL_EXSTYLE, remove, add, flags);
}

// [FUN_6336df] CWnd::ModifyStyleEx
extern "C" BOOL __fastcall Wnd_ModifyStyleEx(BYTE* self, void*, DWORD remove, DWORD add, UINT flags) {
    if (F(void*, self, 56)) return CallVirt<BOOL>(F(void*, self, 56), 124, remove, add, flags);
    return Wnd_ModifyStyleExStatic(HWND_OF(self), remove, add, flags);
}

// [FUN_633710] CWnd::SetWindowText
extern "C" void __fastcall Wnd_SetWindowText(BYTE* self, void*, const char* text) {
    if (F(void*, self, 56)) CallVirt(F(void*, self, 56), 128, text);
    else SetWindowTextA(HWND_OF(self), text);
}

// [FUN_6337c8] CWnd::EnableWindow
extern "C" void __fastcall Wnd_EnableWindow(BYTE* self, void*, BOOL enable) {
    if (F(void*, self, 56)) CallVirt(F(void*, self, 56), 168, enable);
    else EnableWindow(HWND_OF(self), enable);
}

// [FUN_6337ef] CWnd::SetFocus
extern "C" BYTE* __fastcall Wnd_SetFocus(BYTE* self) {
    if (F(void*, self, 56)) return CallVirt<BYTE*>(F(void*, self, 56), 172);
    return Wnd_FromHandle(SetFocus(HWND_OF(self)));
}

// [$L_63397d] CWnd's deleting destructor
extern "C" BYTE* __fastcall Wnd_DeletingDestruct(BYTE* self, void*, int flags) {
    Wnd_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [$L_63384d] static initializer: registers the drag list message
extern "C" void __cdecl Afx_InitDragListMsg() {
    g_msgDragList = RegisterWindowMessageA(g_dragListMsgName);
}

// [$L_633886] destroys wndTop at exit
extern "C" void __cdecl WndTop_Term() {
    if (g_wndStaticsDone & 1) return;
    g_wndStaticsDone |= 1;
    Wnd_Destruct((BYTE*)g_wndTop);
}

// [$L_6338c4] destroys wndBottom at exit
extern "C" void __cdecl WndBottom_Term() {
    if (g_wndStaticsDone & 2) return;
    g_wndStaticsDone |= 2;
    Wnd_Destruct((BYTE*)g_wndBottom);
}

// [$L_633902] destroys wndTopMost at exit
extern "C" void __cdecl WndTopMost_Term() {
    if (g_wndStaticsDone & 4) return;
    g_wndStaticsDone |= 4;
    Wnd_Destruct((BYTE*)g_wndTopMost);
}

// [$L_633940] destroys wndNoTopMost at exit
extern "C" void __cdecl WndNoTopMost_Term() {
    if (g_wndStaticsDone & 8) return;
    g_wndStaticsDone |= 8;
    Wnd_Destruct((BYTE*)g_wndNoTopMost);
}

// [FUN_63386d] constructs wndTop (HWND_TOP)
extern "C" void __cdecl WndTop_Construct() { Wnd_ConstructHwnd((BYTE*)g_wndTop, 0, HWND_TOP); }

// [FUN_6338ab] constructs wndBottom (HWND_BOTTOM)
extern "C" void __cdecl WndBottom_Construct() { Wnd_ConstructHwnd((BYTE*)g_wndBottom, 0, HWND_BOTTOM); }

// [FUN_6338e9] constructs wndTopMost (HWND_TOPMOST)
extern "C" void __cdecl WndTopMost_Construct() { Wnd_ConstructHwnd((BYTE*)g_wndTopMost, 0, HWND_TOPMOST); }

// [FUN_633927] constructs wndNoTopMost (HWND_NOTOPMOST)
extern "C" void __cdecl WndNoTopMost_Construct() { Wnd_ConstructHwnd((BYTE*)g_wndNoTopMost, 0, HWND_NOTOPMOST); }

// [$L_63387a] registers wndTop's destruction
extern "C" void __cdecl WndTop_AtExit() { atexit(WndTop_Term); }

// [$L_6338b8] registers wndBottom's destruction
extern "C" void __cdecl WndBottom_AtExit() { atexit(WndBottom_Term); }

// [$L_6338f6] registers wndTopMost's destruction
extern "C" void __cdecl WndTopMost_AtExit() { atexit(WndTopMost_Term); }

// [$L_633934] registers wndNoTopMost's destruction
extern "C" void __cdecl WndNoTopMost_AtExit() { atexit(WndNoTopMost_Term); }

// [$L_633863] static initializer of wndTop
extern "C" void __cdecl WndTop_Init() { WndTop_Construct(); WndTop_AtExit(); }

// [$L_6338a1] static initializer of wndBottom
extern "C" void __cdecl WndBottom_Init() { WndBottom_Construct(); WndBottom_AtExit(); }

// [$L_6338df] static initializer of wndTopMost
extern "C" void __cdecl WndTopMost_Init() { WndTopMost_Construct(); WndTopMost_AtExit(); }

// [$L_63391d] static initializer of wndNoTopMost
extern "C" void __cdecl WndNoTopMost_Init() { WndNoTopMost_Construct(); WndNoTopMost_AtExit(); }

// ---------------------------------------------------------------------------
// CString helpers, CException, CRuntimeClass, AUX_DATA

CRT_PROC(FUN_620585, "__mbschr")                // _tcschr
KING_VAR(char, g_afxChNil, 72db58)

// [FUN_636309] CString::Release(CStringData*)
extern "C" void __stdcall String_ReleaseData(BYTE* data) {
    if (data != g_afxDataNil && InterlockedDecrement((LONG*)data) <= 0) String_FreeData(data);
}

// [FUN_63632c] CString::Empty
extern "C" void __fastcall String_Empty(char** self) {
    if (!F(int, *self, -8)) return;
    if (F(int, *self, -12) >= 0) String_Release(self);
    else String_Assign(self, 0, &g_afxChNil);
}

// [FUN_63634a] CString::CopyBeforeWrite
extern "C" void __fastcall String_CopyBeforeWrite(char** self) {
    char* old = *self;
    if (F(int, old, -12) <= 1) return;
    String_Release(self);
    String_AllocBuffer(self, 0, F(int, old, -8));
    memcpy(*self, old, F(int, old, -8) + 1);
}

// [FUN_636471] CString::GetBuffer
extern "C" char* __fastcall String_GetBuffer(char** self, void*, int minLen) {
    char* old = *self;
    if (F(int, old, -12) > 1 || minLen > F(int, old, -4)) {
        int len = F(int, old, -8);
        if (minLen < len) minLen = len;
        String_AllocBuffer(self, 0, minLen);
        memcpy(*self, old, len + 1);
        F(int, *self, -8) = len;
        String_ReleaseData((BYTE*)old - 12);
    }
    return *self;
}

// [FUN_6364c0] CString::ReleaseBuffer
extern "C" void __fastcall String_ReleaseBuffer(char** self, void*, int len) {
    String_CopyBeforeWrite(self);
    if (len == -1) len = lstrlenA(*self);
    F(int, *self, -8) = len;
    (*self)[len] = 0;
}

// [FUN_6364e8] CString::GetBufferSetLength
extern "C" char* __fastcall String_GetBufferSetLength(char** self, void*, int len) {
    String_GetBuffer(self, 0, len);
    F(int, *self, -8) = len;
    (*self)[len] = 0;
    return *self;
}

// [FUN_636508] _mbstowcsz
extern "C" int __cdecl Afx_mbstowcsz(wchar_t* w, const char* s, int count) {
    if (!count && w) return 0;
    int r = MultiByteToWideChar(CP_ACP, 0, s, -1, w, count);
    if (r > 0) w[r - 1] = 0;
    return r;
}

// [FUN_6365e8] AfxExtractSubString(rString, lpszFullString, iSubString, chSep)
extern "C" BOOL __stdcall Afx_ExtractSubString(char** out, const char* full, int index, char sep) {
    if (!full) return FALSE;
    while (index--) {
        full = CallC<const char*>(FUN_620585, full, (int)sep);
        if (!full) {
            String_Empty(out);
            return FALSE;
        }
        full++;
    }
    const char* end = CallC<const char*>(FUN_620585, full, (int)sep);
    int len = end ? (int)(end - full) : lstrlenA(full);
    memcpy(String_GetBufferSetLength(out, 0, len), full, len);
    return TRUE;
}

// [FUN_636660] CException::CException(bAutoDelete) body
extern "C" BYTE* __fastcall Exception_SetAutoDelete(BYTE* self, void*, BOOL autoDelete) {
    F(BOOL, self, 4) = autoDelete;
    return self;
}

// [FUN_63666c] CException::Delete
extern "C" void __fastcall Exception_Delete(BYTE* self) {
    if (F(int, self, 4) > 0 && self) CallVirt(self, 4, 1);
}

extern "C" int __stdcall Afx_MessageBoxText(const char* text, UINT type, UINT help);
extern "C" int __stdcall Afx_MessageBoxId(UINT id, UINT type, UINT help);

// [$L_63667e] CException::ReportError(nType, nError)
extern "C" int __fastcall Exception_ReportError(BYTE* self, void*, UINT type, UINT error) {
    char buf[512];
    UINT help;
    if (CallVirt<BOOL>(self, 12, buf, 512, &help))              // GetErrorMessage
        return Afx_MessageBoxText(buf, type, help);
    if (!error) error = 61472;                                     // AFX_IDP_NO_ERROR_AVAILABLE
    return Afx_MessageBoxId(error, type, help);
}

// [FUN_63672b] CRuntimeClass::IsDerivedFrom
extern "C" BOOL __fastcall RuntimeClass_IsDerivedFrom(BYTE* self, void*, BYTE* base) {
    for (; self; self = F(BYTE*, self, 16))
        if (self == base) return TRUE;
    return FALSE;
}

// [FUN_6366d3] CObject::IsKindOf
extern "C" BOOL __fastcall Object_IsKindOf(BYTE* self, void*, BYTE* cls) {
    return RuntimeClass_IsDerivedFrom(CallVirt<BYTE*>(self, 0), 0, cls);
}

KING_VAR(BOOL, g_afxWin4, 72e0d4)             // afxData.bWin4
KING_VAR(int, g_cxVScroll, 72e078)              // afxData scroll bar sizes
KING_VAR(int, g_cyHScroll, 72e07c)
KING_VAR(BOOL, g_scrollPlusOne, 72e0e0)

// [FUN_63b0d9] the Windows 4 scroll bar sizes (one more pixel each)
extern "C" void __cdecl Afx_UpdateBorders4() {
    if (!g_afxWin4) return;
    g_cxVScroll = GetSystemMetrics(SM_CXVSCROLL) + 1;
    g_scrollPlusOne = TRUE;
    g_cyHScroll = GetSystemMetrics(SM_CYHSCROLL) + 1;
}

// [FUN_63b109] the scroll bar sizes
extern "C" void __cdecl Afx_UpdateBorders() {
    g_cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
    g_scrollPlusOne = FALSE;
    g_cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
}

// [FUN_636744] AUX_DATA::UpdateSysColors
extern "C" void __fastcall AuxData_UpdateSysColors(BYTE* self) {
    F(COLORREF, self, 40) = GetSysColor(COLOR_BTNFACE);
    F(COLORREF, self, 44) = GetSysColor(COLOR_BTNSHADOW);
    F(COLORREF, self, 48) = GetSysColor(COLOR_BTNHIGHLIGHT);
    F(COLORREF, self, 52) = GetSysColor(COLOR_BTNTEXT);
    F(COLORREF, self, 56) = GetSysColor(COLOR_WINDOWFRAME);
    F(HBRUSH, self, 36) = GetSysColorBrush(COLOR_BTNFACE);
    F(HBRUSH, self, 32) = GetSysColorBrush(COLOR_WINDOWFRAME);
}

// [FUN_636788] AUX_DATA::UpdateSysMetrics
extern "C" void __fastcall AuxData_UpdateSysMetrics(BYTE* self) {
    F(int, self, 8) = GetSystemMetrics(SM_CXICON);
    F(int, self, 12) = GetSystemMetrics(SM_CYICON);
    if (F(BOOL, self, 104)) Afx_UpdateBorders4();
    else Afx_UpdateBorders();
    HDC dc = GetDC(0);
    F(int, self, 24) = GetDeviceCaps(dc, LOGPIXELSX);
    F(int, self, 28) = GetDeviceCaps(dc, LOGPIXELSY);
    ReleaseDC(0, dc);
}

// [FUN_639e40] _AfxLoadDotBitmap: the radio item dot, sized to the menu
// check marks
extern "C" void __cdecl Afx_LoadDotBitmap() {
    DWORD dims = GetMenuCheckMarkDimensions();
    int cx = (short)LOWORD(dims), cy = (short)HIWORD(dims);
    if (cx > 32) cx = 32;
    int words = (cx + 15) >> 4;
    int shift = (cx - 4) / 2 + words * 16 - cx;
    if (shift > 12) shift = 12;
    if (cy > 32) cy = 32;
    BYTE bits[128];
    memset(bits, 0xFF, 128);
    const BYTE* dot = (const BYTE*)g_dotPattern;
    BYTE* p = bits + ((cy - 6) >> 1) * words * 2;
    for (int i = 0; i < 5; i++, p += words * 2) {
        WORD w = (WORD)~(WORD)(dot[i] << shift);
        p[1] = (BYTE)w;
        p[0] = (BYTE)(w >> 8);
    }
    g_hbmMenuDot = CreateBitmap(cx, cy, 1, 1, bits);
    if (!g_hbmMenuDot) g_hbmMenuDot = LoadBitmapA(0, MAKEINTRESOURCEA(32739));   // OBM_CHECK
}

// ---------------------------------------------------------------------------
// CMenu handle map

KING_VAR(AsmLabel, g_tempMenuClass, 654628)

// [FUN_636a6c] afxMapHMENU
extern "C" BYTE* __stdcall Afx_MapHMENU(BOOL create) {
    return NewHandleMap(F(BYTE*, Afx_GetModuleThreadState(), 24), create, g_tempMenuClass, 4, 1);
}

// [FUN_636ade] CMenu::FromHandle
extern "C" BYTE* __stdcall Menu_FromHandle(HMENU h) { return HandleMap_FromHandle(Afx_MapHMENU(TRUE), 0, h); }

// [FUN_636af3] CMenu::FromHandlePermanent
extern "C" BYTE* __stdcall Menu_FromHandlePermanent(HMENU h) {
    BYTE* map = Afx_MapHMENU(FALSE);
    return map ? (BYTE*)PtrMap_GetValueAt((PtrMap*)(map + 4), 0, h) : 0;
}

// [FUN_636b11] CMenu::Detach
extern "C" HMENU __fastcall Menu_Detach(BYTE* self) {
    HMENU h = F(HMENU, self, 4);
    if (h) {
        BYTE* map = Afx_MapHMENU(FALSE);
        if (map) PtrMap_RemoveKey((PtrMap*)(map + 4), 0, F(HMENU, self, 4));
    }
    F(HMENU, self, 4) = 0;
    return h;
}

// [FUN_636b3b] CMenu::DestroyMenu
extern "C" BOOL __fastcall Menu_DestroyMenu(BYTE* self) {
    if (!F(HMENU, self, 4)) return FALSE;
    return DestroyMenu(Menu_Detach(self));
}

// [$L_636bc7] sends WM_CLOSE to the window of the object at +28
extern "C" LRESULT __fastcall Owner_SendClose(BYTE* self) {
    return SendMessageA(HWND_OF(F(BYTE*, self, 28)), WM_CLOSE, 0, 0);
}

// ---------------------------------------------------------------------------
// CDC (+4 m_hDC, +8 m_hAttribDC)



// [$L_636bee] CDC's deleting destructor
extern "C" BYTE* __fastcall Dc_DeletingDestruct(BYTE* self, void*, int flags) {
    Cdc_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [$L_636d2f] CDC::SetAttribDC
extern "C" void __fastcall Dc_SetAttribDC(BYTE* self, void*, HDC h) { F(HDC, self, 8) = h; }

// [$L_636d39] CDC::SetOutputDC
extern "C" void __fastcall Dc_SetOutputDC(BYTE* self, void*, HDC h) { F(HDC, self, 4) = h; }

// [$L_636d43] CDC::ReleaseAttribDC
extern "C" void __fastcall Dc_ReleaseAttribDC(BYTE* self) { F(HDC, self, 8) = 0; }

// [$L_636d48] CDC::ReleaseOutputDC
extern "C" void __fastcall Dc_ReleaseOutputDC(BYTE* self) { F(HDC, self, 4) = 0; }

// [$L_636d4d] CDC::SaveDC
extern "C" int __fastcall Dc_SaveDC(BYTE* self) {
    int r = 0;
    if (F(HDC, self, 8)) r = SaveDC(F(HDC, self, 8));
    if (F(HDC, self, 4) != F(HDC, self, 8) && SaveDC(F(HDC, self, 4))) r = -1;
    return r;
}

// [$L_636d7e] CDC::RestoreDC
extern "C" BOOL __fastcall Dc_RestoreDC(BYTE* self, void*, int saved) {
    BOOL r = TRUE;
    if (F(HDC, self, 4) != F(HDC, self, 8)) r = RestoreDC(F(HDC, self, 4), saved);
    if (F(HDC, self, 8)) r = r && RestoreDC(F(HDC, self, 8), saved);
    return r;
}

// [$L_636dd3] CDC::SelectStockObject
extern "C" BYTE* __fastcall Dc_SelectStockObject(BYTE* self, void*, int index) {
    HGDIOBJ obj = GetStockObject(index);
    HGDIOBJ old = 0;
    if (F(HDC, self, 4) != F(HDC, self, 8)) old = SelectObject(F(HDC, self, 4), obj);
    if (F(HDC, self, 8)) old = SelectObject(F(HDC, self, 8), obj);
    return GdiObject_FromHandle(old);
}

// [$L_636e0f] CDC::SelectObject(CGdiObject*)
extern "C" BYTE* __fastcall Dc_SelectObject(BYTE* self, void*, BYTE* obj) {
    HGDIOBJ old = 0;
    if (F(HDC, self, 4) != F(HDC, self, 8)) old = SelectObject(F(HDC, self, 4), obj ? F(HGDIOBJ, obj, 4) : 0);
    if (F(HDC, self, 8)) old = SelectObject(F(HDC, self, 8), obj ? F(HGDIOBJ, obj, 4) : 0);
    return GdiObject_FromHandle(old);
}

// [$L_636e55] CDC::SetBkColor
extern "C" COLORREF __fastcall Dc_SetBkColor(BYTE* self, void*, COLORREF c) {
    COLORREF r = CLR_INVALID;
    if (F(HDC, self, 4) != F(HDC, self, 8)) r = SetBkColor(F(HDC, self, 4), c);
    if (F(HDC, self, 8)) r = SetBkColor(F(HDC, self, 8), c);
    return r;
}

// [$L_636e84] CDC::SetTextColor
extern "C" COLORREF __fastcall Dc_SetTextColor(BYTE* self, void*, COLORREF c) {
    COLORREF r = CLR_INVALID;
    if (F(HDC, self, 4) != F(HDC, self, 8)) r = SetTextColor(F(HDC, self, 4), c);
    if (F(HDC, self, 8)) r = SetTextColor(F(HDC, self, 8), c);
    return r;
}

// the viewport/window origin and extent setters: on the output DC when it
// differs, then on the attribute DC; the result comes from the last call
typedef BOOL (WINAPI* DcPointFn)(HDC, int, int, POINT*);
typedef BOOL (WINAPI* DcScaleFn)(HDC, int, int, int, int, SIZE*);

static POINT* DcPoint(BYTE* self, POINT* out, DcPointFn fn, int x, int y) {
    POINT pt = { 0, 0 };
    if (F(HDC, self, 4) != F(HDC, self, 8)) fn(F(HDC, self, 4), x, y, &pt);
    if (F(HDC, self, 8)) fn(F(HDC, self, 8), x, y, &pt);
    *out = pt;
    return out;
}

static SIZE* DcScale(BYTE* self, SIZE* out, DcScaleFn fn, int a, int b, int c, int d) {
    SIZE sz = { 0, 0 };
    if (F(HDC, self, 4) != F(HDC, self, 8)) fn(F(HDC, self, 4), a, b, c, d, &sz);
    if (F(HDC, self, 8)) fn(F(HDC, self, 8), a, b, c, d, &sz);
    *out = sz;
    return out;
}

// [$L_636ee1] CDC::SetViewportOrg
extern "C" POINT* __fastcall Dc_SetViewportOrg(BYTE* self, void*, POINT* out, int x, int y) {
    return DcPoint(self, out, SetViewportOrgEx, x, y);
}

// [$L_636f2d] CDC::OffsetViewportOrg
extern "C" POINT* __fastcall Dc_OffsetViewportOrg(BYTE* self, void*, POINT* out, int x, int y) {
    return DcPoint(self, out, OffsetViewportOrgEx, x, y);
}

// [$L_636f79] CDC::SetViewportExt
extern "C" SIZE* __fastcall Dc_SetViewportExt(BYTE* self, void*, SIZE* out, int x, int y) {
    return (SIZE*)DcPoint(self, (POINT*)out, (DcPointFn)SetViewportExtEx, x, y);
}

// [$L_636fc5] CDC::ScaleViewportExt
extern "C" SIZE* __fastcall Dc_ScaleViewportExt(BYTE* self, void*, SIZE* out, int xn, int xd, int yn, int yd) {
    return DcScale(self, out, ScaleViewportExtEx, xn, xd, yn, yd);
}

// [$L_63701d] CDC::SetWindowExt
extern "C" SIZE* __fastcall Dc_SetWindowExt(BYTE* self, void*, SIZE* out, int x, int y) {
    return (SIZE*)DcPoint(self, (POINT*)out, (DcPointFn)SetWindowExtEx, x, y);
}

// [$L_637069] CDC::ScaleWindowExt
extern "C" SIZE* __fastcall Dc_ScaleWindowExt(BYTE* self, void*, SIZE* out, int xn, int xd, int yn, int yd) {
    return DcScale(self, out, ScaleWindowExtEx, xn, xd, yn, yd);
}

// [$L_6370c1] CDC::GetClipBox
extern "C" int __fastcall Dc_GetClipBox(BYTE* self, void*, RECT* r) { return GetClipBox(F(HDC, self, 4), r); }

// ---------------------------------------------------------------------------
// window helpers, message boxes, the wait cursor, CWinApp document hooks

// [FUN_637228] _AfxChildWindowFromPoint: the visible child control under pt
extern "C" HWND __stdcall Afx_ChildWindowFromPoint(HWND h, POINT pt) {
    ClientToScreen(h, &pt);
    for (HWND child = GetWindow(h, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT)) {
        if ((WORD)GetDlgCtrlID(child) == 0xFFFF) continue;
        if (!(GetWindowLongA(child, GWL_STYLE) & WS_VISIBLE)) continue;
        RECT r;
        GetWindowRect(child, &r);
        if (PtInRect(&r, pt)) return child;
    }
    return 0;
}

// [FUN_63729d] AfxSetWindowText: only when the text changes (no flicker)
extern "C" void __stdcall Afx_SetWindowText(HWND h, const char* text) {
    int len = lstrlenA(text);
    char old[256];
    if ((unsigned)len <= 256 && GetWindowTextA(h, old, 256) == len && !lstrcmpA(old, text)) return;
    SetWindowTextA(h, text);
}

CRT_PROC(FUN_6206b1, "__expand")                // _expand

// [$L_6373bb] AfxCriticalNewHandler: gives back the safety pool and retries
extern "C" int __cdecl Afx_CriticalNewHandler(size_t size) {
    BYTE* ts = Afx_GetThreadState();
    if (ts && F(void*, ts, 12)) {
        size_t have = CallC<size_t>(FUN_620815, F(void*, ts, 12));
        if (have <= size + 4) {
            free(F(void*, ts, 12));
            F(void*, ts, 12) = 0;
        } else {
            CallC(FUN_6206b1, F(void*, ts, 12), have - size - 4);
        }
        return 1;
    }
    ThrowMemory();
    return 0;
}

KING_VAR(HCURSOR, g_hcurWait, 72e0b4)

// [$L_63743f] CWinApp::DoWaitCursor (+160 count, +164 cursor to restore)
extern "C" void __fastcall WinApp_DoWaitCursor(BYTE* self, void*, int code) {
    Afx_LockGlobals(2);
    F(int, self, 160) += code;
    if (F(int, self, 160) > 0) {
        HCURSOR old = SetCursor(g_hcurWait);
        if (code > 0 && F(int, self, 160) == 1) F(HCURSOR, self, 164) = old;
    } else {
        F(int, self, 160) = 0;
        SetCursor(F(HCURSOR, self, 164));
    }
    Afx_UnlockGlobals(2);
}

// CWinApp +128 m_pDocManager, +168 m_pRecentFileList

// [$L_6374b1] CWinApp::AddToRecentFileList
extern "C" void __fastcall WinApp_AddToRecentFileList(BYTE* self, void*, const char* path) {
    BYTE* list = F(BYTE*, self, 168);
    if (list) CallVirt(list, 4, path);                             // Add
}

// [$L_6374c7] CWinApp::AddDocTemplate
extern "C" void __fastcall WinApp_AddDocTemplate(BYTE* self, void*, BYTE* tmpl) {
    CallVirt(F(BYTE*, self, 128), 28, tmpl);
}

// [$L_6374ef] CWinApp::OnUpdateRecentFileMenu
extern "C" void __fastcall WinApp_OnUpdateRecentFileMenu(BYTE* self, void*, BYTE* cmdUI) {
    BYTE* list = F(BYTE*, self, 168);
    if (!list) CallVirt(cmdUI, 0, FALSE);                          // Enable
    else CallVirt(list, 8, cmdUI);                                 // UpdateMenu
}

// [$L_637511] CWinApp::OpenDocumentFile
extern "C" BYTE* __fastcall WinApp_OpenDocumentFile(BYTE* self, void*, const char* name) {
    BYTE* dm = F(BYTE*, self, 128);
    return dm ? CallVirt<BYTE*>(dm, 48, name) : 0;
}

// [$L_63752b] CWinApp::OnOpenRecentFile(nID): opens the file of the MRU entry,
// drops the entry when that fails
extern "C" BOOL __fastcall WinApp_OnOpenRecentFile(BYTE* self, void*, UINT id) {
    int index = id - 57616;                                        // ID_FILE_MRU_FILE1
    BYTE* list = F(BYTE*, self, 168);
    if (!CallVirt<BYTE*>(self, 124, F(char**, list, 8)[index]))    // OpenDocumentFile
        CallVirt(F(BYTE*, self, 168), 0, index);                   // Remove
    return TRUE;
}

// [FUN_63755e] lets the main frame's notify hook (+104) enable or disable
// modeless windows
extern "C" void __fastcall WinApp_EnableModeless(BYTE* self, void*, BOOL enable) {
    BYTE* main = CallC<BYTE*>(FUN_61c4c1);
    if (!main || !CallVirt<BOOL>(main, 176)) return;               // IsFrameWnd
    if (F(BYTE*, main, 104)) CallVirt(F(BYTE*, main, 104), 100, enable);
}

// [FUN_6377a0] the thread state's window at +196
extern "C" BYTE* __cdecl Afx_ThreadStateWnd196() { return F(BYTE*, Afx_GetThreadState(), 196); }

// [FUN_637704] CWnd::GetSafeOwner_(hParent, pWndTop): the owner for a
// modal window; disables the top level window when asked
extern "C" HWND __stdcall Wnd_GetSafeOwner(HWND parent, HWND* top) {
    HWND h = parent;
    if (!h) {
        BYTE* w = Afx_ThreadStateWnd196();
        if (!w) w = CallC<BYTE*>(FUN_61c4c1);
        h = w ? HWND_OF(w) : 0;
    }
    while (h && (GetWindowLongA(h, GWL_STYLE) & WS_CHILD)) h = GetParent(h);
    HWND topWnd = h;
    for (HWND p = h; p; p = GetParent(p)) topWnd = p;
    if (!parent && h) h = GetLastActivePopup(h);
    if (top) {
        if (topWnd && IsWindowEnabled(topWnd) && topWnd != h) {
            *top = topWnd;
            EnableWindow(topWnd, FALSE);
        } else {
            *top = 0;
        }
    }
    return h;
}

// [FUN_63758c] CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt); also
// called with no application (this 0)
extern "C" int __fastcall WinApp_DoMessageBox(BYTE* self, void*, const char* prompt, UINT type, UINT idPrompt) {
    WinApp_EnableModeless(self, 0, FALSE);
    HWND top;
    HWND owner = Wnd_GetSafeOwner(0, &top);
    DWORD* context = 0;
    if (owner) context = (DWORD*)SendMessageA(top, 886, 0, 0);    // WM_HELPPROMPTADDR
    if (!context && self) context = &F(DWORD, self, 156);
    DWORD oldContext = 0;
    if (context) {
        oldContext = *context;
        if (idPrompt) *context = idPrompt + 0x30000;               // HID_BASE_PROMPT
    }
    if (!(type & MB_ICONMASK)) {
        UINT t = type & MB_TYPEMASK;
        if (t <= MB_OKCANCEL || (t > MB_ABORTRETRYIGNORE && t <= MB_YESNO)) type |= MB_ICONEXCLAMATION;
    }
    char file[260];
    const char* title;
    if (self) title = F(const char*, self, 120);
    else {
        GetModuleFileNameA(0, file, 260);
        title = file;
    }
    int r = MessageBoxA(owner, prompt, title, type);
    if (context) *context = oldContext;
    if (top) EnableWindow(top, TRUE);
    WinApp_EnableModeless(self, 0, TRUE);
    return r;
}

// [FUN_637673] AfxMessageBox(lpszText, nType, nIDHelp)
extern "C" int __stdcall Afx_MessageBoxText(const char* text, UINT type, UINT help) {
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app) return CallVirt<int>(app, 140, text, type, help);
    return WinApp_DoMessageBox(0, 0, text, type, help);
}

// [FUN_6376ab] AfxMessageBox(nIDPrompt, nType, nIDHelp)
extern "C" int __stdcall Afx_MessageBoxId(UINT id, UINT type, UINT help) {
    char* s = g_afxPchNil;
    String_LoadString(&s, 0, id);
    if (help == (UINT)-1) help = id;
    int r = Afx_MessageBoxText(s, type, help);
    String_Destruct(&s);
    return r;
}

// ---------------------------------------------------------------------------
// CWnd::OnCommand, AfxRegisterWndClass, CFrameWnd's modal state, help mode
// and view creation. CFrameWnd: +72 m_hAccelTable, +80 m_bHelpMode,
// +104 m_pNotifyHook, +140 m_nIDHelp, +144 m_nIDTracking, +160 m_cModalStack,
// +164 m_phWndDisable; CWnd +36 m_nFlags.

KING_VAR(RECT, g_rectDefault, 72db68)         // CFrameWnd::rectDefault
KING_VAR(UINT, g_msgMouseWheel, 72db60)
KING_VAR(AsmLabel, g_mouseWheelMsgName, 654170)   // "MSWHEEL_ROLLMSG"
KING_VAR(AsmLabel, g_frameMap653d00, 653d00)
KING_VAR(AsmLabel, g_testCmdUIVtbl, 653be0)
KING_VAR(HCURSOR, g_hcurHelp, 72e0bc)
KING_VAR(AsmLabel, g_wndFrameOrViewClass, 653a3c)
KING_VAR(AsmLabel, g_classNameShortFmt, 653bd0)  // "Afx:%x:%x"
KING_VAR(AsmLabel, g_classNameFmt, 653bbc)       // "Afx:%x:%x:%x:%x:%x"
extern "C" void FUN_63569a(); KING_ALIAS("_FUN_63569a", "_Wnd_ReflectLastMsg@8")                            // CWnd::ReflectLastMsg

// [$L_637821] static initializer of CFrameWnd::rectDefault
extern "C" void __cdecl FrameWnd_InitRectDefault() {
    g_rectDefault.right = 0;
    g_rectDefault.bottom = 0;
    g_rectDefault.left = CW_USEDEFAULT;
    g_rectDefault.top = CW_USEDEFAULT;
}

// [$L_637844] static initializer: the mouse wheel message of Windows 95 and
// NT 3.x
extern "C" void __cdecl Afx_InitMouseWheelMsg() {
    if (((GetVersion() & 0x80000000) && (WORD)GetVersion() == 4)
        || (!(GetVersion() & 0x80000000) && (WORD)GetVersion() == 3))
        g_msgMouseWheel = RegisterWindowMessageA(g_mouseWheelMsgName);
    else
        g_msgMouseWheel = 0;
}

// [$L_63788e] CFrameWnd map getter ($L_653d00)
extern "C" void* __fastcall FrameWnd_GetMap653d00(BYTE* self) { return g_frameMap653d00; }

// [$L_637956] CFrameWnd's deleting destructor
extern "C" BYTE* __fastcall FrameWnd_DeletingDestruct(BYTE* self, void*, int flags) {
    FrameWnd_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [FUN_635117] CTestCmdUI::CTestCmdUI
extern "C" BYTE* __fastcall TestCmdUI_Construct(BYTE* self) {
    CmdUI_Construct(self);
    F(void*, self, 0) = g_testCmdUIVtbl;
    F(BOOL, self, 40) = TRUE;                                      // m_bEnabled
    return self;
}

// [$L_635130] CTestCmdUI::Enable
extern "C" void __fastcall TestCmdUI_Enable(BYTE* self, void*, BOOL on) {
    F(BOOL, self, 24) = TRUE;
    F(BOOL, self, 40) = on;
}

// [$L_635141] CTestCmdUI::SetCheck/SetRadio/SetText: nothing
extern "C" void __fastcall TestCmdUI_Ignore(BYTE* self, void*, int) {}

// [FUN_635144] CWnd::OnCommand
extern "C" BOOL __fastcall Wnd_OnCommand(BYTE* self, void*, WPARAM w, LPARAM l) {
    UINT id = LOWORD(w);
    int code = HIWORD(w);
    if (!l) {
        if (!id) return FALSE;
        BYTE state[44];
        TestCmdUI_Construct(state);
        F(UINT, state, 4) = id;
        CallVirt(self, 12, id, -1, (void*)state, (void*)0);       // OnCmdMsg(CN_UPDATE_COMMAND_UI)
        if (!F(BOOL, state, 40)) return TRUE;
        code = 0;
    } else {
        if (F(HWND, Afx_GetThreadState(), 184) == HWND_OF(self)) return TRUE;  // m_hLockoutNotifyWindow
        if (CallStd<BOOL>(FUN_63569a, (HWND)l, (LRESULT*)0)) return TRUE;
        if (!id) return FALSE;
    }
    return CallVirt<BOOL>(self, 12, id, code, (void*)0, (void*)0);
}

// [FUN_63493f] AfxRegisterWndClass: "Afx:..." class names, registered once
extern "C" const char* __stdcall Afx_RegisterWndClass(UINT style, HCURSOR cursor, HBRUSH brush, HICON icon) {
    char* name = (char*)Afx_GetThreadState() + 88;                 // m_szTempClassName
    HINSTANCE inst = F(HINSTANCE, Afx_GetModuleState(), 8);
    if (!cursor && !brush && !icon) wsprintfA(name, g_classNameShortFmt, inst, style);
    else wsprintfA(name, g_classNameFmt, inst, style, cursor, brush, icon);
    WNDCLASSA wc;
    if (!GetClassInfoA(inst, name, &wc)) {
        wc.style = style;
        wc.lpfnWndProc = DefWindowProcA;
        wc.cbClsExtra = wc.cbWndExtra = 0;
        wc.lpszMenuName = 0;
        wc.hIcon = icon;
        wc.hInstance = inst;
        wc.hCursor = cursor;
        wc.hbrBackground = brush;
        wc.lpszClassName = name;
        if (!Afx_RegisterClass(&wc)) CallC(FUN_6371c5);           // AfxThrowResourceException
    }
    return name;
}

// [FUN_63611b] CFrameWnd::IsTracking: a menu item's prompt is shown
extern "C" BOOL __fastcall FrameWnd_IsTracking(BYTE* self) {
    UINT id = F(UINT, self, 144);
    return id && id != 57346 && id != 57345;                       // AFX_IDS_HELPMODEMESSAGE, AFX_IDS_IDLEMESSAGE
}

// [FUN_637a3a] CFrameWnd::LoadAccelTable
extern "C" BOOL __fastcall FrameWnd_LoadAccelTable(BYTE* self, void*, const char* name) {
    F(HACCEL, self, 72) = LoadAcceleratorsA(F(HINSTANCE, Afx_GetModuleState(), 12), name);
    return F(HACCEL, self, 72) != 0;
}

// [$L_637b07] CFrameWnd handler: Default, then tells the notify hook
// (vtbl+108)
extern "C" void __fastcall FrameWnd_DefaultThenHook108(BYTE* self, void*, int a) {
    Wnd_Default(self);
    BYTE* hook = F(BYTE*, self, 104);
    if (hook) CallVirt(hook, 108, a);
}

// [$L_637b23] CFrameWnd handler: the notify hook's answer (vtbl+112), else
// Default
extern "C" LRESULT __fastcall FrameWnd_HookOrDefault112(BYTE* self) {
    BYTE* hook = F(BYTE*, self, 104);
    if (hook && CallVirt<BOOL>(hook, 112)) return TRUE;
    return Wnd_Default(self);
}

// [$L_637b44] CFrameWnd::ExitHelpMode
extern "C" void __fastcall FrameWnd_ExitHelpMode(BYTE* self) {
    if (!F(BOOL, self, 80)) return;
    MSG msg;
    if (!PeekMessageA(&msg, HWND_OF(self), 871, 871, PM_REMOVE | PM_NOYIELD))   // WM_EXITHELPMODE
        PostMessageA(HWND_OF(self), 871, 0, 0);
    if (GetCapture() == HWND_OF(self)) ReleaseCapture();
    BYTE* frame = Wnd_GetTopLevelFrame(self);
    F(BOOL, self, 80) = FALSE;
    F(BOOL, frame, 80) = FALSE;
    PostMessageA(HWND_OF(self), 874, 0, 0);                        // WM_IDLEUPDATECMDUI
}

// [FUN_637baf] CFrameWnd::OnSetCursor: the help cursor in help mode
extern "C" BOOL __fastcall FrameWnd_OnSetCursor(BYTE* self, void*, BYTE* wnd, UINT hit, UINT msg) {
    if (F(BOOL, Wnd_GetTopLevelFrame(self), 80)) {
        SetCursor(g_hcurHelp);
        return TRUE;
    }
    return (BOOL)Wnd_Default(self);
}

// [$L_637bd9] CFrameWnd::OnCommandHelp
extern "C" LRESULT __fastcall FrameWnd_OnCommandHelp(BYTE* self, void*, WPARAM w, LPARAM context) {
    if (!context) {
        if (FrameWnd_IsTracking(self)) context = F(UINT, self, 144) + 0x10000;    // HID_BASE_COMMAND
        else context = F(UINT, self, 140) + 0x20000;                              // HID_BASE_RESOURCE
        if (!context) return FALSE;
    }
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app) CallVirt(app, 152, (DWORD)context, HELP_CONTEXT);    // WinHelp
    return TRUE;
}

// [$L_637c31] CFrameWnd::OnHelpHitTest
extern "C" LRESULT __fastcall FrameWnd_OnHelpHitTest(BYTE* self, void*, WPARAM, LPARAM) {
    return F(UINT, self, 140) ? F(UINT, self, 140) + 0x20000 : 0;
}

// [$L_637c47] CFrameWnd::OnCommand: commands in help mode show their help
extern "C" BOOL __fastcall FrameWnd_OnCommand(BYTE* self, void*, WPARAM w, LPARAM l) {
    UINT id = LOWORD(w);
    if (F(BOOL, Wnd_GetTopLevelFrame(self), 80) && !l && id != 57670 && id != 57671 && id != 57669) {
        if (!SendMessageA(HWND_OF(self), 869, 0, id + 0x10000))   // WM_COMMANDHELP
            SendMessageA(HWND_OF(self), WM_COMMAND, 57671, 0);     // ID_DEFAULT_HELP
        return TRUE;
    }
    return Wnd_OnCommand(self, 0, w, l);
}

// windows of this application below the desktop that BeginModalState
// disables: enabled, permanent, descendants of parent and not refusing
// (WM_DISABLEMODAL)
static BOOL ModalCandidate(HWND h, BYTE* parent) {
    return IsWindowEnabled(h) && Wnd_FromHandlePermanent(h) && Afx_IsDescendant(HWND_OF(parent), h)
        && !SendMessageA(h, 876, 0, 0);
}

// [$L_637cde] CFrameWnd::BeginModalState
extern "C" void __fastcall FrameWnd_BeginModalState(BYTE* self) {
    if ((UINT)++F(int, self, 160) > 1) return;
    BYTE* parent = Wnd_GetTopLevelParent(self);
    int n = 0;
    HWND h = GetWindow(GetDesktopWindow(), GW_CHILD);
    if (!h) return;
    for (; h; h = GetWindow(h, GW_HWNDNEXT))
        if (ModalCandidate(h, parent)) n++;
    if (!n) return;
    HWND* list = (HWND*)operator new(n * 4 + 4);
    F(HWND*, self, 164) = list;
    n = 0;
    for (h = GetWindow(GetDesktopWindow(), GW_CHILD); h; h = GetWindow(h, GW_HWNDNEXT)) {
        if (ModalCandidate(h, parent)) {
            EnableWindow(h, FALSE);
            F(HWND*, self, 164)[n++] = h;
        }
    }
    F(HWND*, self, 164)[n] = 0;
}

// [$L_637dfa] CFrameWnd::EndModalState
extern "C" void __fastcall FrameWnd_EndModalState(BYTE* self) {
    if (!F(int, self, 160)) return;
    if (--F(UINT, self, 160) > 0) return;
    HWND* list = F(HWND*, self, 164);
    if (!list) return;
    for (int i = 0; F(HWND*, self, 164)[i]; i++)
        if (IsWindow(F(HWND*, self, 164)[i])) EnableWindow(F(HWND*, self, 164)[i], TRUE);
    operator delete(F(HWND*, self, 164));
    F(HWND*, self, 164) = 0;
}

// [FUN_637e69] CFrameWnd::ShowOwnedWindows: hides or shows again the
// top level windows owned by this one
extern "C" void __fastcall FrameWnd_ShowOwnedWindows(BYTE* self, void*, BOOL show) {
    for (HWND h = GetWindow(GetDesktopWindow(), GW_CHILD); h; h = GetWindow(h, GW_HWNDNEXT)) {
        BYTE* w = Wnd_FromHandlePermanent(h);
        if (!w || HWND_OF(self) == h || !Afx_IsDescendant(HWND_OF(self), h)) continue;
        DWORD style = GetWindowLongA(h, GWL_STYLE);
        if (!show) {
            if ((style & (WS_VISIBLE | WS_DISABLED)) == WS_VISIBLE) {
                ShowWindow(h, SW_HIDE);
                F(DWORD, w, 36) |= 2;                              // WF_TEMPHIDE
            }
        } else if (!(style & (WS_VISIBLE | WS_DISABLED)) && (F(DWORD, w, 36) & 2)) {
            ShowWindow(h, SW_SHOWNOACTIVATE);
            F(DWORD, w, 36) &= ~2;
        }
    }
}

// [$L_637f01] CFrameWnd::OnEnable
extern "C" void __fastcall FrameWnd_OnEnable(BYTE* self, void*, BOOL enable) {
    if (enable && (F(DWORD, self, 36) & 4)) {                      // WF_STAYDISABLED
        Wnd_EnableWindow(self, 0, FALSE);
        SetFocus(0);
        return;
    }
    if (Wnd_FromHandle(GetParent(HWND_OF(self)))) return;
    if (!enable) {
        if (!F(int, self, 160)) {
            F(DWORD, self, 36) |= 0x80;                            // WF_MODALDISABLE
            CallVirt(self, 136);                                   // BeginModalState
        }
    } else {
        if (F(DWORD, self, 36) & 0x80) {
            F(DWORD, self, 36) &= ~0x80;
            CallVirt(self, 140);                                   // EndModalState
            HWND h = HWND_OF(self);
            if (GetActiveWindow() == h) SendMessageA(h, WM_ACTIVATE, WA_ACTIVE, 0);
        }
        if (F(DWORD, self, 36) & 32)                               // WF_STAYACTIVE
            SendMessageA(HWND_OF(self), WM_NCACTIVATE, TRUE, 0);
    }
    Wnd_NotifyFloatingWindows(self, 0, enable ? 16 : 32);          // FS_ENABLE / FS_DISABLE
}

// [$L_638061] CFrameWnd::PreCreateWindow
extern "C" BOOL __fastcall FrameWnd_PreCreateWindow(BYTE* self, void*, CREATESTRUCTA* cs) {
    if (!cs->lpszClass) {
        Afx_EndDeferRegisterClass(8);                              // AFX_WNDFRAMEORVIEW_REG
        cs->lpszClass = g_wndFrameOrViewClass;
    }
    if (g_afxWin4) {
        if (cs->style & 0x8000) cs->style |= 0x4000;               // FWS_ADDTOTITLE -> FWS_PREFIXTITLE
        cs->dwExStyle |= WS_EX_CLIENTEDGE;
    }
    return TRUE;
}

// [FUN_63813d] CFrameWnd::CreateView(pContext, nID)
extern "C" BYTE* __fastcall FrameWnd_CreateView(BYTE* self, void*, BYTE** context, UINT id) {
    BYTE* view = (BYTE*)RuntimeClass_CreateObject(context[0]);    // m_pNewViewClass
    if (!view) return 0;
    RECT r = { 0, 0, 0, 0 };
    if (!CallVirt<BOOL>(view, 84, (const char*)0, (const char*)0, (DWORD)0x50800000, &r, self, id, context))
        return 0;                                                  // Create(AFX_WS_DEFAULT_VIEW)
    if (g_afxWin4 && (Wnd_GetExStyle(view) & WS_EX_CLIENTEDGE))
        Wnd_ModifyStyleEx(self, 0, WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
    return view;
}

// [$L_6381b5] CFrameWnd::OnCreateClient
extern "C" BOOL __fastcall FrameWnd_OnCreateClient(BYTE* self, void*, CREATESTRUCTA* cs, BYTE** context) {
    if (context && context[0] && !FrameWnd_CreateView(self, 0, context, 59648))   // AFX_IDW_PANE_FIRST
        return FALSE;
    return TRUE;
}

// [$L_6381d7] CFrameWnd::OnCreate (message map entry)
extern "C" int __fastcall FrameWnd_OnCreateMsg(BYTE* self, void*, CREATESTRUCTA* cs) {
    return FrameWnd_OnCreate(self, 0, cs, cs->lpCreateParams);
}

// [FUN_638235] CFrameWnd::GetIconWndClass(dwDefaultStyle, nIDResource): the
// frame's window class with the resource's icon
extern "C" const char* __fastcall FrameWnd_GetIconWndClass(BYTE* self, void*, DWORD style, UINT id) {
    HICON icon = LoadIconA(F(HINSTANCE, Afx_GetModuleState(), 12), MAKEINTRESOURCEA((WORD)id));
    if (!icon) return 0;
    CREATESTRUCTA cs;
    memset(&cs, 0, sizeof cs);
    cs.style = style;
    CallVirt(self, 92, &cs);                                       // PreCreateWindow
    if (!cs.lpszClass) return 0;
    WNDCLASSA wc;
    if (!GetClassInfoA(F(HINSTANCE, Afx_GetModuleState(), 8), cs.lpszClass, &wc) || wc.hIcon == icon) return 0;
    return Afx_RegisterWndClass(wc.style, wc.hCursor, wc.hbrBackground, icon);
}

// ---------------------------------------------------------------------------
// CWnd's system change handlers; CFrameWnd's activation, menus, DDE and
// session end. CFrameWnd +60 m_bAutoMenuEnable, +68 m_hMenuDefault,
// +152 m_pViewActive; CWinApp +176 m_atomApp, +178 m_atomSystemTopic.

#include <shellapi.h>
typedef void (WINAPI* VoidFn)();
extern "C" void FUN_6332c8(); KING_ALIAS("_FUN_6332c8", "@CmdTarget_OnCmdMsg@24")                            // CCmdTarget::OnCmdMsg
extern "C" void FUN_638fd9(); KING_ALIAS("_FUN_638fd9", "@FrameWnd_DestroyDockBars@4")                            // CFrameWnd::DestroyDockBars
extern "C" void FUN_6327b7(); KING_ALIAS("_FUN_6327b7", "@WinApp_DevModeChange@12")                            // CWinApp::DevModeChange
extern "C" void FUN_63566d(); KING_ALIAS("_FUN_63566d", "@Wnd_SendChildNotifyLastMsg@12")                            // CWnd::SendChildNotifyLastMsg
extern "C" void FUN_6317b8(); KING_ALIAS("_FUN_6317b8", "_UnpackDDElParam@16")                            // UnpackDDElParam
extern "C" void FUN_6317b2(); KING_ALIAS("_FUN_6317b2", "_ReuseDDElParam@20")                            // ReuseDDElParam
KING_VAR(AsmLabel, g_afxData, 72e078)
KING_VAR(int, g_gotScrollLines, 72c16c)

extern "C" BYTE* __fastcall FrameWnd_Get152(BYTE* self);
#define GetActiveView FrameWnd_Get152

// [FUN_639958] AfxOleSetUserCtrl
extern "C" void __stdcall Afx_OleSetUserCtrl(BOOL userCtrl) { F(BOOL, Afx_GetModuleState(), 48) = userCtrl; }

// [FUN_6358ae] CWnd::OnActivateTopLevel: tool tips go with deactivation
extern "C" LRESULT __fastcall Wnd_OnActivateTopLevel(BYTE* self, void*, WPARAM w, LPARAM l) {
    if (LOWORD(w) == WA_INACTIVE && !(F(BYTE, Afx_GetThreadState(), 223) & 64)) Afx_CancelToolTips(TRUE);
    return 0;
}

// [$L_6358d0] CWnd::OnSysColorChange
extern "C" void __fastcall Wnd_OnSysColorChange(BYTE* self) {
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app && F(BYTE*, app, 28) == self) AuxData_UpdateSysColors((BYTE*)g_afxData);
    if (!F(BYTE, Afx_GetModuleState(), 20) && Afx_GetThread() && F(BYTE*, Afx_GetThread(), 28) == self) {
        BYTE* ctl = (BYTE*)ProcessLocal_GetData(&g_ctl3dState, 0, (void* (__stdcall*)())$L_639de0);
        if (F(void*, ctl, 28)) F(VoidFn, ctl, 28)();      // Ctl3dColorChange
    }
    if (!(Wnd_GetStyle(self) & WS_CHILD))
        Wnd_SendMessageToDescendants(HWND_OF(self), WM_SYSCOLORCHANGE, 0, 0, TRUE, TRUE);
    Wnd_Default(self);
}

// [FUN_635a4f] CWnd::OnDisplayChange
extern "C" LRESULT __fastcall Wnd_OnDisplayChange(BYTE* self, void*, WPARAM, LPARAM) {
    if (CallC<BYTE*>(FUN_61c4c1) == self) AuxData_UpdateSysMetrics((BYTE*)g_afxData);
    if (!(Wnd_GetStyle(self) & WS_CHILD)) {
        MSG* m = Afx_GetCurrentMessage();
        Wnd_SendMessageToDescendants(HWND_OF(self), m->message, m->wParam, m->lParam, TRUE, TRUE);
    }
    return Wnd_Default(self);
}

// [$L_63594d] CWnd::OnSettingChange
extern "C" void __fastcall Wnd_OnSettingChange(BYTE* self, void*, UINT, const char*) {
    if (!F(BYTE, Afx_GetModuleState(), 20)) {
        BYTE* ctl = (BYTE*)ProcessLocal_GetData(&g_ctl3dState, 0, (void* (__stdcall*)())$L_639de0);
        if (Afx_GetThread() && F(BYTE*, Afx_GetThread(), 28) == self && F(void*, ctl, 36))
            F(VoidFn, ctl, 36)();                        // Ctl3dWinIniChange
    }
    g_gotScrollLines = 0;
    Wnd_OnDisplayChange(self, 0, 0, 0);
}

// [$L_6359a0] CWnd::OnDevModeChange
extern "C" void __fastcall Wnd_OnDevModeChange(BYTE* self, void*, char* device) {
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app && F(BYTE*, app, 28) == self) CallThis(FUN_6327b7, app, device);
    if (!(Wnd_GetStyle(self) & WS_CHILD)) {
        MSG* m = Afx_GetCurrentMessage();
        Wnd_SendMessageToDescendants(HWND_OF(self), m->message, m->wParam, m->lParam, TRUE, TRUE);
    }
}

// [$L_6359eb] CWnd::OnHelpInfo: F1 without modifiers sends ID_HELP to the
// main window
extern "C" BOOL __fastcall Wnd_OnHelpInfo(BYTE* self, void*, HELPINFO*) {
    BYTE* main;
    if (!(Wnd_GetStyle(self) & WS_CHILD) && (main = CallC<BYTE*>(FUN_61c4c1)) != 0
        && GetKeyState(VK_SHIFT) >= 0 && GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_MENU) >= 0) {
        SendMessageA(HWND_OF(main), WM_COMMAND, 57670, 0);         // ID_HELP
        return TRUE;
    }
    return (BOOL)Wnd_Default(self);
}

// [$L_635a98] reflects the message to the window at lParam+4
extern "C" LRESULT __fastcall Wnd_ReflectToStruct4(BYTE* self, void*, WPARAM, LPARAM l) {
    LRESULT r = l;
    if (CallStd<BOOL>(FUN_63569a, F(HWND, l, 4), &r)) return r;
    return Wnd_Default(self);
}

// [$L_635ac2] CWnd::OnHScroll/OnVScroll: to the scroll bar control first
extern "C" void __fastcall Wnd_OnScroll(BYTE* self, void*, UINT, UINT, BYTE* bar) {
    if (bar && CallThis<BOOL>(FUN_63566d, bar, (LRESULT*)0)) return;
    Wnd_Default(self);
}

// [FUN_635ae3] CWnd::OnEnterIdle: dispatches WM_ENTERIDLE posted meanwhile
extern "C" void __fastcall Wnd_OnEnterIdle(BYTE* self, void*, UINT, BYTE*) {
    MSG msg;
    while (PeekMessageA(&msg, 0, WM_ENTERIDLE, WM_ENTERIDLE, PM_REMOVE)) DispatchMessageA(&msg);
    Wnd_Default(self);
}

// [$L_635b2d] CWnd::OnCtlColor: the control's own colours first
extern "C" HBRUSH __fastcall Wnd_OnCtlColor(BYTE* self, void*, BYTE* dc, BYTE* wnd, UINT) {
    LRESULT r = (LRESULT)wnd;
    if (CallThis<BOOL>(FUN_63566d, wnd, &r)) return (HBRUSH)r;
    return (HBRUSH)Wnd_Default(self);
}

// [$L_638382] CFrameWnd::OnUpdateFrameMenu
extern "C" void __fastcall FrameWnd_OnUpdateFrameMenu(BYTE* self, void*, HMENU alt) {
    if (!alt) {
        BYTE* doc = CallVirt<BYTE*>(self, 188);                    // GetActiveDocument
        if (!doc || (alt = CallVirt<HMENU>(doc, 164)) == 0)        // GetDefaultMenu
            alt = F(HMENU, self, 68);
    }
    SetMenu(HWND_OF(self), alt);
}

// [$L_6384af] CFrameWnd::OnDestroy
extern "C" void __fastcall FrameWnd_OnDestroy(BYTE* self) {
    CallThis(FUN_638fd9, self);
    HMENU def = F(HMENU, self, 68);
    if (def && GetMenu(HWND_OF(self)) != def) SetMenu(HWND_OF(self), def);
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app && F(BYTE*, app, 28) == self) WinHelpA(HWND_OF(self), 0, HELP_QUIT, 0);
    Wnd_OnDestroy(self);
}

// [$L_638502] CFrameWnd::OnCmdMsg: the active view, the frame, then the
// application; the frame is the routing frame meanwhile
extern "C" BOOL __fastcall FrameWnd_OnCmdMsg(BYTE* self, void*, UINT id, int code, void* extra, void* info) {
    BYTE* ts = Afx_GetThreadState();
    BYTE* old = F(BYTE*, ts, 196);
    F(BYTE*, ts, 196) = self;
    BOOL r = TRUE;
    BYTE* view = GetActiveView(self);
    if (!(view && CallVirt<BOOL>(view, 12, id, code, extra, info))
        && !CallThis<BOOL>(FUN_6332c8, self, id, code, extra, info)) {
        BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
        r = app && CallVirt<BOOL>(app, 12, id, code, extra, info);
    }
    F(BYTE*, ts, 196) = old;
    return r;
}

// [$L_6385aa] CFrameWnd: WM_HSCROLL to the active view
extern "C" void __fastcall FrameWnd_HScrollToView(BYTE* self, void*, UINT, UINT, BYTE*) {
    BYTE* view = GetActiveView(self);
    if (!view) return;
    MSG* m = Afx_GetCurrentMessage();
    SendMessageA(HWND_OF(view), WM_HSCROLL, m->wParam, m->lParam);
}

// [$L_6385d3] CFrameWnd: WM_VSCROLL to the active view
extern "C" void __fastcall FrameWnd_VScrollToView(BYTE* self, void*, UINT, UINT, BYTE*) {
    BYTE* view = GetActiveView(self);
    if (!view) return;
    MSG* m = Afx_GetCurrentMessage();
    SendMessageA(HWND_OF(view), WM_VSCROLL, m->wParam, m->lParam);
}

// [$L_6385fc] CFrameWnd::OnActivateTopLevel
extern "C" LRESULT __fastcall FrameWnd_OnActivateTopLevel(BYTE* self, void*, WPARAM w, LPARAM l) {
    Wnd_OnActivateTopLevel(self, 0, w, l);
    CallVirt(self, 240);                                           // ExitHelpMode
    BYTE* hook = F(BYTE*, self, 104);
    if (hook) CallVirt(hook, 88, (BOOL)(LOWORD(w) && !HIWORD(w)));  // OnActivate
    if (F(BYTE*, Afx_GetThread(), 28) == self) {
        BYTE* view = GetActiveView(self);
        if (!view) view = GetActiveView(CallVirt<BYTE*>(self, 192));   // GetActiveFrame
        if (view) CallVirt(view, 228, FALSE, view, view);          // OnActivateView
    }
    PostMessageA(HWND_OF(self), 874, 0, 0);                        // WM_IDLEUPDATECMDUI
    return 0;
}

// [$L_63868a] CFrameWnd::OnActivate
extern "C" void __fastcall FrameWnd_OnActivate(BYTE* self, void*, UINT state, BYTE* other, BOOL minimized) {
    Wnd_Default(self);
    BYTE* top = (Wnd_GetStyle(self) & WS_CHILD) ? self : Wnd_GetTopLevelFrame(self);
    BYTE* active = state ? self : other;
    BOOL stay = top == active
        || (top == Wnd_GetTopLevelFrame(active) && SendMessageA(HWND_OF(active), 877, 64, 0));   // WM_FLOATSTATUS, FS_SYNCACTIVE
    F(DWORD, top, 36) &= ~32;
    if (stay) F(DWORD, top, 36) |= 32;                             // WF_STAYACTIVE
    Wnd_NotifyFloatingWindows(self, 0, stay ? 4 : 8);              // FS_ACTIVATE / FS_DEACTIVATE
    BYTE* view = GetActiveView(self);
    if (!view && (view = GetActiveView(CallVirt<BYTE*>(self, 192))) == 0) return;
    if (state && !minimized) CallVirt(view, 228, TRUE, view, view); // OnActivateView
    CallVirt(view, 232, state, self);                              // OnActivateFrame
}

// [$L_63875e] CFrameWnd::OnNcActivate
extern "C" BOOL __fastcall FrameWnd_OnNcActivate(BYTE* self, void*, BOOL active) {
    if (F(DWORD, self, 36) & 32) active = TRUE;
    if (!Wnd_IsWindowEnabled(self)) active = FALSE;
    return CallVirt<BOOL>(self, 160, (UINT)WM_NCACTIVATE, (WPARAM)active, (LPARAM)0);   // DefWindowProc
}

// [FUN_638795] CFrameWnd::OnSysCommand: system commands show their help in
// help mode
extern "C" void __fastcall FrameWnd_OnSysCommand(BYTE* self, void*, UINT id, LPARAM l) {
    BYTE* frame = Wnd_GetTopLevelFrame(self);
    id &= 0xFFF0;
    if (F(BOOL, frame, 80)) {
        switch (id) {
        case SC_SIZE: case SC_MOVE: case SC_MINIMIZE: case SC_MAXIMIZE: case SC_NEXTWINDOW:
        case SC_PREVWINDOW: case SC_CLOSE: case SC_RESTORE: case SC_TASKLIST:
            if (!SendMessageA(HWND_OF(self), 869, 0, ((id - 0xF000) >> 4) + 0x1EF00))   // WM_COMMANDHELP
                SendMessageA(HWND_OF(self), WM_COMMAND, 57671, 0);                      // ID_DEFAULT_HELP
            return;
        }
    }
    Wnd_Default(self);
}

// [$L_63882a] CFrameWnd::OnDropFiles: opens each dropped file
extern "C" void __fastcall FrameWnd_OnDropFiles(BYTE* self, void*, HDROP drop) {
    Wnd_FromHandle(SetActiveWindow(HWND_OF(self)));
    UINT n = DragQueryFileA(drop, (UINT)-1, 0, 0);
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    for (UINT i = 0; i < n; i++) {
        char name[260];
        DragQueryFileA(drop, i, name, 260);
        CallVirt(app, 124, name);                                  // OpenDocumentFile
    }
    DragFinish(drop);
}

// [$L_63889c] CFrameWnd::OnQueryEndSession
extern "C" BOOL __fastcall FrameWnd_OnQueryEndSession(BYTE* self) {
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app && F(BYTE*, app, 28) == self) return CallVirt<BOOL>(app, 136);   // SaveAllModified
    return TRUE;
}

// [$L_6388c1] CFrameWnd::OnEndSession
extern "C" void __fastcall FrameWnd_OnEndSession(BYTE* self, void*, BOOL ending) {
    if (!ending) return;
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (!app || F(BYTE*, app, 28) != self) return;
    Afx_OleSetUserCtrl(TRUE);
    WinApp_CloseAllDocuments(app, 0, TRUE);
    CallVirt(app, 104);                                            // ExitInstance
}

// [$L_6388f9] CFrameWnd::OnDDEInitiate: answers for the application's
// system topic
extern "C" LRESULT __fastcall FrameWnd_OnDDEInitiate(BYTE* self, void*, WPARAM w, LPARAM l) {
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app && LOWORD(l) && HIWORD(l) && LOWORD(l) == F(WORD, app, 176) && HIWORD(l) == F(WORD, app, 178)) {
        char name[264];
        GlobalGetAtomNameA(F(ATOM, app, 176), name, 259);
        GlobalAddAtomA(name);
        GlobalGetAtomNameA(F(ATOM, app, 178), name, 259);
        GlobalAddAtomA(name);
        SendMessageA((HWND)w, 996, (WPARAM)HWND_OF(self), MAKELPARAM(F(WORD, app, 176), F(WORD, app, 178)));   // WM_DDE_ACK
    }
    return 0;
}

// [$L_6389bc] CFrameWnd::OnDDEExecute: acknowledges, then hands the command
// to the application while the frame is enabled
extern "C" LRESULT __fastcall FrameWnd_OnDDEExecute(BYTE* self, void*, WPARAM w, LPARAM l) {
    UINT unused;
    HGLOBAL data;
    CallStd(FUN_6317b8, (UINT)1000, l, &unused, (UINT*)&data);    // WM_DDE_EXECUTE
    char command[520];
    lstrcpynA(command, (const char*)GlobalLock(data), 520);
    GlobalUnlock(data);
    PostMessageA((HWND)w, 996, (WPARAM)HWND_OF(self),
                 CallStd<LPARAM>(FUN_6317b2, l, (UINT)1000, (UINT)996, (UINT)0x8000, (UINT)(UINT_PTR)data));
    if (Wnd_IsWindowEnabled(self))
        CallVirt(F(BYTE*, Afx_GetModuleState(), 4), 148, command);   // OnDDECommand
    return 0;
}

// [$L_638a57] CFrameWnd::OnDDETerminate
extern "C" LRESULT __fastcall FrameWnd_OnDDETerminate(BYTE* self, void*, WPARAM w, LPARAM l) {
    PostMessageA((HWND)w, 993, (WPARAM)HWND_OF(self), l);         // WM_DDE_TERMINATE
    return 0;
}

// [$L_638a79] CFrameWnd::OnSetFocus: to the active view
extern "C" void __fastcall FrameWnd_OnSetFocus(BYTE* self, void*, BYTE* old) {
    if (F(BYTE*, self, 152)) Wnd_SetFocus(F(BYTE*, self, 152));
    else Wnd_Default(self);
}

// [$L_638b9f] CFrameWnd handler: tells the notify hook (vtbl+120), then Default
extern "C" void __fastcall FrameWnd_HookThenDefault120(BYTE* self, void*, int a) {
    if (F(BYTE*, self, 104)) CallVirt(F(BYTE*, self, 104), 120, a);
    Wnd_Default(self);
}

// [$L_638bbd] CFrameWnd::OnInitMenuPopup: updates the popup's items through
// CCmdUI
extern "C" void __fastcall FrameWnd_OnInitMenuPopup(BYTE* self, void*, BYTE* menu, UINT index, BOOL sysMenu) {
    Afx_CancelModes(HWND_OF(self));
    if (sysMenu) return;
    BYTE* hook = F(BYTE*, self, 104);
    if (hook && CallVirt<BOOL>(hook, 116, menu, index, FALSE)) return;   // OnInitMenuPopup
    BYTE state[40];
    CmdUI_Construct(state);
    F(BYTE*, state, 12) = menu;                                    // m_pMenu
    HMENU h = F(HMENU, menu, 4);
    if (F(HMENU, Afx_GetThreadState(), 84) == h) {                 // m_hTrackingMenu
        F(BYTE*, state, 36) = menu;                                // m_pParentMenu
    } else if (GetMenu(HWND_OF(self))) {
        BYTE* parent = Wnd_GetTopLevelParent(self);
        HMENU pm;
        if (parent && (pm = GetMenu(HWND_OF(parent))) != 0) {
            int n = GetMenuItemCount(pm);
            for (int i = 0; i < n; i++)
                if (GetSubMenu(pm, i) == F(HMENU, menu, 4)) {
                    F(BYTE*, state, 36) = Menu_FromHandle(pm);
                    break;
                }
        }
    }
    F(UINT, state, 32) = GetMenuItemCount(F(HMENU, menu, 4));     // m_nIndexMax
    for (F(UINT, state, 8) = 0; F(UINT, state, 8) < F(UINT, state, 32); F(UINT, state, 8)++) {
        UINT id = GetMenuItemID(F(HMENU, menu, 4), F(UINT, state, 8));
        F(UINT, state, 4) = id;
        if (!id) continue;
        if (id == (UINT)-1) {
            BYTE* sub = Menu_FromHandle(GetSubMenu(F(HMENU, menu, 4), F(UINT, state, 8)));
            F(BYTE*, state, 16) = sub;                             // m_pSubMenu
            if (!sub) continue;
            id = GetMenuItemID(F(HMENU, sub, 4), 0);
            F(UINT, state, 4) = id;
            if (!id || id == (UINT)-1) continue;
            CmdUI_DoUpdate(state, 0, self, FALSE);
        } else {
            F(BYTE*, state, 16) = 0;
            CmdUI_DoUpdate(state, 0, self, F(BOOL, self, 60) && id < 0xF000);
        }
        UINT count = GetMenuItemCount(F(HMENU, menu, 4));
        if (count < F(UINT, state, 32)) {
            F(UINT, state, 8) += count - F(UINT, state, 32);
            while (F(UINT, state, 8) < count
                   && GetMenuItemID(F(HMENU, menu, 4), F(UINT, state, 8)) == F(UINT, state, 4))
                F(UINT, state, 8)++;
        }
        F(UINT, state, 32) = count;
    }
}

// ---------------------------------------------------------------------------
// collections leftovers, CWinThread::Run and the message filter, the command
// dispatch (CCmdTarget::OnCmdMsg, _AfxDispatchCmdMsg)

KING_VAR(AsmLabel, g_memoryExceptionClass, 654520)   // RUNTIME_CLASS(CMemoryException)
KING_VAR(AsmLabel, g_userExceptionClass, 654350)     // RUNTIME_CLASS(CUserException)
KING_VAR(UINT, g_afxWinVer, 72e0cc)                  // afxData.nWinVer
ASM_PROC(FUN_6201a0)                                   // memcmp

// [$L_632225] CPtrList's deleting destructor
extern "C" BYTE* __fastcall PtrList_DeletingDestruct(BYTE* self, void*, int flags) {
    PtrList_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [FUN_6322e2] CPtrList::FreeNode (+12 count, +16 free list)
extern "C" void __fastcall PtrList_FreeNode(BYTE* self, void*, void** node) {
    node[0] = F(void*, self, 16);
    F(void**, self, 16) = node;
    if (--F(int, self, 12) == 0) PtrList_RemoveAll(self);
}

// [FUN_632324] CPtrList::RemoveHead (+4 head, +8 tail; node: next, prev, data)
extern "C" void* __fastcall PtrList_RemoveHead(BYTE* self) {
    void** node = F(void**, self, 4);
    void* data = node[2];
    void** next = (void**)node[0];
    F(void**, self, 4) = next;
    if (next) next[1] = 0;
    else F(void*, self, 8) = 0;
    PtrList_FreeNode(self, 0, node);
    return data;
}

// [$L_6323a5] CMapPtrToPtr's deleting destructor
extern "C" BYTE* __fastcall PtrMap_DeletingDestruct(BYTE* self, void*, int flags) {
    PtrMap_Destruct((PtrMap*)self);
    if (flags & 1) operator delete(self);
    return self;
}

// [FUN_632629] AfxThrowMemoryException
extern "C" void __cdecl Afx_ThrowMemoryException() { ThrowMemory(); }

// [$L_63269f] CWinApp::WinHelp: leaves the application's help mode (+132)
// and asks the main window
extern "C" void __fastcall WinApp_WinHelp(BYTE* self, void*, DWORD data, UINT cmd) {
    BYTE* main = CallC<BYTE*>(FUN_61c4c1);
    F(BOOL, self, 132) = FALSE;
    PostMessageA(HWND_OF(main), 874, 0, 0);                        // WM_KICKIDLE
    CallVirt(main, 108, data, cmd);                                // CWnd::WinHelp
}

// [FUN_632f0f] CWinThread::ProcessWndProcException
extern "C" LRESULT __fastcall WinThread_ProcessWndProcException(BYTE* self, void*, BYTE* e, const MSG* m) {
    if (m->message == WM_CREATE) return -1;
    if (m->message == WM_PAINT) ValidateRect(m->hwnd, 0);
    return 0;
}

// [$L_6326d6] CWinApp::ProcessWndProcException: reports the exception
extern "C" LRESULT __fastcall WinApp_ProcessWndProcException(BYTE* self, void*, BYTE* e, const MSG* m) {
    if (m->message == WM_CREATE || m->message == WM_PAINT)
        return WinThread_ProcessWndProcException(self, 0, e, m);
    LRESULT r = 0;
    UINT idp = 61704;                                              // AFX_IDP_INTERNAL_FAILURE
    if (m->message == WM_COMMAND) {
        if (!m->lParam) idp++;                                     // AFX_IDP_COMMAND_FAILURE
        r = 1;
    }
    if (Object_IsKindOf(e, 0, (BYTE*)g_memoryExceptionClass))
        CallVirt(e, 16, MB_ICONEXCLAMATION | MB_SYSTEMMODAL, idp);   // ReportError
    else if (!Object_IsKindOf(e, 0, (BYTE*)g_userExceptionClass))
        CallVirt(e, 16, MB_ICONSTOP, idp);
    return r;
}

// [FUN_632c50] CWinThread::Run: the message loop with idle processing
extern "C" int __fastcall WinThread_Run(BYTE* self) {
    BOOL idle = TRUE;
    LONG idleCount = 0;
    MSG* cur = &F(MSG, self, 48);                                  // m_msgCur
    for (;;) {
        while (idle && !PeekMessageA(cur, 0, 0, 0, PM_NOREMOVE))
            if (!CallVirt<BOOL>(self, 96, idleCount++)) idle = FALSE;  // OnIdle
        do {
            if (!CallVirt<BOOL>(self, 92)) return CallVirt<int>(self, 104);   // PumpMessage, ExitInstance
            if (CallVirt<BOOL>(self, 100, cur)) {                  // IsIdleMessage
                idle = TRUE;
                idleCount = 0;
            }
        } while (PeekMessageA(cur, 0, 0, 0, PM_NOREMOVE));
    }
}

// [$L_632d28] returns +56
extern "C" void* __fastcall Object_Get56(BYTE* self) { return F(void*, self, 56); }

// [FUN_632f8c] _AfxIsHelpKey: F1 without modifiers or repeat
extern "C" BOOL __stdcall Afx_IsHelpKey(const MSG* m) {
    return m->message == WM_KEYDOWN && m->wParam == VK_F1 && !(HIWORD(m->lParam) & KF_REPEAT)
        && GetKeyState(VK_SHIFT) >= 0 && GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_MENU) >= 0;
}

// [$L_632fd7] CWinThread::ProcessMessageFilter: help keys in menus and
// dialogs, PreTranslateMessage for the active window's dialogs
extern "C" BOOL __fastcall WinThread_ProcessMessageFilter(BYTE* self, void*, int code, MSG* m) {
    if (!m) return FALSE;
    if (code == MSGF_MENU) {
        BYTE* wnd = Wnd_FromHandle(m->hwnd);
        BYTE* frame;
        if (wnd && (frame = Wnd_GetTopLevelFrame(wnd)) != 0 && FrameWnd_IsTracking(frame) && F(BOOL, frame, 80)) {
            BYTE* main = CallC<BYTE*>(FUN_61c4c1);
            if (F(BYTE*, self, 28)
                && ((m->message == WM_KEYDOWN && m->wParam == VK_RETURN) || m->message == WM_LBUTTONUP)) {
                SendMessageA(HWND_OF(main), WM_COMMAND, 57670, 0); // ID_HELP
                return TRUE;
            }
        }
    } else if (code != MSGF_DIALOGBOX) {
        return FALSE;
    }
    BYTE* main = CallC<BYTE*>(FUN_61c4c1);
    if (g_afxWinVer < 0x333 && main && Afx_IsHelpKey(m)) {
        SendMessageA(HWND_OF(main), WM_COMMAND, 57670, 0);
        return TRUE;
    }
    if (code != MSGF_DIALOGBOX || !F(BYTE*, self, 32) || m->message < WM_KEYFIRST || m->message > 264)   // WM_KEYLAST of the time
        return FALSE;
    BYTE* ts = Afx_GetThreadState();
    if (F(BOOL, ts, 188)) return FALSE;                            // m_bInMsgFilter
    F(BOOL, ts, 188) = TRUE;
    MSG copy = *m;
    if (Wnd_IsWindowEnabled(F(BYTE*, self, 32)) && CallVirt<BOOL>(self, 88, &copy)) {   // PreTranslateMessage
        F(BOOL, ts, 188) = FALSE;
        return TRUE;
    }
    F(BOOL, ts, 188) = FALSE;
    return FALSE;
}

// [FUN_6331c1] _AfxDispatchCmdMsg: calls a command handler of the given
// signature (AFX_PMSG pfn is a thiscall member of the target)
extern "C" BOOL __stdcall Afx_DispatchCmdMsg(BYTE* target, UINT id, int code, void (*pfn)(), void* extra,
                                             UINT sig, void** handlerInfo) {
    if (handlerInfo) {
        handlerInfo[0] = target;
        handlerInfo[1] = (void*)pfn;
        return TRUE;
    }
    void** x = (void**)extra;
    BYTE* ui = (BYTE*)extra;
    switch (sig) {
    case 2:  return CallThis<BOOL>(pfn, target, id);                  // AfxSig_bw
    case 12: CallThis(pfn, target); return TRUE;                      // AfxSig_vv
    case 13: CallThis(pfn, target, id); return TRUE;                  // AfxSig_vw
    case 35: return CallThis<BOOL>(pfn, target);                      // AfxSig_bv
    case 38: CallThis(pfn, target, x[1], x[0]); return TRUE;          // AfxSig_vNMHDRpl
    case 39: return CallThis<BOOL>(pfn, target, x[1], x[0]);          // AfxSig_bNMHDRpl
    case 40: CallThis(pfn, target, id, x[1], x[0]); return TRUE;      // AfxSig_vwNMHDRpl
    case 41: return CallThis<BOOL>(pfn, target, id, x[1], x[0]);      // AfxSig_bwNMHDRpl
    case 44: {                                                        // AfxSig_cmdui
        CallThis(pfn, target, ui);
        BOOL r = !F(BOOL, ui, 28);                                    // m_bContinueRouting
        F(BOOL, ui, 28) = FALSE;
        return r;
    }
    case 45: {                                                        // AfxSig_cmduiw
        CallThis(pfn, target, ui, id);
        BOOL r = !F(BOOL, ui, 28);
        F(BOOL, ui, 28) = FALSE;
        return r;
    }
    case 46: CallThis(pfn, target, extra); return TRUE;               // AfxSig_vpv
    case 47: return CallThis<BOOL>(pfn, target, extra);               // AfxSig_bpv
    }
    return FALSE;
}

// [FUN_6332c8] CCmdTarget::OnCmdMsg: control events, OLE commands, then the
// message maps
extern "C" BOOL __fastcall CmdTarget_OnCmdMsg(BYTE* self, void*, UINT id, int code, void* extra, void* info) {
    if (code == -2)                                                // CN_EVENT
        return CallVirt<BOOL>(F(BYTE*, Afx_GetModuleState(), 4152), 4, self, id, extra, info);   // afxOccManager->OnEvent
    if (code == -3) {                                              // CN_OLECOMMAND
        BYTE* ui = (BYTE*)extra;
        const GUID* group = F(const GUID*, ui, 48);
        BOOL found = FALSE;
        for (void** map = CallVirt<void**>(self, 44); map && !found; map = (void**)map[0]) {   // GetCommandMap
            for (BYTE* e = (BYTE*)map[1]; F(UINT, e, 4) && F(UINT, e, 8) && !found; e += 12) {
                if (id != F(UINT, e, 4)) continue;
                const GUID* g = F(const GUID*, e, 0);
                if (!group ? !g : g && !CallC<int>(FUN_6201a0, group, g, 16)) {
                    found = TRUE;
                    F(UINT, ui, 4) = F(UINT, e, 8);
                }
            }
        }
        return found;
    }
    UINT msg;
    if (code == -1) msg = WM_COMMAND;                              // CN_UPDATE_COMMAND_UI
    else {
        msg = HIWORD(code);
        code = LOWORD(code);
        if (!msg) msg = WM_COMMAND;
    }
    for (const MsgMap* map = CallVirt<const MsgMap*>(self, 40); map; map = map->base) {   // GetMessageMap
        const MsgMapEntry* e = Afx_FindMessageEntry(map->entries, msg, code, id);
        if (e) return Afx_DispatchCmdMsg(self, id, code, e->pfn, extra, (UINT)e->sig, (void**)info);
    }
    return FALSE;
}

// [$L_6333d7] returns TRUE (one argument)
extern "C" BOOL __fastcall CmdTarget_True1(BYTE* self, void*, int) { return TRUE; }

// [$L_6333dd] returns 0x80029C4A (two arguments)
extern "C" HRESULT __fastcall CmdTarget_E80029C4A(BYTE* self, void*, int, int) { return (HRESULT)0x80029C4A; }

// [FUN_6333e5] CCmdTarget::BeginWaitCursor
extern "C" void __fastcall CmdTarget_BeginWaitCursor(BYTE* self) {
    CallVirt(F(BYTE*, Afx_GetModuleState(), 4), 144, 1);           // DoWaitCursor
}

// [FUN_6333fa] CCmdTarget::EndWaitCursor
extern "C" void __fastcall CmdTarget_EndWaitCursor(BYTE* self) {
    CallVirt(F(BYTE*, Afx_GetModuleState(), 4), 144, -1);
}

// ---------------------------------------------------------------------------
// CWnd: child notifications and their reflection, owner-drawn items,
// RepositionBars, GetDescendantWindow, WinHelp

// [FUN_63566d] CWnd::SendChildNotifyLastMsg: the last sent message to
// OnChildNotify
extern "C" BOOL __fastcall Wnd_SendChildNotifyLastMsg(BYTE* self, void*, LRESULT* result) {
    BYTE* ts = Afx_GetThreadState();
    return CallVirt<BOOL>(self, 168, F(UINT, ts, 56), F(WPARAM, ts, 60), F(LPARAM, ts, 64), result);
}

// [FUN_63569a] CWnd::ReflectLastMsg: the last sent message to the child's
// CWnd (or its OLE control site)
extern "C" BOOL __stdcall Wnd_ReflectLastMsg(HWND child, LRESULT* result) {
    BYTE* map = Afx_MapHWND(FALSE);
    if (!map) return FALSE;
    BYTE* w = (BYTE*)PtrMap_GetValueAt((PtrMap*)(map + 4), 0, child);
    if (w) return Wnd_SendChildNotifyLastMsg(w, 0, result);
    BYTE* parent = (BYTE*)PtrMap_GetValueAt((PtrMap*)(map + 4), 0, GetParent(child));
    if (!parent || !F(BYTE*, parent, 52)) return FALSE;
    void* site = PtrMap_GetValueAt((PtrMap*)(F(BYTE*, parent, 52) + 32), 0, child);   // m_siteMap
    if (!site) return FALSE;
    BYTE temp[64];
    Wnd_ConstructHwnd(temp, 0, child);
    F(void*, temp, 56) = site;                                     // m_pCtrlSite
    BOOL r = Wnd_SendChildNotifyLastMsg(temp, 0, result);
    HWND_OF(temp) = 0;
    Wnd_Destruct(temp);
    return r;
}

// [FUN_635799] CWnd::ReflectChildNotify: as WM_REFLECT_BASE+msg through
// the message map
extern "C" BOOL __fastcall Wnd_ReflectChildNotify(BYTE* self, void*, UINT msg, WPARAM w, LPARAM l, LRESULT* result) {
    switch (msg) {
    case WM_HSCROLL: case WM_VSCROLL: case WM_PARENTNOTIFY:
    case 43: case 44: case 45: case 46: case 47:                   // WM_DRAWITEM..WM_VKEYTOITEM
    case WM_COMPAREITEM:
        return Wnd_OnWndMsg(self, 0, 0xBC00 + msg, w, l, result);
    case WM_COMMAND:
        if (CmdTarget_OnCmdMsg(self, 0, 0, MAKELONG(HIWORD(w), 0xBC00 + WM_COMMAND), 0, 0)) {
            if (result) *result = 1;
            return TRUE;
        }
        return FALSE;
    case WM_NOTIFY: {
        void* notify[2] = { result, (void*)l };                    // AFX_NOTIFY
        return CmdTarget_OnCmdMsg(self, 0, 0, MAKELONG((WORD)((NMHDR*)l)->code, 0xBC00 + WM_NOTIFY), notify, 0);
    }
    }
    if (msg >= WM_CTLCOLORMSGBOX && msg <= WM_CTLCOLORSTATIC) {
        struct { HWND wnd; HDC dc; UINT type; } ctl = { 0, (HDC)w, msg - WM_CTLCOLORMSGBOX };   // AFX_CTLCOLOR
        BOOL r = Wnd_OnWndMsg(self, 0, 0xBC00 + 25, 0, (LPARAM)&ctl, result);                    // WM_CTLCOLOR
        return *result ? r : FALSE;
    }
    return FALSE;
}

// [$L_63573e] CWnd::OnChildNotify
extern "C" BOOL __fastcall Wnd_OnChildNotify(BYTE* self, void*, UINT msg, WPARAM w, LPARAM l, LRESULT* result) {
    if (!F(void*, self, 56)) return Wnd_ReflectChildNotify(self, 0, msg, w, l, result);
    LRESULT r = SendMessageA(HWND_OF(self), 0x2000 + msg, w, l);  // OCM__BASE
    if (msg >= WM_CTLCOLORMSGBOX && msg <= WM_CTLCOLORSTATIC && !r) return FALSE;
    if (result) *result = r;
    return TRUE;
}

// [$L_635881] CWnd::OnParentNotify: creation and destruction to the child
extern "C" void __fastcall Wnd_OnParentNotify(BYTE* self, void*, UINT msg, LPARAM l) {
    if ((LOWORD(msg) == WM_CREATE || LOWORD(msg) == WM_DESTROY) && Wnd_ReflectLastMsg((HWND)l, 0)) return;
    Wnd_Default(self);
}

// [$L_6351d0] CWnd::OnNotify
extern "C" BOOL __fastcall Wnd_OnNotify(BYTE* self, void*, WPARAM, LPARAM l, LRESULT* result) {
    NMHDR* hdr = (NMHDR*)l;
    HWND ctrl = hdr->hwndFrom;
    UINT id = LOWORD(GetDlgCtrlID(ctrl));
    UINT code = hdr->code;
    if (F(HWND, Afx_GetThreadState(), 184) == HWND_OF(self)) return TRUE;   // m_hLockoutNotifyWindow
    if (Wnd_ReflectLastMsg(ctrl, result)) return TRUE;
    void* notify[2] = { result, hdr };
    return CallVirt<BOOL>(self, 12, id, (int)MAKELONG((WORD)code, WM_NOTIFY), (void*)notify, (void*)0);
}

// [$L_634368] CWnd::PreCreateWindow
extern "C" BOOL __fastcall Wnd_PreCreateWindow(BYTE* self, void*, CREATESTRUCTA* cs) {
    if (!cs->lpszClass) {
        Afx_EndDeferRegisterClass(1);                              // AFX_WND_REG
        cs->lpszClass = &g_classWnd;
    }
    return TRUE;
}

// [$L_634388] CWnd::Create: a child window
extern "C" BOOL __fastcall Wnd_Create(BYTE* self, void*, const char* cls, const char* name, DWORD style,
                                      const RECT* r, BYTE* parent, UINT id, void* context) {
    return Wnd_CreateEx(self, 0, 0, cls, name, style | WS_CHILD, r->left, r->top, r->right - r->left,
                        r->bottom - r->top, parent ? HWND_OF(parent) : 0, (HMENU)(UINT_PTR)id, context);
}

// [$L_634580] does nothing
extern "C" void __fastcall Wnd_Nothing(BYTE* self) {}

// [$L_634626] CWnd::GetSuperWndProcAddr
extern "C" WNDPROC* __fastcall Wnd_GetSuperWndProcAddr(BYTE* self) { return &F(WNDPROC, self, 40); }

// [$L_6346a0] CWnd::OnToolHitTest: the child control under the point
extern "C" int __fastcall Wnd_OnToolHitTest(BYTE* self, void*, POINT pt, BYTE* ti) {
    HWND child = Afx_ChildWindowFromPoint(HWND_OF(self), pt);
    if (!child) return -1;
    int hit = LOWORD(GetDlgCtrlID(child));
    if (ti && F(UINT, ti, 0) >= 40) {                              // TOOLINFO of MFC 4.2
        F(UINT, ti, 4) |= 1;                                       // TTF_IDISHWND
        F(char*, ti, 36) = (char*)-1;                              // LPSTR_TEXTCALLBACK
        F(HWND, ti, 8) = HWND_OF(self);
        F(HWND, ti, 12) = child;
        if (!(SendMessageA(child, WM_GETDLGCODE, 0, 0) & DLGC_BUTTON)) F(UINT, ti, 4) |= 0x80000002;
    }
    return hit;
}

// [$L_634711] CWnd::OnDrawItem
extern "C" void __fastcall Wnd_OnDrawItem(BYTE* self, void*, int, DRAWITEMSTRUCT* dis) {
    if (dis->CtlType == ODT_MENU) {
        BYTE* menu = Menu_FromHandlePermanent((HMENU)dis->hwndItem);
        if (menu) {
            CallVirt(menu, 12, dis);                               // DrawItem
            return;
        }
    } else if (Wnd_ReflectLastMsg(dis->hwndItem, 0)) {
        return;
    }
    Wnd_Default(self);
}

// [$L_63474e] CWnd::OnCompareItem
extern "C" int __fastcall Wnd_OnCompareItem(BYTE* self, void*, int, COMPAREITEMSTRUCT* cis) {
    LRESULT r = (LRESULT)cis;
    if (Wnd_ReflectLastMsg(cis->hwndItem, &r)) return (int)r;
    return (int)Wnd_Default(self);
}

// [$L_634778] CWnd::OnDeleteItem
extern "C" void __fastcall Wnd_OnDeleteItem(BYTE* self, void*, int, DELETEITEMSTRUCT* dis) {
    if (!Wnd_ReflectLastMsg(dis->hwndItem, 0)) Wnd_Default(self);
}

// [$L_634798] CWnd::OnCharToItem/OnVKeyToItem: the list box first
extern "C" int __fastcall Wnd_OnListBoxKey(BYTE* self, void*, UINT, BYTE* list, UINT) {
    LRESULT r = (LRESULT)list;
    if (list && Wnd_SendChildNotifyLastMsg(list, 0, &r)) return (int)r;
    return (int)Wnd_Default(self);
}

// [FUN_6347c3] _AfxFindPopupMenuFromID: the popup holding the command
extern "C" BYTE* __stdcall Afx_FindPopupMenuFromID(BYTE* menu, UINT id) {
    int n = GetMenuItemCount(F(HMENU, menu, 4));
    for (int i = 0; i < n; i++) {
        BYTE* sub = Menu_FromHandle(GetSubMenu(F(HMENU, menu, 4), i));
        if (sub) {
            BYTE* found = Afx_FindPopupMenuFromID(sub, id);
            if (found) return found;
        } else if (GetMenuItemID(F(HMENU, menu, 4), i) == id) {
            return Menu_FromHandlePermanent(F(HMENU, menu, 4));
        }
    }
    return 0;
}

extern "C" BYTE* __stdcall Wnd_GetDescendantWindow(HWND h, int id, BOOL onlyPerm);

// [$L_634826] CWnd::OnMeasureItem
extern "C" void __fastcall Wnd_OnMeasureItem(BYTE* self, void*, int, MEASUREITEMSTRUCT* mis) {
    if (mis->CtlType == ODT_MENU) {
        BYTE* ts = Afx_GetThreadState();
        HMENU h = F(HWND, ts, 80) == HWND_OF(self) ? F(HMENU, ts, 84) : GetMenu(HWND_OF(self));   // m_hTrackingWindow/Menu
        BYTE* menu = Afx_FindPopupMenuFromID(Menu_FromHandle(h), mis->itemID);
        if (menu) CallVirt(menu, 16, mis);                         // MeasureItem
    } else {
        BYTE* child = Wnd_GetDescendantWindow(HWND_OF(self), mis->CtlID, TRUE);
        if (child && Wnd_SendChildNotifyLastMsg(child, 0, 0)) return;
    }
    Wnd_Default(self);
}

// [$L_6349f3] CWnd::OnNTCtlColor: WM_CTLCOLORxxx as WM_CTLCOLOR
extern "C" LRESULT __fastcall Wnd_OnNTCtlColor(BYTE* self, void*, WPARAM w, LPARAM l) {
    struct { HWND wnd; HDC dc; UINT type; } ctl;
    ctl.dc = (HDC)w;
    ctl.wnd = (HWND)l;
    ctl.type = F(UINT, Afx_GetThreadState(), 56) - WM_CTLCOLORMSGBOX;
    return CallVirt<LRESULT>(self, 152, (UINT)25, (WPARAM)0, (LPARAM)&ctl);   // WindowProc(WM_CTLCOLOR)
}

// [$L_634a39] CWnd::WinHelp: cancels modes everywhere, then starts WinHelp
// on the top level window
extern "C" void __fastcall Wnd_WinHelp(BYTE* self, void*, DWORD data, UINT cmd) {
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    CmdTarget_BeginWaitCursor(F(BYTE*, Afx_GetModuleState(), 4));
    if (CallVirt<BOOL>(self, 176)) CallVirt(self, 240);           // IsFrameWnd, ExitHelpMode
    SendMessageA(HWND_OF(self), WM_CANCELMODE, 0, 0);
    Wnd_SendMessageToDescendants(HWND_OF(self), WM_CANCELMODE, 0, 0, TRUE, TRUE);
    BYTE* top = Wnd_GetTopLevelParent(self);
    SendMessageA(HWND_OF(top), WM_CANCELMODE, 0, 0);
    Wnd_SendMessageToDescendants(HWND_OF(top), WM_CANCELMODE, 0, 0, TRUE, TRUE);
    HWND capture = GetCapture();
    if (capture) SendMessageA(capture, WM_CANCELMODE, 0, 0);
    if (!WinHelpA(HWND_OF(top), F(const char*, app, 140), cmd, data))
        Afx_MessageBoxId(61703, 0, (UINT)-1);                      // AFX_IDP_FAILED_TO_LAUNCH_HELP
    CmdTarget_EndWaitCursor(F(BYTE*, Afx_GetModuleState(), 4));
}

// [FUN_635336] CWnd::GetDescendantWindow
extern "C" BYTE* __stdcall Wnd_GetDescendantWindow(HWND h, int id, BOOL onlyPerm) {
    HWND child = GetDlgItem(h, id);
    if (child) {
        if (GetTopWindow(child)) {
            BYTE* w = Wnd_GetDescendantWindow(child, id, onlyPerm);
            if (w) return w;
        }
        if (!onlyPerm) return Wnd_FromHandle(child);
        BYTE* w = Wnd_FromHandlePermanent(child);
        if (w) return w;
    }
    for (child = GetTopWindow(h); child; child = GetWindow(child, GW_HWNDNEXT)) {
        BYTE* w = Wnd_GetDescendantWindow(child, id, onlyPerm);
        if (w) return w;
    }
    return 0;
}

// AFX_SIZEPARENTPARAMS
struct SizeParentParams { HDWP dwp; RECT rect; SIZE total; BOOL stretch; };

// [FUN_635566] AfxRepositionWindow: moves the window when its rectangle
// changes, deferred when there is a DeferWindowPos handle
extern "C" void __stdcall Afx_RepositionWindow(SizeParentParams* layout, HWND h, const RECT* r) {
    HWND parent = GetParent(h);
    if (layout && !layout->dwp) return;
    RECT old;
    GetWindowRect(h, &old);
    ScreenToClient(parent, (POINT*)&old.left);
    ScreenToClient(parent, (POINT*)&old.right);
    if (EqualRect(&old, r)) return;
    if (layout)
        layout->dwp = DeferWindowPos(layout->dwp, h, 0, r->left, r->top, r->right - r->left, r->bottom - r->top,
                                     SWP_NOACTIVATE | SWP_NOZORDER);
    else
        SetWindowPos(h, 0, r->left, r->top, r->right - r->left, r->bottom - r->top, SWP_NOACTIVATE | SWP_NOZORDER);
}

// [FUN_63542c] CWnd::RepositionBars: lays out the control bars, gives the
// rest to the window nIDLeftOver
extern "C" void __fastcall Wnd_RepositionBars(BYTE* self, void*, UINT first, UINT last, UINT leftOver, UINT flags,
                                              RECT* param, const RECT* client, BOOL stretch) {
    SizeParentParams layout;
    HWND leftWnd = 0;
    layout.stretch = stretch;
    layout.total.cx = layout.total.cy = 0;
    if (client) layout.rect = *client;
    else GetClientRect(HWND_OF(self), &layout.rect);
    layout.dwp = flags != 1 ? BeginDeferWindowPos(8) : 0;          // reposQuery
    for (HWND child = GetTopWindow(HWND_OF(self)); child; child = GetWindow(child, GW_HWNDNEXT)) {
        UINT id = LOWORD(GetDlgCtrlID(child));
        BYTE* w = Wnd_FromHandlePermanent(child);
        if (id == leftOver) leftWnd = child;
        else if (id >= first && id <= last && w) SendMessageA(child, 865, 0, (LPARAM)&layout);   // WM_SIZEPARENT
    }
    if (flags == 1) {
        if (stretch) CopyRect(param, &layout.rect);
        else {
            param->left = param->top = 0;
            param->right = layout.total.cx;
            param->bottom = layout.total.cy;
        }
        return;
    }
    if (leftOver && leftWnd) {
        BYTE* w = Wnd_FromHandle(leftWnd);
        if (flags == 2) {                                          // reposExtra
            layout.rect.left += param->left;
            layout.rect.top += param->top;
            layout.rect.right -= param->right;
            layout.rect.bottom -= param->bottom;
        }
        CallVirt(w, 96, &layout.rect, 0);                          // CalcWindowRect
        Afx_RepositionWindow(&layout, leftWnd, &layout.rect);
    }
    if (layout.dwp) EndDeferWindowPos(layout.dwp);
}

// [$L_635600] CWnd::CalcWindowRect
extern "C" void __fastcall Wnd_CalcWindowRect(BYTE* self, void*, RECT* r, UINT adjust) {
    DWORD ex = Wnd_GetExStyle(self);
    if (!adjust) ex &= ~WS_EX_CLIENTEDGE;
    AdjustWindowRectEx(r, Wnd_GetStyle(self), FALSE, ex);
}

// [$L_635d7e] returns flag 16 of +36
extern "C" DWORD __fastcall Object_Flag36_16(BYTE* self) { return F(DWORD, self, 36) & 16; }

// [$L_635d85] stores +44; when flag 16 of +36 is set, clears it and posts
// an empty message to +28
extern "C" void __fastcall Object_Set44Post(BYTE* self, void*, DWORD v) {
    F(DWORD, self, 44) = v;
    if (F(DWORD, self, 36) & 16) {
        F(DWORD, self, 36) &= ~16;
        PostMessageA(F(HWND, self, 28), 0, 0, 0);
    }
}

// ---------------------------------------------------------------------------
// CFrameWnd: menu prompts and the message bar, tool tips, indicators, the
// title, layout and activation; CControlBar helpers. CFrameWnd +64 m_nWindow,
// +88 m_rectBorder, +108 m_listControlBars (+112 its head), +148
// m_nIDLastMessage, +156 m_lpfnCloseProc, +172 m_strTitle, +176
// m_bInRecalcLayout, +184 m_nIdleFlags.

extern "C" void FUN_638aa4(); KING_ALIAS("_FUN_638aa4", "@FrameWnd_ShowControlBar@20")                            // CFrameWnd::ShowControlBar
KING_VAR(char, g_fmtWindowNumber, 65427c)     // ":%d"
KING_VAR(char, g_titleSeparator, 654278)      // " - "
KING_VAR(BYTE*, g_staticDocManager, 72c160)   // CDocManager::pStaticDocManager
KING_VAR(BOOL, g_docManagerStaticInit, 685e34)

// [FUN_638fc2] CFrameWnd::SetMessageText(nID)
extern "C" void __fastcall FrameWnd_SetMessageTextId(BYTE* self, void*, UINT id) {
    SendMessageA(HWND_OF(self), 866, id, 0);                       // WM_SETMESSAGESTRING
}

// [$L_638d3e] CFrameWnd::OnMenuSelect: the prompt of the selected item
extern "C" void __fastcall FrameWnd_OnMenuSelect(BYTE* self, void*, UINT item, UINT flags, HMENU sysMenu) {
    BYTE* frame = Wnd_GetTopLevelFrame(self);
    BYTE* hook = F(BYTE*, self, 104);
    if (hook && CallVirt<BOOL>(hook, 124, item, flags, sysMenu)) return;
    if (flags == 0xFFFF) {                                         // menu closed
        F(DWORD, self, 36) &= ~64;                                 // WF_NOPOPMSG
        F(UINT, self, 144) = F(BOOL, frame, 80) ? 57346 : 57345;   // AFX_IDS_HELPMODEMESSAGE / IDLEMESSAGE
        SendMessageA(HWND_OF(self), 866, F(UINT, self, 144), 0);
        BYTE* bar = CallVirt<BYTE*>(self, 212);                    // GetMessageBar
        if (bar) UpdateWindow(HWND_OF(bar));
    } else {
        if (!item || (flags & (MF_SEPARATOR | MF_POPUP))) F(UINT, self, 144) = 0;
        else if (item >= 0xF000 && item < 0xF1F0) F(UINT, self, 144) = ((item - 0xF000) >> 4) + 0xEF00;   // ID_COMMAND_FROM_SC
        else if (item >= 0xFF00) F(UINT, self, 144) = 0xEF1F;      // AFX_IDS_MDICHILD
        else F(UINT, self, 144) = item;
        F(DWORD, frame, 36) |= 64;
    }
    if (F(UINT, self, 144) != F(UINT, self, 148) && Wnd_FromHandle(GetParent(HWND_OF(self))))
        PostMessageA(HWND_OF(self), 874, 0, 0);                    // WM_KICKIDLE
}

// [$L_638e4f] CFrameWnd::GetMessageString: the prompt part of the string
// resource
extern "C" void __fastcall FrameWnd_GetMessageString(BYTE* self, void*, UINT id, char** message) {
    char* buf = String_GetBuffer(message, 0, 255);
    if (Afx_LoadString(id, buf, 256)) {
        char* nl = CallC<char*>(FUN_620585, buf, 10);
        if (nl) *nl = 0;
    }
    String_ReleaseBuffer(message, 0, -1);
}

// [$L_638e93] CFrameWnd::OnPopMessageString
extern "C" LRESULT __fastcall FrameWnd_OnPopMessageString(BYTE* self, void*, WPARAM w, LPARAM l) {
    if (F(DWORD, self, 36) & 64) return 0;
    return SendMessageA(HWND_OF(self), 866, w, l);
}

// [$L_638eb6] CFrameWnd::OnSetMessageString: shows a prompt (lParam) or the
// prompt of a string id (wParam) in the message bar
extern "C" LRESULT __fastcall FrameWnd_OnSetMessageString(BYTE* self, void*, WPARAM w, LPARAM l) {
    UINT last = F(UINT, self, 148);
    F(DWORD, self, 36) &= ~64;
    UINT id = (UINT)w;
    BYTE* bar = CallVirt<BYTE*>(self, 212);                        // GetMessageBar
    if (bar) {
        char* s = g_afxPchNil;
        const char* text = 0;
        if (l) {
            text = (const char*)l;
        } else if (id) {
            if (id == 0xEF06 && F(void*, self, 156)) id = 0xF005;  // AFX_IDS_SCCLOSE in preview
            CallVirt(self, 196, id, &s);                           // GetMessageString
            text = s;
        }
        Wnd_SetWindowText(bar, 0, text);
        BYTE* frame = Wnd_GetParentFrame(bar);
        if (frame) {
            F(UINT, frame, 148) = id;
            F(UINT, frame, 144) = id;
        }
        String_Destruct(&s);
    }
    F(UINT, self, 148) = id;
    F(UINT, self, 144) = id;
    return last;
}

// [$L_638f7b] returns +76 (two arguments)
extern "C" void* __fastcall FrameWnd_Get76(BYTE* self, void*, int, int) { return self + 76; }

// [$L_638f81] CFrameWnd::GetMessageBar: the status bar
extern "C" BYTE* __fastcall FrameWnd_GetMessageBar(BYTE* self) {
    return Wnd_GetDescendantWindow(HWND_OF(self), 59393, TRUE);    // AFX_IDW_STATUS_BAR
}

// [$L_638f91] CFrameWnd::OnEnterIdle: shows the pending menu prompt
extern "C" void __fastcall FrameWnd_OnEnterIdle(BYTE* self, void*, UINT why, BYTE* who) {
    Wnd_OnEnterIdle(self, 0, why, who);
    if (why == MSGF_MENU && F(UINT, self, 144) != F(UINT, self, 148))
        FrameWnd_SetMessageTextId(self, 0, F(UINT, self, 144));
}

// [FUN_639064] CFrameWnd::GetControlBar(nID)
extern "C" BYTE* __fastcall FrameWnd_GetControlBar(BYTE* self, void*, UINT id) {
    if (!id) return 0;
    for (void** node = F(void**, self, 112); node; ) {
        BYTE* bar = (BYTE*)node[2];
        node = (void**)node[0];
        if (LOWORD(GetDlgCtrlID(HWND_OF(bar))) == id) return bar;
    }
    return 0;
}

// [$L_639098] CFrameWnd::OnUpdateControlBarMenu
extern "C" void __fastcall FrameWnd_OnUpdateControlBarMenu(BYTE* self, void*, BYTE* cmdUI) {
    BYTE* bar = FrameWnd_GetControlBar(self, 0, F(UINT, cmdUI, 4));
    if (bar) CallVirt(cmdUI, 4, (int)((Wnd_GetStyle(bar) >> 28) & 1));   // SetCheck(WS_VISIBLE)
    else F(BOOL, cmdUI, 28) = TRUE;                                // ContinueRouting
}

// [$L_6390cd] CFrameWnd::OnBarCheck: toggles the control bar
extern "C" BOOL __fastcall FrameWnd_OnBarCheck(BYTE* self, void*, UINT id) {
    BYTE* bar = FrameWnd_GetControlBar(self, 0, id);
    if (!bar) return FALSE;
    CallThis(FUN_638aa4, self, bar, (BOOL)((~Wnd_GetStyle(bar) >> 28) & 1), FALSE);
    return TRUE;
}

// [$L_639106] CFrameWnd::OnToolTipText: the tip part of the string
// resource
extern "C" BOOL __fastcall FrameWnd_OnToolTipText(BYTE* self, void*, UINT, NMHDR* hdr, LRESULT* result) {
    char* tip = g_afxPchNil;
    UINT id = (UINT)hdr->idFrom;
    BYTE* t = (BYTE*)hdr;
    if ((hdr->code == (UINT)-520 && (F(UINT, t, 100) & 1))        // TTN_NEEDTEXTA, TTF_IDISHWND
        || (hdr->code == (UINT)-530 && (F(UINT, t, 180) & 1)))    // TTN_NEEDTEXTW
        id = LOWORD(GetDlgCtrlID((HWND)(UINT_PTR)id));
    BOOL r = FALSE;
    if (id) {
        char full[256];
        if (!Afx_LoadString(id, full, 256)) goto done;
        Afx_ExtractSubString(&tip, full, 1, '\n');
    }
    if (hdr->code == (UINT)-520) lstrcpynA((char*)t + 16, tip, 80);
    else Afx_mbstowcsz((wchar_t*)(t + 16), tip, 80);
    *result = 0;
    SetWindowPos(hdr->hwndFrom, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);
    r = TRUE;
done:
    String_Destruct(&tip);
    return r;
}

// [$L_6391dd] CFrameWnd::OnUpdateKeyIndicator: caps, num, scroll lock, kana
extern "C" void __fastcall FrameWnd_OnUpdateKeyIndicator(BYTE* self, void*, BYTE* cmdUI) {
    UINT flag = 1;
    int vk;
    switch (F(UINT, cmdUI, 4)) {
    case 59137: vk = VK_CAPITAL; break;                            // ID_INDICATOR_CAPS
    case 59138: vk = VK_NUMLOCK; break;
    case 59139: vk = VK_SCROLL; break;
    case 59142:                                                    // ID_INDICATOR_KANA
        vk = VK_KANA;
        if (!g_afxWin4) flag = 0x8000;
        break;
    default:
        F(BOOL, cmdUI, 28) = TRUE;                                 // ContinueRouting
        return;
    }
    CallVirt(cmdUI, 0, (BOOL)((int)GetKeyState(vk) & flag));      // Enable
}

// [$L_63923e] CFrameWnd::OnUpdateContextHelp
extern "C" void __fastcall FrameWnd_OnUpdateContextHelp(BYTE* self, void*, BYTE* cmdUI) {
    if (CallC<BYTE*>(FUN_61c4c1) == self) CallVirt(cmdUI, 4, (int)(F(BOOL, self, 80) != 0));   // SetCheck
    else F(BOOL, cmdUI, 28) = TRUE;
}

// [FUN_6392b1] CFrameWnd::UpdateFrameTitleForDocument
extern "C" void __fastcall FrameWnd_UpdateFrameTitleForDocument(BYTE* self, void*, const char* doc) {
    char text[516];
    const char* title = F(const char*, self, 172);
    if (Wnd_GetStyle(self) & 0x4000) {                             // FWS_PREFIXTITLE
        text[0] = 0;
        if (doc) {
            lstrcpyA(text, doc);
            if (F(int, self, 64) > 0) wsprintfA(text + lstrlenA(text), &g_fmtWindowNumber, F(int, self, 64));
            lstrcatA(text, &g_titleSeparator);
        }
        lstrcatA(text, F(const char*, self, 172));
    } else {
        lstrcpyA(text, title);
        if (doc) {
            lstrcatA(text, &g_titleSeparator);
            lstrcatA(text, doc);
            if (F(int, self, 64) > 0) wsprintfA(text + lstrlenA(text), &g_fmtWindowNumber, F(int, self, 64));
        }
    }
    Afx_SetWindowText(HWND_OF(self), text);
}

// [$L_63926d] CFrameWnd::OnUpdateFrameTitle
extern "C" void __fastcall FrameWnd_OnUpdateFrameTitle(BYTE* self, void*, BOOL addToTitle) {
    if (!(Wnd_GetStyle(self) & 0x8000)) return;                    // FWS_ADDTOTITLE
    BYTE* hook = F(BYTE*, self, 104);
    if (hook && CallVirt<BOOL>(hook, 104)) return;                 // OnUpdateFrameTitle
    BYTE* doc = CallVirt<BYTE*>(self, 188);                        // GetActiveDocument
    FrameWnd_UpdateFrameTitleForDocument(self, 0, addToTitle && doc ? F(const char*, doc, 28) : 0);
}

// [$L_6395c9] sets idle flag 1 (+184) and +168
extern "C" void __fastcall FrameWnd_Set168Idle1(BYTE* self, void*, void* v) {
    F(DWORD, self, 184) |= 1;
    F(void*, self, 168) = v;
}

// [$L_63964c] returns this
extern "C" BYTE* __fastcall Object_Self(BYTE* self) { return self; }

// [$L_63964f] CFrameWnd::RecalcLayout
extern "C" void __fastcall FrameWnd_RecalcLayout(BYTE* self, void*, BOOL notify) {
    if (F(BOOL, self, 176)) return;
    F(BOOL, self, 176) = TRUE;
    if (F(DWORD, self, 184) & 4) notify = TRUE;                    // idleNotify
    F(DWORD, self, 184) &= ~12;
    if (notify && F(BYTE*, self, 104)) CallVirt(F(BYTE*, self, 104), 80);   // OnRecalcLayout
    if (Wnd_GetStyle(self) & 0x2000) {                             // FWS_SNAPTOBARS
        RECT r = { 0, 0, 32767, 32767 };
        Wnd_RepositionBars(self, 0, 0, 0xFFFF, 59648, 1, &r, &r, FALSE);
        Wnd_RepositionBars(self, 0, 0, 0xFFFF, 59648, 2, &F(RECT, self, 88), &r, TRUE);
        CallVirt(self, 96, &r, 0);                                 // CalcWindowRect
        Wnd_SetWindowPos(self, 0, 0, 0, 0, r.right - r.left, r.bottom - r.top,
                         SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
    } else {
        Wnd_RepositionBars(self, 0, 0, 0xFFFF, 59648, 2, &F(RECT, self, 88), 0, TRUE);
    }
    F(BOOL, self, 176) = FALSE;
}

// [$L_63973c] CFrameWnd::NegotiateBorderSpace
extern "C" BOOL __fastcall FrameWnd_NegotiateBorderSpace(BYTE* self, void*, UINT cmd, RECT* border) {
    RECT* mine = &F(RECT, self, 88);
    switch (cmd) {
    case 1:                                                        // borderGet
        Wnd_RepositionBars(self, 0, 0, 0xFFFF, 59648, 1, border, 0, TRUE);
        break;
    case 3:                                                        // borderSet
        if (!border) {
            if (!mine->left && !mine->right && !mine->top && !mine->bottom) return FALSE;
            SetRectEmpty(mine);
        } else {
            if (EqualRect(mine, border)) return FALSE;
            CopyRect(mine, border);
        }
        break;
    }
    return TRUE;
}

// [$L_6397b6] CFrameWnd::OnSize
extern "C" void __fastcall FrameWnd_OnSize(BYTE* self, void*, UINT type, int cx, int cy) {
    Wnd_Default(self);
    if (type != SIZE_MINIMIZED) CallVirt(self, 200, TRUE);        // RecalcLayout
}

// [$L_6397d5] CFrameWnd::OnEraseBkgnd: nothing to erase under the view
extern "C" BOOL __fastcall FrameWnd_OnEraseBkgnd(BYTE* self, void*, BYTE* dc) {
    if (F(BYTE*, self, 152)) return TRUE;
    return (BOOL)Wnd_Default(self);
}

// [$L_6397eb] CFrameWnd::OnRegisteredMouseWheel: MSWHEEL_ROLLMSG as
// WM_MOUSEWHEEL to the focus window and its parents
extern "C" LRESULT __fastcall FrameWnd_OnRegisteredMouseWheel(BYTE* self, void*, WPARAM w, LPARAM l) {
    WORD keys = (GetKeyState(VK_CONTROL) < 0 ? MK_CONTROL : 0) | (GetKeyState(VK_SHIFT) < 0 ? MK_SHIFT : 0);
    HWND focus = GetFocus();
    HWND desktop = GetDesktopWindow();
    if (!focus) return SendMessageA(HWND_OF(self), 522, (w << 16) | keys, l);
    LRESULT r;
    do {
        r = SendMessageA(focus, 522, (w << 16) | keys, l);
        focus = GetParent(focus);
    } while (!r && focus && focus != desktop);
    return r;
}

// [FUN_6398de] CFrameWnd::BringToTop
extern "C" void __fastcall FrameWnd_BringToTop(BYTE* self, void*, int show) {
    if (show == SW_HIDE || show == SW_MINIMIZE || show == SW_SHOWMINNOACTIVE || show == SW_SHOWNA
        || show == SW_SHOWNOACTIVATE)
        return;
    BringWindowToTop(GetLastActivePopup(HWND_OF(self)));
}

// [$L_63988e] CFrameWnd::ActivateFrame
extern "C" void __fastcall FrameWnd_ActivateFrame(BYTE* self, void*, int show) {
    if (show == -1) {
        if (!IsWindowVisible(HWND_OF(self))) show = SW_SHOWNORMAL;
        else if (IsIconic(HWND_OF(self))) show = SW_RESTORE;
    }
    FrameWnd_BringToTop(self, 0, show);
    if (show != -1) {
        Wnd_ShowWindow(self, 0, show);
        FrameWnd_BringToTop(self, 0, show);
    }
}

// [FUN_63990d] CControlBar::GetDockingFrame
extern "C" BYTE* __fastcall ControlBar_GetDockingFrame(BYTE* self) {
    BYTE* frame = Wnd_GetParentFrame(self);
    return frame ? frame : F(BYTE*, self, 108);                    // m_pDockSite
}

// [FUN_63991e] CControlBar::IsFloating
extern "C" BOOL __fastcall ControlBar_IsFloating(BYTE* self) {
    if (CallVirt<BOOL>(self, 208)) return F(BOOL, self, 120);      // IsDockBar: m_bFloating
    BYTE* dock = F(BYTE*, self, 112);
    return dock && F(BOOL, dock, 120);
}

// [$L_639a7e] CWinApp's deleting destructor
extern "C" BYTE* __fastcall WinApp_DeletingDestruct(BYTE* self, void*, int flags) {
    WinApp_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [$L_639a9a] CWinApp::InitApplication: takes over the static document
// manager
extern "C" BOOL __fastcall WinApp_InitApplication(BYTE* self) {
    if (g_staticDocManager) {
        if (!F(BYTE*, self, 128)) F(BYTE*, self, 128) = g_staticDocManager;
        g_staticDocManager = 0;
    }
    if (F(BYTE*, self, 128)) CallVirt(F(BYTE*, self, 128), 12, (void*)0);   // AddDocTemplate(NULL)
    else g_docManagerStaticInit = FALSE;
    return TRUE;
}

// [$L_639ada] returns TRUE
extern "C" BOOL __fastcall Object_True(BYTE* self) { return TRUE; }

// ---------------------------------------------------------------------------
// static objects: the CFixedAlloc pools, the static exceptions, afxData,
// process and thread locals; CreateObject and deleting destructors of the
// temporary map classes; runtime class getters

extern "C" void FUN_61ca2b(); KING_ALIAS("_FUN_61ca2b", "@FixedAlloc_Construct@16")                            // CFixedAlloc::CFixedAlloc(nAllocSize, nBlockSize)
extern "C" void $L_61ca53(); KING_ALIAS("_$L_61ca53", "@FixedAlloc_Destruct@4")                             // CFixedAlloc::~CFixedAlloc
extern "C" void FUN_61c749(); KING_ALIAS("_FUN_61c749", "@ResourceException_Construct@16")                            // CResourceException::CResourceException
extern "C" void FUN_61c78f(); KING_ALIAS("_FUN_61c78f", "@ResourceException_Destruct@4")                            // ~CResourceException
extern "C" void FUN_61c796(); KING_ALIAS("_FUN_61c796", "@UserException_Construct@16")                            // CUserException::CUserException
extern "C" void FUN_61c7dc(); KING_ALIAS("_FUN_61c7dc", "@UserException_Destruct@4")                            // ~CUserException
extern "C" void FUN_61c8e8(); KING_ALIAS("_FUN_61c8e8", "@MemoryException_Construct@16")                            // CMemoryException::CMemoryException
extern "C" void FUN_61c92e(); KING_ALIAS("_FUN_61c92e", "@MemoryException_Destruct@4")                            // ~CMemoryException
extern "C" void FUN_61c935(); KING_ALIAS("_FUN_61c935", "@NotSupportedException_Construct@16")                            // CNotSupportedException::CNotSupportedException
extern "C" void FUN_61c97b(); KING_ALIAS("_FUN_61c97b", "@NotSupportedException_Destruct@4")                            // ~CNotSupportedException
KING_VAR(AsmLabel, g_allocTempWnd, 72d970)
KING_VAR(AsmLabel, g_allocTempDc, 72dce0)
KING_VAR(AsmLabel, g_allocTempGdi, 72dcb8)
KING_VAR(AsmLabel, g_allocTempMenu, 72e0f0)
KING_VAR(BYTE, g_allocTempWndDone, 72c168)
KING_VAR(BYTE, g_allocTempDcGdiDone, 72db80)
KING_VAR(BYTE, g_allocTempMenuDone, 72e0e8)
KING_VAR(AsmLabel, g_resourceException, 72dc20)
KING_VAR(AsmLabel, g_userException, 72db88)
KING_VAR(AsmLabel, g_memoryException, 72dda8)
KING_VAR(AsmLabel, g_notSupportedException, 72dd10)
KING_VAR(AsmLabel, g_processLocal72c15c, 72c15c)
KING_VAR(AsmLabel, g_processLocal72db78, 72db78)
KING_VAR(AsmLabel, g_threadLocal72db7c, 72db7c)
KING_VAR(AsmLabel, g_threadLocal72dd08, 72dd08)
KING_VAR(AsmLabel, g_class6533f8, 6533f8)
KING_VAR(AsmLabel, g_class653548, 653548)
KING_VAR(AsmLabel, g_class653730, 653730)
KING_VAR(AsmLabel, g_class653708, 653708)
KING_VAR(AsmLabel, g_class653cd8, 653cd8)
KING_VAR(AsmLabel, g_class654378, 654378)
KING_VAR(AsmLabel, g_class654350, 654350)
KING_VAR(AsmLabel, g_class654330, 654330)
KING_VAR(AsmLabel, g_class654308, 654308)
KING_VAR(AsmLabel, g_class6542e8, 6542e8)
KING_VAR(AsmLabel, g_class6542c8, 6542c8)
KING_VAR(AsmLabel, g_class6542a0, 6542a0)
KING_VAR(AsmLabel, g_class654520, 654520)
KING_VAR(AsmLabel, g_class6544f0, 6544f0)
KING_VAR(AsmLabel, g_class6545f0, 6545f0)
KING_VAR(AsmLabel, g_class654650, 654650)
KING_VAR(AsmLabel, g_class654628, 654628)
KING_VAR(AsmLabel, g_class6546a0, 6546a0)

// [$L_636118] returns 0
extern "C" int __cdecl Afx_Zero() { return 0; }

// [FUN_636153] constructs the CFixedAlloc of strings up to 64 characters
extern "C" void __cdecl AllocString64_Construct() { CallThis(FUN_61ca2b, g_alloc64, 80, 64); }

// [$L_63616e] destroys it at exit
extern "C" void __cdecl AllocString64_Term() { CallThis($L_61ca53, g_alloc64); }

// [$L_636162] registers its destruction
extern "C" void __cdecl AllocString64_AtExit() { atexit(AllocString64_Term); }

// [$L_636149] its static initializer
extern "C" void __cdecl AllocString64_Init() { AllocString64_Construct(); AllocString64_AtExit(); }

// [FUN_636182] constructs the CFixedAlloc of strings up to 128 characters
extern "C" void __cdecl AllocString128_Construct() { CallThis(FUN_61ca2b, g_alloc128, 144, 64); }

// [$L_6361a0] destroys it at exit
extern "C" void __cdecl AllocString128_Term() { CallThis($L_61ca53, g_alloc128); }

// [$L_636194] registers its destruction
extern "C" void __cdecl AllocString128_AtExit() { atexit(AllocString128_Term); }

// [$L_636178] its static initializer
extern "C" void __cdecl AllocString128_Init() { AllocString128_Construct(); AllocString128_AtExit(); }

// [FUN_6361b4] constructs the CFixedAlloc of strings up to 256 characters
extern "C" void __cdecl AllocString256_Construct() { CallThis(FUN_61ca2b, g_alloc256, 272, 64); }

// [$L_6361d2] destroys it at exit
extern "C" void __cdecl AllocString256_Term() { CallThis($L_61ca53, g_alloc256); }

// [$L_6361c6] registers its destruction
extern "C" void __cdecl AllocString256_AtExit() { atexit(AllocString256_Term); }

// [$L_6361aa] its static initializer
extern "C" void __cdecl AllocString256_Init() { AllocString256_Construct(); AllocString256_AtExit(); }

// [FUN_6361e6] constructs the CFixedAlloc of strings up to 512 characters
extern "C" void __cdecl AllocString512_Construct() { CallThis(FUN_61ca2b, g_alloc512, 528, 64); }

// [$L_636204] destroys it at exit
extern "C" void __cdecl AllocString512_Term() { CallThis($L_61ca53, g_alloc512); }

// [$L_6361f8] registers its destruction
extern "C" void __cdecl AllocString512_AtExit() { atexit(AllocString512_Term); }

// [$L_6361dc] its static initializer
extern "C" void __cdecl AllocString512_Init() { AllocString512_Construct(); AllocString512_AtExit(); }

// [FUN_639fcd] constructs the CFixedAlloc of the temporary Wnd objects
extern "C" void __cdecl AllocTempWnd_Construct() { CallThis(FUN_61ca2b, g_allocTempWnd, 60, 64); }

// [$L_639fe8] destroys it at exit
extern "C" void __cdecl AllocTempWnd_Term() {
    if (g_allocTempWndDone & 1) return;
    g_allocTempWndDone |= 1;
    CallThis($L_61ca53, g_allocTempWnd);
}

// [$L_639fdc] registers its destruction
extern "C" void __cdecl AllocTempWnd_AtExit() { atexit(AllocTempWnd_Term); }

// [$L_639fc3] its static initializer
extern "C" void __cdecl AllocTempWnd_Init() { AllocTempWnd_Construct(); AllocTempWnd_AtExit(); }

// [FUN_63a575] constructs the CFixedAlloc of the temporary Dc objects
extern "C" void __cdecl AllocTempDc_Construct() { CallThis(FUN_61ca2b, g_allocTempDc, 16, 64); }

// [$L_63a590] destroys it at exit
extern "C" void __cdecl AllocTempDc_Term() {
    if (g_allocTempDcGdiDone & 1) return;
    g_allocTempDcGdiDone |= 1;
    CallThis($L_61ca53, g_allocTempDc);
}

// [$L_63a584] registers its destruction
extern "C" void __cdecl AllocTempDc_AtExit() { atexit(AllocTempDc_Term); }

// [$L_63a56b] its static initializer
extern "C" void __cdecl AllocTempDc_Init() { AllocTempDc_Construct(); AllocTempDc_AtExit(); }

// [FUN_63a5b5] constructs the CFixedAlloc of the temporary Gdi objects
extern "C" void __cdecl AllocTempGdi_Construct() { CallThis(FUN_61ca2b, g_allocTempGdi, 8, 64); }

// [$L_63a5d0] destroys it at exit
extern "C" void __cdecl AllocTempGdi_Term() {
    if (g_allocTempDcGdiDone & 2) return;
    g_allocTempDcGdiDone |= 2;
    CallThis($L_61ca53, g_allocTempGdi);
}

// [$L_63a5c4] registers its destruction
extern "C" void __cdecl AllocTempGdi_AtExit() { atexit(AllocTempGdi_Term); }

// [$L_63a5ab] its static initializer
extern "C" void __cdecl AllocTempGdi_Init() { AllocTempGdi_Construct(); AllocTempGdi_AtExit(); }

// [FUN_63b294] constructs the CFixedAlloc of the temporary Menu objects
extern "C" void __cdecl AllocTempMenu_Construct() { CallThis(FUN_61ca2b, g_allocTempMenu, 8, 64); }

// [$L_63b2af] destroys it at exit
extern "C" void __cdecl AllocTempMenu_Term() {
    if (g_allocTempMenuDone & 1) return;
    g_allocTempMenuDone |= 1;
    CallThis($L_61ca53, g_allocTempMenu);
}

// [$L_63b2a3] registers its destruction
extern "C" void __cdecl AllocTempMenu_AtExit() { atexit(AllocTempMenu_Term); }

// [$L_63b28a] its static initializer
extern "C" void __cdecl AllocTempMenu_Init() { AllocTempMenu_Construct(); AllocTempMenu_AtExit(); }

// [FUN_63a40c] constructs the static CResourceException (not auto-deleted)
extern "C" void __cdecl StaticResourceException_Construct() { CallThis(FUN_61c749, g_resourceException, FALSE, 61474); }

// [$L_63a42a] destroys it at exit
extern "C" void __cdecl StaticResourceException_Term() { CallThis(FUN_61c78f, g_resourceException); }

// [$L_63a41e] registers its destruction
extern "C" void __cdecl StaticResourceException_AtExit() { atexit(StaticResourceException_Term); }

// [$L_63a402] its static initializer
extern "C" void __cdecl StaticResourceException_Init() { StaticResourceException_Construct(); StaticResourceException_AtExit(); }

// [FUN_63a444] constructs the static CUserException (not auto-deleted)
extern "C" void __cdecl StaticUserException_Construct() { CallThis(FUN_61c796, g_userException, FALSE, 61476); }

// [$L_63a462] destroys it at exit
extern "C" void __cdecl StaticUserException_Term() { CallThis(FUN_61c7dc, g_userException); }

// [$L_63a456] registers its destruction
extern "C" void __cdecl StaticUserException_AtExit() { atexit(StaticUserException_Term); }

// [$L_63a43a] its static initializer
extern "C" void __cdecl StaticUserException_Init() { StaticUserException_Construct(); StaticUserException_AtExit(); }

// [FUN_63a842] constructs the static CMemoryException (not auto-deleted)
extern "C" void __cdecl StaticMemoryException_Construct() { CallThis(FUN_61c8e8, g_memoryException, FALSE, 61475); }

// [$L_63a860] destroys it at exit
extern "C" void __cdecl StaticMemoryException_Term() { CallThis(FUN_61c92e, g_memoryException); }

// [$L_63a854] registers its destruction
extern "C" void __cdecl StaticMemoryException_AtExit() { atexit(StaticMemoryException_Term); }

// [$L_63a838] its static initializer
extern "C" void __cdecl StaticMemoryException_Init() { StaticMemoryException_Construct(); StaticMemoryException_AtExit(); }

// [FUN_63a87a] constructs the static CNotSupportedException (not auto-deleted)
extern "C" void __cdecl StaticNotSupportedException_Construct() { CallThis(FUN_61c935, g_notSupportedException, FALSE, 61473); }

// [$L_63a898] destroys it at exit
extern "C" void __cdecl StaticNotSupportedException_Term() { CallThis(FUN_61c97b, g_notSupportedException); }

// [$L_63a88c] registers its destruction
extern "C" void __cdecl StaticNotSupportedException_AtExit() { atexit(StaticNotSupportedException_Term); }

// [$L_63a870] its static initializer
extern "C" void __cdecl StaticNotSupportedException_Init() { StaticNotSupportedException_Construct(); StaticNotSupportedException_AtExit(); }

// [$L_63ae4a] CProcessLocalObject::~CProcessLocalObject
extern "C" void __fastcall ProcessLocal_Destruct(void** self) {
    if (*self) CallVirt(*self, 0, 1);                              // deleting destructor
}

// [$L_639ca1] CProcessLocal<>::~CProcessLocal
extern "C" void __fastcall ProcessLocal_DestructT(void** self) { ProcessLocal_Destruct(self); }

// [$L_63a795] CThreadLocal<>::~CThreadLocal
extern "C" void __fastcall ThreadLocal_DestructT(int* self) { ThreadLocal_Destruct(self); }

// [FUN_639c8a] constructs ProcessLocal72c15c (nothing to do)
extern "C" void __cdecl LocalProcessLocal72c15c_Construct() {}

// [$L_639c97] destroys it at exit
extern "C" void __cdecl LocalProcessLocal72c15c_Term() { ProcessLocal_DestructT((void**)&g_processLocal72c15c); }

// [$L_639c8b] registers its destruction
extern "C" void __cdecl LocalProcessLocal72c15c_AtExit() { atexit(LocalProcessLocal72c15c_Term); }

// [$L_639c80] its static initializer
extern "C" void __cdecl LocalProcessLocal72c15c_Init() { LocalProcessLocal72c15c_Construct(); LocalProcessLocal72c15c_AtExit(); }

// [FUN_63a12f] constructs ThreadState (nothing to do)
extern "C" void __cdecl LocalThreadState_Construct() {}

// [$L_63a13c] destroys it at exit
extern "C" void __cdecl LocalThreadState_Term() { ThreadLocal_DestructT((int*)&g_threadLocal72db7c); }

// [$L_63a130] registers its destruction
extern "C" void __cdecl LocalThreadState_AtExit() { atexit(LocalThreadState_Term); }

// [$L_63a125] its static initializer
extern "C" void __cdecl LocalThreadState_Init() { LocalThreadState_Construct(); LocalThreadState_AtExit(); }

// [FUN_63a32b] constructs ProcessLocal72db78 (nothing to do)
extern "C" void __cdecl LocalProcessLocal72db78_Construct() {}

// [$L_63a338] destroys it at exit
extern "C" void __cdecl LocalProcessLocal72db78_Term() { ProcessLocal_DestructT((void**)&g_processLocal72db78); }

// [$L_63a32c] registers its destruction
extern "C" void __cdecl LocalProcessLocal72db78_AtExit() { atexit(LocalProcessLocal72db78_Term); }

// [$L_63a321] its static initializer
extern "C" void __cdecl LocalProcessLocal72db78_Init() { LocalProcessLocal72db78_Construct(); LocalProcessLocal72db78_AtExit(); }

// [FUN_63a77e] constructs ThreadLocal72dd08 (nothing to do)
extern "C" void __cdecl LocalThreadLocal72dd08_Construct() {}

// [$L_63a78b] destroys it at exit
extern "C" void __cdecl LocalThreadLocal72dd08_Term() { ThreadLocal_DestructT((int*)&g_threadLocal72dd08); }

// [$L_63a77f] registers its destruction
extern "C" void __cdecl LocalThreadLocal72dd08_AtExit() { atexit(LocalThreadLocal72dd08_Term); }

// [$L_63a774] its static initializer
extern "C" void __cdecl LocalThreadLocal72dd08_Init() { LocalThreadLocal72dd08_Construct(); LocalThreadLocal72dd08_AtExit(); }

// [FUN_63a7a4] constructs Ctl3dState (nothing to do)
extern "C" void __cdecl LocalCtl3dState_Construct() {}

// [$L_63a7b1] destroys it at exit
extern "C" void __cdecl LocalCtl3dState_Term() { ProcessLocal_DestructT((void**)&g_ctl3dState); }

// [$L_63a7a5] registers its destruction
extern "C" void __cdecl LocalCtl3dState_AtExit() { atexit(LocalCtl3dState_Term); }

// [$L_63a79a] its static initializer
extern "C" void __cdecl LocalCtl3dState_Init() { LocalCtl3dState_Construct(); LocalCtl3dState_AtExit(); }

// [$L_639c7a] GetRuntimeClass ($L_6533f8)
extern "C" void* __fastcall Class6533f8_Get(BYTE* self) { return g_class6533f8; }

// [$L_639da6] GetRuntimeClass ($L_653548)
extern "C" void* __fastcall Class653548_Get(BYTE* self) { return g_class653548; }

// [$L_639f52] GetRuntimeClass ($L_653730)
extern "C" void* __fastcall Class653730_Get(BYTE* self) { return g_class653730; }

// [$L_639fbd] GetRuntimeClass ($L_653708)
extern "C" void* __fastcall Class653708_Get(BYTE* self) { return g_class653708; }

// [$L_63a069] GetRuntimeClass ($L_653cd8)
extern "C" void* __fastcall Class653cd8_Get(BYTE* self) { return g_class653cd8; }

// [$L_63a3fc] GetRuntimeClass ($L_654378)
extern "C" void* __fastcall Class654378_Get(BYTE* self) { return g_class654378; }

// [$L_63a434] GetRuntimeClass ($L_654350)
extern "C" void* __fastcall Class654350_Get(BYTE* self) { return g_class654350; }

// [$L_63a483] GetRuntimeClass ($L_654330)
extern "C" void* __fastcall Class654330_Get(BYTE* self) { return g_class654330; }

// [$L_63a4a3] GetRuntimeClass ($L_654308)
extern "C" void* __fastcall Class654308_Get(BYTE* self) { return g_class654308; }

// [$L_63a4a9] GetRuntimeClass ($L_6542e8)
extern "C" void* __fastcall Class6542e8_Get(BYTE* self) { return g_class6542e8; }

// [$L_63a4fa] GetRuntimeClass ($L_6542c8)
extern "C" void* __fastcall Class6542c8_Get(BYTE* self) { return g_class6542c8; }

// [$L_63a565] GetRuntimeClass ($L_6542a0)
extern "C" void* __fastcall Class6542a0_Get(BYTE* self) { return g_class6542a0; }

// [$L_63a832] GetRuntimeClass ($L_654520)
extern "C" void* __fastcall Class654520_Get(BYTE* self) { return g_class654520; }

// [$L_63a86a] GetRuntimeClass ($L_6544f0)
extern "C" void* __fastcall Class6544f0_Get(BYTE* self) { return g_class6544f0; }

// [$L_63b1c6] GetRuntimeClass ($L_6545f0)
extern "C" void* __fastcall Class6545f0_Get(BYTE* self) { return g_class6545f0; }

// [$L_63b219] GetRuntimeClass ($L_654650)
extern "C" void* __fastcall Class654650_Get(BYTE* self) { return g_class654650; }

// [$L_63b284] GetRuntimeClass ($L_654628)
extern "C" void* __fastcall Class654628_Get(BYTE* self) { return g_class654628; }

// [$L_63b2ca] GetRuntimeClass ($L_6546a0)
extern "C" void* __fastcall Class6546a0_Get(BYTE* self) { return g_class6546a0; }

KING_VAR(AsmLabel, g_tempWndVtbl, 653c1c)
KING_VAR(AsmLabel, g_gdiObjectVtbl, 654420)
KING_VAR(AsmLabel, g_tempDcVtbl, 654430)
KING_VAR(AsmLabel, g_tempGdiVtbl, 6544a8)
KING_VAR(AsmLabel, g_menuVtbl, 654674)
KING_VAR(AsmLabel, g_tempMenuVtbl, 65468c)
KING_VAR(AsmLabel, g_ctl3dStateVtbl, 6535f0)
KING_VAR(AsmLabel, g_ctl3dThreadVtbl, 6535f8)

// [$L_639d1b] CWinThread's deleting destructor
extern "C" BYTE* __fastcall WinThread_DeletingDestruct(BYTE* self, void*, int flags) {
    WinThread_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [FUN_63a5eb] _AFX_CTL3D_STATE::~_AFX_CTL3D_STATE: unregisters and frees
// CTL3D32.DLL
extern "C" void __fastcall Ctl3dState_Destruct(BYTE* self) {
    F(void*, self, 0) = g_ctl3dStateVtbl;
    typedef BOOL (WINAPI* UnregisterFn)(HINSTANCE);
    if (F(void*, self, 16)) F(UnregisterFn, self, 16)(0);          // Ctl3dUnregister
    if (F(HMODULE, self, 8)) FreeLibrary(F(HMODULE, self, 8));
}

// [$L_639e0a] its deleting destructor
extern "C" BYTE* __fastcall Ctl3dState_DeletingDestruct(BYTE* self, void*, int flags) {
    Ctl3dState_Destruct(self);
    if (flags & 1) NoTrack_Delete(self);
    return self;
}

// [FUN_63a62c] _AFX_CTL3D_THREAD::~_AFX_CTL3D_THREAD: ends the automatic
// subclassing
extern "C" void __fastcall Ctl3dThread_Destruct(BYTE* self) {
    F(void*, self, 0) = g_ctl3dThreadVtbl;
    BYTE* state = (BYTE*)g_ctl3dState;                             // m_pObject of the process local
    if (state && F(void*, state, 24)) F(VoidFn, state, 24)();     // Ctl3dUnAutoSubclass
}

// [$L_639e25] its deleting destructor
extern "C" BYTE* __fastcall Ctl3dThread_DeletingDestruct(BYTE* self, void*, int flags) {
    Ctl3dThread_Destruct(self);
    if (flags & 1) NoTrack_Delete(self);
    return self;
}

// [$L_639f20] CWnd's CreateObject
extern "C" BYTE* __cdecl Wnd_CreateObject() {
    BYTE* p = (BYTE*)operator new(60);
    return p ? Wnd_Construct(p) : 0;
}

// [$L_639f58] CTempWnd's CreateObject (from its CFixedAlloc)
extern "C" BYTE* __cdecl TempWnd_CreateObject() {
    BYTE* p = CallThis<BYTE*>(FUN_61ca8f, g_allocTempWnd);
    if (!p) return 0;
    Wnd_Construct(p);
    F(void*, p, 0) = g_tempWndVtbl;
    return p;
}

// [FUN_639fb8] CTempWnd::~CTempWnd
extern "C" void __fastcall TempWnd_Destruct(BYTE* self) { Wnd_Destruct(self); }

// [$L_639f98] CTempWnd's deleting destructor (back to its CFixedAlloc)
extern "C" BYTE* __fastcall TempWnd_DeletingDestruct(BYTE* self, void*, int flags) {
    TempWnd_Destruct(self);
    if (flags & 1) CallThis(FUN_61cb1e, g_allocTempWnd, self);
    return self;
}

// [$L_63a034] CFrameWnd's CreateObject
extern "C" BYTE* __cdecl FrameWnd_CreateObject() {
    BYTE* p = (BYTE*)operator new(188);
    return p ? FrameWnd_Construct(p) : 0;
}

// [FUN_63a0a1] _AFX_THREAD_STATE::~_AFX_THREAD_STATE: the tool tip
// window (+204), the message and CBT hooks (+48, +44), the safety pool (+12)
extern "C" void __fastcall ThreadState_Destruct(BYTE* self) {
    F(void*, self, 0) = g_threadStateVtbl;
    if (F(BYTE*, self, 204)) {
        CallVirt(F(BYTE*, self, 204), 88);                         // DestroyWindow
        if (F(BYTE*, self, 204)) CallVirt(F(BYTE*, self, 204), 4, 1);
    }
    if (F(HHOOK, self, 48)) UnhookWindowsHookEx(F(HHOOK, self, 48));
    if (F(HHOOK, self, 44)) UnhookWindowsHookEx(F(HHOOK, self, 44));
    if (F(void*, self, 12)) free(F(void*, self, 12));
}

// [$L_63a086] its deleting destructor
extern "C" BYTE* __fastcall ThreadState_DeletingDestruct(BYTE* self, void*, int flags) {
    ThreadState_Destruct(self);
    if (flags & 1) NoTrack_Delete(self);
    return self;
}

// [FUN_63a1c4] AFX_MODULE_STATE::~AFX_MODULE_STATE
extern "C" void __fastcall ModuleState_Destruct(BYTE* self) {
    F(void*, self, 0) = g_moduleStateVtbl;
    if (F(BYTE*, self, 4164)) CallVirt(F(BYTE*, self, 4164), 0, 1);
    if (F(BYTE*, self, 4204)) {
        CallVirt(F(BYTE*, self, 4204), 12, self + 4168);
        if (F(BYTE*, self, 4204)) CallVirt(F(BYTE*, self, 4204), 4, 1);
    }
    ThreadLocal_Destruct((int*)(self + 4208));                     // m_thread
}

// [$L_63a1a9] its deleting destructor
extern "C" BYTE* __fastcall ModuleState_DeletingDestruct(BYTE* self, void*, int flags) {
    ModuleState_Destruct(self);
    if (flags & 1) NoTrack_Delete(self);
    return self;
}

// [FUN_63a3f7] _AFX_BASE_MODULE_STATE::~_AFX_BASE_MODULE_STATE
extern "C" void __fastcall BaseModuleState_Destruct(BYTE* self) { ModuleState_Destruct(self); }

// [$L_63a3dc] its deleting destructor
extern "C" BYTE* __fastcall BaseModuleState_DeletingDestruct(BYTE* self, void*, int flags) {
    BaseModuleState_Destruct(self);
    if (flags & 1) NoTrack_Delete(self);
    return self;
}

// [FUN_63a273] AFX_MODULE_THREAD_STATE::~AFX_MODULE_THREAD_STATE: the handle
// maps (+20..+36) and the three lists (+48, +52, +56; +56 owns its items)
extern "C" void __fastcall ModuleThreadState_Destruct(BYTE* self) {
    F(void*, self, 0) = g_moduleThreadStateVtbl;
    for (int off = 20; off <= 36; off += 4)
        if (F(BYTE*, self, off)) CallVirt(F(BYTE*, self, off), 0, 1);
    if (F(BYTE*, self, 56))
        while (F(int, F(BYTE*, self, 56), 12)) operator delete(PtrList_RemoveHead(F(BYTE*, self, 56)));
    if (F(BYTE*, self, 48)) CallVirt(F(BYTE*, self, 48), 4, 1);
    if (F(BYTE*, self, 52)) CallVirt(F(BYTE*, self, 52), 4, 1);
    if (F(BYTE*, self, 56)) CallVirt(F(BYTE*, self, 56), 4, 1);
}

// [$L_63a258] its deleting destructor
extern "C" BYTE* __fastcall ModuleThreadState_DeletingDestruct(BYTE* self, void*, int flags) {
    ModuleThreadState_Destruct(self);
    if (flags & 1) NoTrack_Delete(self);
    return self;
}

// [FUN_63ac3a] a destructor with nothing to do
extern "C" void __fastcall NoTrack_EmptyDestruct(BYTE* self) {}

// [$L_63ac1f] its deleting destructor
extern "C" BYTE* __fastcall NoTrack_EmptyDeletingDestruct(BYTE* self, void*, int flags) {
    NoTrack_EmptyDestruct(self);
    if (flags & 1) NoTrack_Delete(self);
    return self;
}

// [$L_63a46c] CDC's CreateObject
extern "C" BYTE* __cdecl Dc_CreateObject() {
    BYTE* p = (BYTE*)operator new(16);
    return p ? Cdc_Construct(p) : 0;
}

// [$L_63a489] CGdiObject's CreateObject
extern "C" BYTE* __cdecl GdiObject_CreateObject() {
    BYTE* p = (BYTE*)operator new(8);
    if (!p) return 0;
    F(void*, p, 0) = g_gdiObjectVtbl;
    F(HGDIOBJ, p, 4) = 0;
    return p;
}

// [$L_63a4af] CTempDC's CreateObject
extern "C" BYTE* __cdecl TempDc_CreateObject() {
    BYTE* p = CallThis<BYTE*>(FUN_61ca8f, g_allocTempDc);
    if (!p) return 0;
    Cdc_Construct(p);
    F(void*, p, 0) = g_tempDcVtbl;
    return p;
}

// [FUN_63a4f5] CTempDC::~CTempDC
extern "C" void __fastcall TempDc_Destruct(BYTE* self) { Cdc_Destruct(self); }

// [$L_63a4d5] CTempDC's deleting destructor
extern "C" BYTE* __fastcall TempDc_DeletingDestruct(BYTE* self, void*, int flags) {
    TempDc_Destruct(self);
    if (flags & 1) CallThis(FUN_61cb1e, g_allocTempDc, self);
    return self;
}

// [$L_63a500] CTempGdiObject's CreateObject
extern "C" BYTE* __cdecl TempGdi_CreateObject() {
    BYTE* p = CallThis<BYTE*>(FUN_61ca8f, g_allocTempGdi);
    if (!p) return 0;
    F(HGDIOBJ, p, 4) = 0;
    F(void*, p, 0) = g_tempGdiVtbl;
    return p;
}

// [FUN_63a53c] CGdiObject::~CGdiObject
extern "C" void __fastcall GdiObject_Destruct(BYTE* self) {
    F(void*, self, 0) = g_gdiObjectVtbl;
    GdiObject_DeleteObject(self);
}

// [$L_63a51c] CTempGdiObject's deleting destructor
extern "C" BYTE* __fastcall TempGdi_DeletingDestruct(BYTE* self, void*, int flags) {
    GdiObject_Destruct(self);
    if (flags & 1) CallThis(FUN_61cb1e, g_allocTempGdi, self);
    return self;
}

// [$L_63b1ff] CMenu's CreateObject
extern "C" BYTE* __cdecl Menu_CreateObject() {
    BYTE* p = (BYTE*)operator new(8);
    if (!p) return 0;
    F(void*, p, 0) = g_menuVtbl;
    F(HMENU, p, 4) = 0;
    return p;
}

// [$L_63b21f] CTempMenu's CreateObject
extern "C" BYTE* __cdecl TempMenu_CreateObject() {
    BYTE* p = CallThis<BYTE*>(FUN_61ca8f, g_allocTempMenu);
    if (!p) return 0;
    F(HMENU, p, 4) = 0;
    F(void*, p, 0) = g_tempMenuVtbl;
    return p;
}

// [FUN_63b25b] CMenu::~CMenu
extern "C" void __fastcall Menu_Destruct(BYTE* self) {
    F(void*, self, 0) = g_menuVtbl;
    Menu_DestroyMenu(self);
}

// [$L_63b23b] CTempMenu's deleting destructor
extern "C" BYTE* __fastcall TempMenu_DeletingDestruct(BYTE* self, void*, int flags) {
    Menu_Destruct(self);
    if (flags & 1) CallThis(FUN_61cb1e, g_allocTempMenu, self);
    return self;
}

// [$L_63b12b] AUX_DATA::AUX_DATA (afxData): Windows version, metrics,
// colours and cursors
extern "C" BYTE* __fastcall AuxData_Construct(BYTE* self) {
    DWORD v = GetVersion();
    F(int, self, 84) = (LOBYTE(v) << 8) + HIBYTE(LOWORD(v));      // nWinVer
    F(BOOL, self, 88) = v >> 31;                                   // bWin95
    BOOL win4 = (BYTE)v >= 4;
    F(BOOL, self, 92) = win4;                                      // bWin4
    F(BOOL, self, 96) = 1 - win4;                                  // bNotWin4
    F(BOOL, self, 100) = win4;                                     // bSmCaption
    F(BOOL, self, 104) = FALSE;                                    // bMarked4
    if (win4) F(BOOL, self, 104) = GetProcessVersion(0) >= 0x40000;
    AuxData_UpdateSysMetrics(self);
    F(HBRUSH, self, 36) = 0;
    AuxData_UpdateSysColors(self);
    F(HCURSOR, self, 60) = LoadCursorA(0, IDC_WAIT);
    F(HCURSOR, self, 64) = LoadCursorA(0, IDC_ARROW);
    F(HBITMAP, self, 80) = 0;
    F(HCURSOR, self, 68) = 0;
    F(int, self, 16) = F(int, self, 20) = F(BOOL, self, 92) ? 2 : 1;   // cxBorder2, cyBorder2
    return self;
}

// [$L_63b545] AUX_DATA::~AUX_DATA
extern "C" void __fastcall AuxData_Destruct(BYTE* self) { Afx_DeleteObject(&F(HGDIOBJ, self, 80)); }

// [FUN_63b0b9] constructs afxData
extern "C" void __cdecl AfxData_Construct() { AuxData_Construct((BYTE*)g_afxData); }

// [$L_63b0cf] destroys afxData at exit
extern "C" void __cdecl AfxData_Term() { AuxData_Destruct((BYTE*)g_afxData); }

// [$L_63b0c3] registers its destruction
extern "C" void __cdecl AfxData_AtExit() { atexit(AfxData_Term); }

// [$L_63b0af] static initializer of afxData
extern "C" void __cdecl AfxData_Init() { AfxData_Construct(); AfxData_AtExit(); }

// [FUN_63b1cc] _AfxIsDBCS: the OEM code page has multibyte characters
extern "C" BOOL __cdecl Afx_IsDBCS() {
    CPINFO info;
    GetCPInfo(GetOEMCP(), &info);
    return info.MaxCharSize > 1;
}

// [$L_63b1ef] static initializer of _afxDBCS
extern "C" void __cdecl Afx_InitDBCS() { g_afxDBCS = Afx_IsDBCS(); }

// ---------------------------------------------------------------------------
// MFC's out-of-line inline functions (0x61c000): AfxGetMainWnd, the
// multiple monitor stubs, CDC text output, the exception constructors,
// CHandleMap's destructor, CFixedAlloc

KING_VAR(BOOL, g_multiMonInitDone, 72dab0)
KING_VAR(FARPROC, g_pfnGetSystemMetrics, 72da98)
KING_VAR(FARPROC, g_pfnMonitorFromWindow, 72da9c)
KING_VAR(FARPROC, g_pfnMonitorFromRect, 72daa0)
KING_VAR(FARPROC, g_pfnMonitorFromPoint, 72daa4)
KING_VAR(FARPROC, g_pfnGetMonitorInfo, 72daa8)
KING_VAR(FARPROC, g_pfnEnumDisplayMonitors, 72daac)
KING_VAR(char, g_user32Name, 653ad4)          // "USER32"
KING_VAR(char, g_nameGetSystemMetrics, 653ac0)
KING_VAR(char, g_nameMonitorFromWindow, 653aac)
KING_VAR(char, g_nameMonitorFromRect, 653a9c)
KING_VAR(char, g_nameMonitorFromPoint, 653a88)
KING_VAR(char, g_nameEnumDisplayMonitors, 653a74)
KING_VAR(char, g_nameGetMonitorInfo, 653a64)
KING_VAR(char, g_displayName, 653adc)         // "DISPLAY"
KING_VAR(AsmLabel, g_resourceExceptionVtbl, 6544b8)
KING_VAR(AsmLabel, g_userExceptionVtbl, 6544d0)
KING_VAR(AsmLabel, g_memoryExceptionVtbl, 654578)
KING_VAR(AsmLabel, g_notSupportedExceptionVtbl, 654590)

#define PRIMARY_MONITOR ((HMONITOR)0x12340042)

// [$L_61c180] returns +24
extern "C" void* __fastcall Object_Get24(BYTE* self) { return F(void*, self, 24); }

// [FUN_61c4c1] AfxGetMainWnd
extern "C" BYTE* __cdecl Afx_GetMainWnd() {
    BYTE* t = Afx_GetThread();
    return t ? CallVirt<BYTE*>(t, 116) : 0;                        // GetMainWnd
}

// [FUN_61c4d5] InitMultipleMonitorStubs: the monitor functions of USER32
// when there are any
extern "C" BOOL __cdecl Afx_InitMultipleMonitorStubs() {
    if (g_multiMonInitDone) return g_pfnGetMonitorInfo != 0;
    HMODULE user = GetModuleHandleA(&g_user32Name);
    if (user && (g_pfnGetSystemMetrics = GetProcAddress(user, &g_nameGetSystemMetrics)) != 0
        && (g_pfnMonitorFromWindow = GetProcAddress(user, &g_nameMonitorFromWindow)) != 0
        && (g_pfnMonitorFromRect = GetProcAddress(user, &g_nameMonitorFromRect)) != 0
        && (g_pfnMonitorFromPoint = GetProcAddress(user, &g_nameMonitorFromPoint)) != 0
        && (g_pfnEnumDisplayMonitors = GetProcAddress(user, &g_nameEnumDisplayMonitors)) != 0
        && (g_pfnGetMonitorInfo = GetProcAddress(user, &g_nameGetMonitorInfo)) != 0) {
        g_multiMonInitDone = TRUE;
        return TRUE;
    }
    g_pfnGetSystemMetrics = g_pfnMonitorFromWindow = g_pfnMonitorFromRect = g_pfnMonitorFromPoint = 0;
    g_pfnGetMonitorInfo = g_pfnEnumDisplayMonitors = 0;
    g_multiMonInitDone = TRUE;
    return FALSE;
}

// [FUN_61c5ad] xMonitorFromRect
extern "C" HMONITOR __stdcall Afx_MonitorFromRect(const RECT* r, DWORD flags) {
    if (Afx_InitMultipleMonitorStubs())
        return ((HMONITOR (WINAPI*)(const RECT*, DWORD))g_pfnMonitorFromRect)(r, flags);
    if (flags & 3) return PRIMARY_MONITOR;                         // MONITOR_DEFAULTTOPRIMARY/NEAREST
    if (r->right > 0 && r->bottom > 0 && r->left < GetSystemMetrics(SM_CXSCREEN)
        && r->top < GetSystemMetrics(SM_CYSCREEN))
        return PRIMARY_MONITOR;
    return 0;
}

// [FUN_61c603] xMonitorFromWindow
extern "C" HMONITOR __stdcall Afx_MonitorFromWindow(HWND h, DWORD flags) {
    if (Afx_InitMultipleMonitorStubs())
        return ((HMONITOR (WINAPI*)(HWND, DWORD))g_pfnMonitorFromWindow)(h, flags);
    if (flags & 3) return PRIMARY_MONITOR;
    WINDOWPLACEMENT wp;
    wp.length = sizeof wp;
    RECT* r = &wp.rcNormalPosition;
    BOOL ok = IsIconic(h) ? GetWindowPlacement(h, &wp) : GetWindowRect(h, r);
    return ok ? Afx_MonitorFromRect(r, flags) : 0;
}

// [FUN_61c66e] xGetMonitorInfo
extern "C" BOOL __stdcall Afx_GetMonitorInfo(HMONITOR m, MONITORINFO* info) {
    if (Afx_InitMultipleMonitorStubs())
        return ((BOOL (WINAPI*)(HMONITOR, MONITORINFO*))g_pfnGetMonitorInfo)(m, info);
    RECT work;
    if (m != PRIMARY_MONITOR || !info || info->cbSize < 40 || !SystemParametersInfoA(SPI_GETWORKAREA, 0, &work, 0))
        return FALSE;
    info->rcMonitor.left = 0;
    info->rcMonitor.top = 0;
    info->rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
    info->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    info->rcWork = work;
    info->dwFlags = MONITORINFOF_PRIMARY;
    if (info->cbSize >= 72) lstrcpyA((char*)info + 40, &g_displayName);   // MONITORINFOEX::szDevice
    return TRUE;
}

// [FUN_61c701] CWnd::GetOwner
extern "C" BYTE* __fastcall Wnd_GetOwner(BYTE* self) {
    HWND h = F(HWND, self, 32);                                    // m_hWndOwner
    if (!h) h = GetParent(HWND_OF(self));
    return Wnd_FromHandle(h);
}

// [$L_61c718] disables the window
extern "C" void __fastcall Wnd_Disable(BYTE* self) { EnableWindow(HWND_OF(self), FALSE); }

// [$L_61c724] enables the window
extern "C" void __fastcall Wnd_Enable(BYTE* self) { EnableWindow(HWND_OF(self), TRUE); }

// CSimpleException(bAutoDelete, nResourceID): +12 m_bInitialized,
// +16 m_szMessage, +148 the resource id
static BYTE* SimpleException_Construct(BYTE* self, BOOL autoDelete, UINT id, void* vtbl) {
    Exception_SetAutoDelete(self, 0, autoDelete);
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    F(UINT, self, 148) = id;
    F(void*, self, 0) = vtbl;
    return self;
}

// [FUN_61c749] CResourceException::CResourceException
extern "C" BYTE* __fastcall ResourceException_Construct(BYTE* self, void*, BOOL autoDelete, UINT id) {
    return SimpleException_Construct(self, autoDelete, id, g_resourceExceptionVtbl);
}

// [FUN_61c78f] CResourceException::~CResourceException
extern "C" void __fastcall ResourceException_Destruct(BYTE* self) { F(void*, self, 0) = g_resourceExceptionVtbl; }

// [$L_61c773] its deleting destructor
extern "C" BYTE* __fastcall ResourceException_DeletingDestruct(BYTE* self, void*, int flags) {
    ResourceException_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [FUN_61c796] CUserException::CUserException
extern "C" BYTE* __fastcall UserException_Construct(BYTE* self, void*, BOOL autoDelete, UINT id) {
    return SimpleException_Construct(self, autoDelete, id, g_userExceptionVtbl);
}

// [FUN_61c7dc] CUserException::~CUserException
extern "C" void __fastcall UserException_Destruct(BYTE* self) { F(void*, self, 0) = g_userExceptionVtbl; }

// [$L_61c7c0] its deleting destructor
extern "C" BYTE* __fastcall UserException_DeletingDestruct(BYTE* self, void*, int flags) {
    UserException_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [FUN_61c8e8] CMemoryException::CMemoryException
extern "C" BYTE* __fastcall MemoryException_Construct(BYTE* self, void*, BOOL autoDelete, UINT id) {
    return SimpleException_Construct(self, autoDelete, id, g_memoryExceptionVtbl);
}

// [FUN_61c92e] CMemoryException::~CMemoryException
extern "C" void __fastcall MemoryException_Destruct(BYTE* self) { F(void*, self, 0) = g_memoryExceptionVtbl; }

// [$L_61c912] its deleting destructor
extern "C" BYTE* __fastcall MemoryException_DeletingDestruct(BYTE* self, void*, int flags) {
    MemoryException_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [FUN_61c935] CNotSupportedException::CNotSupportedException
extern "C" BYTE* __fastcall NotSupportedException_Construct(BYTE* self, void*, BOOL autoDelete, UINT id) {
    return SimpleException_Construct(self, autoDelete, id, g_notSupportedExceptionVtbl);
}

// [FUN_61c97b] CNotSupportedException::~CNotSupportedException
extern "C" void __fastcall NotSupportedException_Destruct(BYTE* self) {
    F(void*, self, 0) = g_notSupportedExceptionVtbl;
}

// [$L_61c95f] its deleting destructor
extern "C" BYTE* __fastcall NotSupportedException_DeletingDestruct(BYTE* self, void*, int flags) {
    NotSupportedException_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// CDC text output on m_hDC (+4)

// [$L_61c7e3] CDC::PtVisible
extern "C" BOOL __fastcall Dc_PtVisible(BYTE* self, void*, int x, int y) { return PtVisible(F(HDC, self, 4), x, y); }

// [$L_61c7f7] CDC::RectVisible
extern "C" BOOL __fastcall Dc_RectVisible(BYTE* self, void*, const RECT* r) { return RectVisible(F(HDC, self, 4), r); }

// [$L_61c807] CDC::TextOut
extern "C" BOOL __fastcall Dc_TextOut(BYTE* self, void*, int x, int y, const char* s, int n) {
    return TextOutA(F(HDC, self, 4), x, y, s, n);
}

// [$L_61c823] CDC::ExtTextOut
extern "C" BOOL __fastcall Dc_ExtTextOut(BYTE* self, void*, int x, int y, UINT options, const RECT* r,
                                         const char* s, UINT n, const int* dx) {
    return ExtTextOutA(F(HDC, self, 4), x, y, options, r, s, n, dx);
}

// [$L_61c848] CDC::TabbedTextOut
extern "C" SIZE* __fastcall Dc_TabbedTextOut(BYTE* self, void*, SIZE* out, int x, int y, const char* s, int n,
                                             int tabs, const int* stops, int origin) {
    LONG r = TabbedTextOutA(F(HDC, self, 4), x, y, s, n, tabs, stops, origin);
    out->cx = (short)LOWORD(r);
    out->cy = (short)HIWORD(r);
    return out;
}

// [$L_61c880] CDC::DrawText
extern "C" int __fastcall Dc_DrawText(BYTE* self, void*, const char* s, int n, RECT* r, UINT format) {
    return DrawTextA(F(HDC, self, 4), s, n, r, format);
}

// [$L_61c89c] CDC::GrayString
extern "C" BOOL __fastcall Dc_GrayString(BYTE* self, void*, BYTE* brush, GRAYSTRINGPROC proc, LPARAM data,
                                         int n, int x, int y, int cx, int cy) {
    return GrayStringA(F(HDC, self, 4), brush ? F(HBRUSH, brush, 4) : 0, proc, data, n, x, y, cx, cy);
}

// [$L_61c8cc] CDC::Escape
extern "C" int __fastcall Dc_Escape(BYTE* self, void*, int escape, int n, const char* in, void* out) {
    return Escape(F(HDC, self, 4), escape, n, in, out);
}

extern "C" void __fastcall HandleMap_DeleteTemp(BYTE* self);

// [FUN_61c99e] CHandleMap::~CHandleMap
extern "C" void __fastcall HandleMap_Destruct(BYTE* self) {
    F(void*, self, 0) = g_handleMapVtbl;
    HandleMap_DeleteTemp(self);
    PtrMap_Destruct((PtrMap*)(self + 32));                         // m_temporaryMap
    PtrMap_Destruct((PtrMap*)(self + 4));                          // m_permanentMap
}

// [$L_61c982] its deleting destructor
extern "C" BYTE* __fastcall HandleMap_DeletingDestruct(BYTE* self, void*, int flags) {
    HandleMap_Destruct(self);
    if (flags & 1) operator delete(self);
    return self;
}

// [FUN_61ca02] CMenu::~CMenu (second copy)
extern "C" void __fastcall Menu_Destruct2(BYTE* self) {
    F(void*, self, 0) = g_menuVtbl;
    Menu_DestroyMenu(self);
}

// [$L_61c9e6] CMenu's deleting destructor
extern "C" BYTE* __fastcall Menu_DeletingDestruct(BYTE* self, void*, int flags) {
    Menu_Destruct2(self);
    if (flags & 1) operator delete(self);
    return self;
}

// CFixedAlloc: +0 m_nAllocSize, +4 m_nBlockSize, +8 m_pBlocks,
// +12 m_pNodeFree, +16 m_protect

// [FUN_61ca2b] CFixedAlloc::CFixedAlloc
extern "C" BYTE* __fastcall FixedAlloc_Construct(BYTE* self, void*, UINT allocSize, UINT blockSize) {
    F(void*, self, 12) = 0;
    F(void*, self, 8) = 0;
    F(UINT, self, 0) = allocSize;
    F(UINT, self, 4) = blockSize;
    InitializeCriticalSection(&F(CRITICAL_SECTION, self, 16));
    return self;
}

extern "C" void __fastcall Plex_FreeDataChain(Plex* self);

// [FUN_61ca67] CFixedAlloc::FreeAll
extern "C" void __fastcall FixedAlloc_FreeAll(BYTE* self) {
    EnterCriticalSection(&F(CRITICAL_SECTION, self, 16));
    Plex_FreeDataChain(F(Plex*, self, 8));
    F(void*, self, 8) = 0;
    F(void*, self, 12) = 0;
    LeaveCriticalSection(&F(CRITICAL_SECTION, self, 16));
}

// [$L_61ca53] CFixedAlloc::~CFixedAlloc
extern "C" void __fastcall FixedAlloc_Destruct(BYTE* self) {
    FixedAlloc_FreeAll(self);
    DeleteCriticalSection(&F(CRITICAL_SECTION, self, 16));
}

// [FUN_61ca8f] CFixedAlloc::Alloc: a new CPlex block feeds the free list
extern "C" void* __fastcall FixedAlloc_Alloc(BYTE* self) {
    EnterCriticalSection(&F(CRITICAL_SECTION, self, 16));
    if (!F(void*, self, 12)) {
        UINT size = F(UINT, self, 0);
        BYTE* block = (BYTE*)Plex_Create(&F(Plex*, self, 8), F(UINT, self, 4), size);
        int n = F(int, self, 4) - 1;
        BYTE* node = block + 4 + n * size;
        for (; n >= 0; n--, node -= size) {
            *(void**)node = F(void*, self, 12);
            F(void*, self, 12) = node;
        }
    }
    void** node = F(void**, self, 12);
    F(void*, self, 12) = *node;
    LeaveCriticalSection(&F(CRITICAL_SECTION, self, 16));
    return node;
}

// [FUN_61cb1e] CFixedAlloc::Free
extern "C" void __fastcall FixedAlloc_Free(BYTE* self, void*, void* p) {
    if (!p) return;
    EnterCriticalSection(&F(CRITICAL_SECTION, self, 16));
    *(void**)p = F(void*, self, 12);
    F(void*, self, 12) = p;
    LeaveCriticalSection(&F(CRITICAL_SECTION, self, 16));
}

// ---------------------------------------------------------------------------
// CPtrList nodes, the message filter hook, dialog helpers, CenterWindow,
// common controls, the registry profile

KING_VAR(char, g_comboBoxClass, 6545ac)        // "combobox"
KING_VAR(char, g_comctlName, 653c08)           // "COMCTL32.DLL"
KING_VAR(char, g_initCommonControlsExName, 653bf0)
KING_VAR(char, g_fmtInt, 66c8cc)               // "%d"
KING_VAR(char, g_softwareKey, 6545b8)          // "software"
KING_VAR(AsmLabel, g_vtbl6534a0, 6534a0)
extern "C" __declspec(dllimport) void __stdcall InitCommonControls();

// [FUN_632202] CPtrList::CPtrList(nBlockSize)
extern "C" BYTE* __fastcall PtrList_Construct(BYTE* self, void*, int blockSize) {
    F(void*, self, 12) = 0;                                        // m_nCount
    F(void*, self, 16) = 0;                                        // m_pNodeFree
    F(void*, self, 8) = 0;                                         // m_pNodeTail
    F(void*, self, 4) = 0;                                         // m_pNodeHead
    F(void*, self, 20) = 0;                                        // m_pBlocks
    F(void*, self, 0) = g_ptrListVtbl;
    F(int, self, 24) = blockSize;
    return self;
}

// [FUN_63228a] CPtrList::NewNode(pPrev, pNext): nodes come from CPlex
// blocks of m_nBlockSize
extern "C" void** __fastcall PtrList_NewNode(BYTE* self, void*, void** prev, void** next) {
    if (!F(void*, self, 16)) {
        int n = F(int, self, 24);
        BYTE* block = (BYTE*)Plex_Create(&F(Plex*, self, 20), n, 12);
        void** node = (void**)(block + 4 + (n - 1) * 12);
        for (int i = n - 1; i >= 0; i--, node -= 3) {
            node[0] = F(void*, self, 16);
            F(void**, self, 16) = node;
        }
    }
    void** node = F(void**, self, 16);
    F(void*, self, 16) = node[0];
    node[1] = prev;
    node[0] = next;
    F(int, self, 12)++;
    node[2] = 0;
    return node;
}

// [FUN_6322fb] CPtrList::AddTail
extern "C" void** __fastcall PtrList_AddTail(BYTE* self, void*, void* data) {
    void** node = PtrList_NewNode(self, 0, F(void**, self, 8), 0);
    node[2] = data;
    if (F(void**, self, 8)) F(void**, self, 8)[0] = node;
    else F(void**, self, 4) = node;
    F(void**, self, 8) = node;
    return node;
}

// [$L_632f34] _AfxMsgFilterHook: dialog, menu and DDE message filters to
// the thread's ProcessMessageFilter
extern "C" LRESULT __stdcall Afx_MsgFilterHook(int code, WPARAM w, LPARAM l) {
    if (!F(BYTE, Afx_GetModuleState(), 20) && (code >= 0 || code == 0x8001)) {   // MSGF_DDEMGR
        BYTE* thread = Afx_GetThread();
        if (thread) return CallVirt<BOOL>(thread, 112, code, (MSG*)l);   // ProcessMessageFilter
    }
    return CallNextHookEx(F(HHOOK, Afx_GetThreadState(), 48), code, w, l);
}

// [FUN_633a2e] _AfxPreInitDialog: the window's rectangle and style before
// WM_INITDIALOG
extern "C" void __stdcall Afx_PreInitDialog(BYTE* wnd, RECT* oldRect, DWORD* oldStyle) {
    GetWindowRect(HWND_OF(wnd), oldRect);
    *oldStyle = Wnd_GetStyle(wnd);
}

extern "C" void __fastcall Wnd_CenterWindow(BYTE* self, void*, BYTE* alternateOwner);

// [FUN_633a51] _AfxPostInitDialog: centres a dialog that WM_INITDIALOG
// neither showed nor moved
extern "C" void __stdcall Afx_PostInitDialog(BYTE* wnd, const RECT* oldRect, DWORD oldStyle) {
    if (oldStyle & WS_VISIBLE) return;
    if (Wnd_GetStyle(wnd) & (WS_VISIBLE | WS_CHILD)) return;
    RECT r;
    GetWindowRect(HWND_OF(wnd), &r);
    if (oldRect->left != r.left || oldRect->top != r.top) return;
    BYTE* owner = Wnd_FromHandle(GetWindow(HWND_OF(wnd), GW_OWNER));
    if (owner && Wnd_IsWindowEnabled(owner)) return;
    if (!CallVirt<BOOL>(wnd, 172)) return;                         // CheckAutoCenter
    Wnd_CenterWindow(wnd, 0, 0);
}

// [FUN_6371de] _AfxIsComboBoxControl(hWnd, nStyle)
extern "C" BOOL __stdcall Afx_IsComboBoxControl(HWND h, UINT style) {
    if (!h || (UINT)(GetWindowLongA(h, GWL_STYLE) & 15) != style) return FALSE;
    char cls[12];
    GetClassNameA(h, cls, 10);
    return !lstrcmpiA(cls, &g_comboBoxClass);
}

// [FUN_635b54] AfxGrayCtlColor: the dialog brush's colour behind the text
// of gray controls
extern "C" BOOL __stdcall Afx_GrayCtlColor(HDC dc, HWND h, UINT type, HBRUSH gray, COLORREF text) {
    if (!dc || !gray) return FALSE;
    if (type == CTLCOLOR_EDIT || type == CTLCOLOR_MSGBOX || type == CTLCOLOR_SCROLLBAR) return FALSE;
    if (type == CTLCOLOR_LISTBOX && !Afx_IsComboBoxControl(h, CBS_DROPDOWN)) return FALSE;
    LOGBRUSH lb;
    GetObjectA(gray, sizeof lb, &lb);
    SetBkColor(dc, lb.lbColor);
    if (text == (COLORREF)-1) text = GetSysColor(COLOR_WINDOWTEXT);
    SetTextColor(dc, text);
    return TRUE;
}

// [FUN_635bc9] CWnd::CenterWindow: over the owner (or the work area),
// kept inside the work area or the parent's client area
extern "C" void __fastcall Wnd_CenterWindow(BYTE* self, void*, BYTE* alternateOwner) {
    DWORD style = Wnd_GetStyle(self);
    HWND center;
    if (alternateOwner) {
        center = HWND_OF(alternateOwner);
    } else {
        center = (style & WS_CHILD) ? GetParent(HWND_OF(self)) : GetWindow(HWND_OF(self), GW_OWNER);
        if (center) {
            HWND t = (HWND)SendMessageA(center, 875, 0, 0);       // WM_QUERYCENTERWND
            if (t) center = t;
        }
    }
    RECT dlg, area, rc;
    GetWindowRect(HWND_OF(self), &dlg);
    if (!(style & WS_CHILD)) {
        if (center) {
            DWORD cs = GetWindowLongA(center, GWL_STYLE);
            if (!(cs & WS_VISIBLE) || (cs & WS_MINIMIZE)) center = 0;
        }
        MONITORINFO mi;
        mi.cbSize = 40;
        if (!center) {
            BYTE* main = Afx_GetMainWnd();
            Afx_GetMonitorInfo(Afx_MonitorFromWindow(main ? HWND_OF(main) : 0, MONITOR_DEFAULTTOPRIMARY), &mi);
            CopyRect(&rc, &mi.rcWork);
            CopyRect(&area, &mi.rcWork);
        } else {
            GetWindowRect(center, &rc);
            Afx_GetMonitorInfo(Afx_MonitorFromWindow(center, MONITOR_DEFAULTTONEAREST), &mi);
            CopyRect(&area, &mi.rcWork);
        }
    } else {
        HWND parent = GetParent(HWND_OF(self));
        GetClientRect(parent, &area);
        GetClientRect(center, &rc);
        MapWindowPoints(center, parent, (POINT*)&rc, 2);
    }
    int w = dlg.right - dlg.left, h = dlg.bottom - dlg.top;
    int x = (rc.left + rc.right) / 2 - w / 2;
    int y = (rc.top + rc.bottom) / 2 - h / 2;
    if (x < area.left) x = area.left;
    else if (x + w > area.right) x = area.right - w;
    if (y < area.top) y = area.top;
    else if (y + h > area.bottom) y = area.bottom - h;
    Wnd_SetWindowPos(self, 0, 0, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// [FUN_635dea] _AfxInitCommonControls: InitCommonControlsEx when
// COMCTL32 has it, else InitCommonControls for the old control classes
extern "C" LONG __stdcall Afx_InitCommonControls(void* init, LONG toRegister) {
    HMODULE before = GetModuleHandleA(&g_comctlName);
    HMODULE lib = LoadLibraryA(&g_comctlName);
    if (!lib) return 0;
    LONG r = 0;
    typedef BOOL (WINAPI* InitExFn)(void*);
    InitExFn initEx = (InitExFn)GetProcAddress(lib, &g_initCommonControlsExName);
    if (!initEx) {
        if ((toRegister & 0x3FC0) == toRegister) {                 // only the Windows 95 classes
            InitCommonControls();
            r = 0x3FC0;
        }
    } else if (initEx(init)) {
        r = toRegister;
        if (!before) {
            InitCommonControls();
            r |= 0x3FC0;
        }
    }
    FreeLibrary(lib);
    return r;
}

// [FUN_63aeb1] CWinApp::GetAppRegistryKey: HKCU\software\<registry key
// (+124)>\<profile name (+144)>
extern "C" HKEY __fastcall WinApp_GetAppRegistryKey(BYTE* self) {
    HKEY app = 0, soft = 0, company = 0;
    DWORD disp;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, &g_softwareKey, 0, KEY_WRITE | KEY_READ, &soft) == ERROR_SUCCESS
        && RegCreateKeyExA(soft, F(const char*, self, 124), 0, 0, 0, KEY_WRITE | KEY_READ, 0, &company, &disp)
               == ERROR_SUCCESS)
        RegCreateKeyExA(company, F(const char*, self, 144), 0, 0, 0, KEY_WRITE | KEY_READ, 0, &app, &disp);
    if (soft) RegCloseKey(soft);
    if (company) RegCloseKey(company);
    return app;
}

// [FUN_63af45] CWinApp::GetSectionKey
extern "C" HKEY __fastcall WinApp_GetSectionKey(BYTE* self, void*, const char* section) {
    HKEY app = WinApp_GetAppRegistryKey(self);
    if (!app) return 0;
    HKEY key = 0;
    DWORD disp;
    RegCreateKeyExA(app, section, 0, 0, 0, KEY_WRITE | KEY_READ, 0, &key, &disp);
    RegCloseKey(app);
    return key;
}

// [FUN_6377ac] CWinApp::WriteProfileInt: the registry, or the INI file
// (+144)
extern "C" BOOL __fastcall WinApp_WriteProfileInt(BYTE* self, void*, const char* section, const char* entry, int value) {
    if (F(const char*, self, 124)) {
        HKEY key = WinApp_GetSectionKey(self, 0, section);
        if (!key) return FALSE;
        LONG r = RegSetValueExA(key, entry, 0, REG_DWORD, (const BYTE*)&value, 4);
        RegCloseKey(key);
        return r == ERROR_SUCCESS;
    }
    char text[16];
    wsprintfA(text, &g_fmtInt, value);
    return WritePrivateProfileStringA(section, entry, text, F(const char*, self, 144));
}

// [FUN_639948] AfxOleCanExitApp: no OLE objects left
extern "C" BOOL __cdecl Afx_OleCanExitApp() { return F(int, Afx_GetModuleState(), 44) == 0; }

// [FUN_639970] sets the vtable $L_6534a0
extern "C" BYTE* __fastcall Object_SetVtbl6534a0(BYTE* self) {
    F(void*, self, 0) = g_vtbl6534a0;
    return self;
}

// ---------------------------------------------------------------------------
// CSimpleException's message, CWinApp::DevModeChange, the gray dialog
// background, CFrameWnd::LoadFrame, the CTL3D and window state factories

extern "C" void FUN_6317ca(); KING_ALIAS("_FUN_6317ca", "_OpenPrinterA@12")                            // OpenPrinterA
extern "C" void FUN_6317c4(); KING_ALIAS("_FUN_6317c4", "_DocumentPropertiesA@24")                            // DocumentPropertiesA
extern "C" void FUN_6317be(); KING_ALIAS("_FUN_6317be", "_ClosePrinter@4")                            // ClosePrinter
KING_VAR(AsmLabel, g_processLocalWinState, 72c15c)   // CProcessLocal<_AFX_WIN_STATE>

// [FUN_63217d] CSimpleException::InitString: loads the message of the
// resource id (+148) into +20
extern "C" void __fastcall SimpleException_InitString(BYTE* self) {
    F(BOOL, self, 12) = TRUE;                                      // m_bInitialized
    F(BOOL, self, 16) = Afx_LoadString(F(UINT, self, 148), (char*)self + 20, 128) != 0;   // m_bLoaded
}

// [$L_6321a6] CSimpleException::GetErrorMessage
extern "C" BOOL __fastcall SimpleException_GetErrorMessage(BYTE* self, void*, char* error, UINT maxError, UINT* help) {
    if (help) *help = 0;
    if (!F(BOOL, self, 12)) SimpleException_InitString(self);
    if (F(BOOL, self, 16)) lstrcpynA(error, (const char*)self + 20, maxError);
    else *error = 0;
    return F(BOOL, self, 16);
}

// [FUN_6327b7] CWinApp::DevModeChange: rereads the DEVMODE (+148) of the
// selected printer (+152 its DEVNAMES)
extern "C" void __fastcall WinApp_DevModeChange(BYTE* self, void*, char* device) {
    if (!F(HGLOBAL, self, 152)) return;
    BYTE* names = (BYTE*)GlobalLock(F(HGLOBAL, self, 152));
    if (lstrcmpA((const char*)names + F(WORD, names, 2), device)) return;   // wDeviceOffset
    HANDLE printer;
    if (!CallStd<BOOL>(FUN_6317ca, device, &printer, (void*)0)) return;
    if (F(HGLOBAL, self, 148)) Afx_GlobalFree(F(HGLOBAL, self, 148));
    LONG size = CallStd<LONG>(FUN_6317c4, (HWND)0, printer, device, (void*)0, (void*)0, (DWORD)0);
    F(HGLOBAL, self, 148) = GlobalAlloc(GHND, size);
    void* mode = GlobalLock(F(HGLOBAL, self, 148));
    if (CallStd<LONG>(FUN_6317c4, (HWND)0, printer, device, mode, (void*)0, (DWORD)2) != IDOK) {   // DM_OUT_BUFFER
        Afx_GlobalFree(F(HGLOBAL, self, 148));
        F(HGLOBAL, self, 148) = 0;
    }
    CallStd(FUN_6317be, printer);
}

// [$L_63a003] new _AFX_WIN_STATE
extern "C" BYTE* __cdecl WinState_New() {
    BYTE* p = (BYTE*)NoTrack_New(16);
    return p ? Object_SetVtbl6534a0(p) : 0;
}

// [$L_633fa2] _AfxGrayBackgroundWndProc: the dialog brush behind gray
// controls (SetDialogBkColor), the rest to the activation procedure
extern "C" LRESULT __stdcall Afx_GrayBackgroundWndProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    BYTE* ws = (BYTE*)ProcessLocal_GetData((void**)&g_processLocalWinState, 0, (void* (__stdcall*)())WinState_New);
    if (F(HBRUSH, ws, 4)
        && (msg == WM_CTLCOLORBTN || msg == WM_CTLCOLORDLG || msg == WM_CTLCOLORSTATIC
            || msg == WM_CTLCOLORSCROLLBAR || msg == WM_CTLCOLORLISTBOX)
        && Afx_GrayCtlColor((HDC)w, (HWND)l, msg - WM_CTLCOLORMSGBOX, F(HBRUSH, ws, 4), F(COLORREF, ws, 8)))
        return (LRESULT)F(HBRUSH, ws, 4);
    return Afx_ActivationWndProc(h, msg, w, l);
}

// [$L_6382b8] CFrameWnd::LoadFrame: the title, icon class, menu and
// accelerators of the resource id
extern "C" BOOL __fastcall FrameWnd_LoadFrame(BYTE* self, void*, UINT id, DWORD style, BYTE* parent, void* context) {
    F(UINT, self, 140) = id;                                       // m_nIDHelp
    char* full = g_afxPchNil;
    if (String_LoadString(&full, 0, id)) Afx_ExtractSubString(&F(char*, self, 172), full, 0, '\n');
    Afx_EndDeferRegisterClass(8);                                  // AFX_WNDFRAMEORVIEW_REG
    const char* cls = FrameWnd_GetIconWndClass(self, 0, style, id);
    BOOL r = FALSE;
    if (FrameWnd_Create(self, 0, cls, F(const char*, self, 172), style, &g_rectDefault, parent,
                        MAKEINTRESOURCEA((WORD)id), 0, context)) {
        F(HMENU, self, 68) = GetMenu(HWND_OF(self));               // m_hMenuDefault
        FrameWnd_LoadAccelTable(self, 0, MAKEINTRESOURCEA((WORD)id));
        if (!context) Wnd_SendMessageToDescendants(HWND_OF(self), 868, 0, 0, TRUE, TRUE);   // WM_INITIALUPDATE
        r = TRUE;
    }
    String_Destruct(&full);
    return r;
}

// [$L_639de0] new _AFX_CTL3D_STATE
extern "C" BYTE* __cdecl Ctl3dState_New() {
    BYTE* p = (BYTE*)NoTrack_New(48);
    if (!p) return 0;
    F(void*, p, 0) = g_ctl3dStateVtbl;
    return p;
}

// new _AFX_CTL3D_THREAD ($L_639df5, inside the PROC of $L_639de0)
extern "C" BYTE* __cdecl Ctl3dThread_New() {
    BYTE* p = (BYTE*)NoTrack_New(4);
    if (!p) return 0;
    F(void*, p, 0) = g_ctl3dThreadVtbl;
    return p;
}

// ---------------------------------------------------------------------------
// CWnd::OnFinalRelease, control bars and the print preview mode of
// CFrameWnd

KING_VAR(AsmLabel, g_class653630, 653630)
KING_VAR(AsmLabel, g_class653650, 653650)
KING_VAR(AsmLabel, g_class653680, 653680)
KING_VAR(AsmLabel, g_class653750, 653750)

// [$L_63340f] GetRuntimeClass ($L_653630)
extern "C" void* __fastcall Class653630_Get(BYTE* self) { return g_class653630; }

// [$L_633415] GetRuntimeClass ($L_653650)
extern "C" void* __fastcall Class653650_Get(BYTE* self) { return g_class653650; }

// [$L_63341b] GetRuntimeClass ($L_653680)
extern "C" void* __fastcall Class653680_Get(BYTE* self) { return g_class653680; }

// [$L_634b1b] GetRuntimeClass ($L_653750)
extern "C" void* __fastcall Class653750_Get(BYTE* self) { return g_class653750; }

// [$L_634581] CWnd::OnFinalRelease
extern "C" void __fastcall Wnd_OnFinalRelease(BYTE* self) {
    if (HWND_OF(self)) CallVirt(self, 88);                         // DestroyWindow (calls PostNcDestroy)
    else CallVirt(self, 164);                                      // PostNcDestroy
}

// [FUN_638fd9] CFrameWnd::DestroyDockBars: the dock bars, a floating one
// with its frame
extern "C" void __fastcall FrameWnd_DestroyDockBars(BYTE* self) {
    BYTE list[28];
    PtrList_Construct(list, 0, 10);
    for (void** node = F(void**, self, 112); node; ) {
        BYTE* bar = (BYTE*)node[2];
        node = (void**)node[0];
        if (CallVirt<BOOL>(bar, 208)) PtrList_AddTail(list, 0, bar);   // IsDockBar
    }
    for (void** node = F(void**, list, 4); node; ) {
        BYTE* dock = (BYTE*)node[2];
        node = (void**)node[0];
        if (F(BOOL, dock, 120)) CallVirt(Wnd_GetParentFrame(dock), 88);   // m_bFloating: DestroyWindow
        else CallVirt(dock, 88);
    }
    PtrList_Destruct(list);
}

// [FUN_638aa4] CFrameWnd::ShowControlBar(pBar, bShow, bDelay)
extern "C" void __fastcall FrameWnd_ShowControlBar(BYTE* self, void*, BYTE* bar, BOOL show, BOOL delay) {
    BYTE* frame = ControlBar_GetDockingFrame(bar);
    if (delay) {
        CallVirt(bar, 196, show);                                  // DelayShow
        F(DWORD, frame, 184) |= 12;                                // DelayRecalcLayout
    } else {
        Wnd_SetWindowPos(bar, 0, 0, 0, 0, 0, 0,
                         (show ? SWP_SHOWWINDOW : SWP_HIDEWINDOW) | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        CallVirt(bar, 196, show);
        if (show || !ControlBar_IsFloating(bar)) CallVirt(frame, 200, FALSE);   // RecalcLayout
    }
    if (!ControlBar_IsFloating(bar)) return;
    BYTE* dock = F(BYTE*, bar, 112);
    int visible = dock ? CallVirt<int>(dock, 224) : show != 0;     // GetDockedVisibleCount
    if (visible == 1 && show) {
        F(int, frame, 136) = -1;                                   // m_nShowDelay
        if (delay) {
            F(int, frame, 136) = SW_SHOWNA;
            CallVirt(frame, 200, FALSE);
        } else {
            Wnd_ShowWindow(frame, 0, SW_SHOWNA);
        }
    } else if (visible == 0) {
        F(int, frame, 136) = -1;
        if (delay) F(int, frame, 136) = SW_HIDE;
        else Wnd_ShowWindow(frame, 0, SW_HIDE);
    } else if (!delay) {
        CallVirt(frame, 200, FALSE);
    }
}

// [$L_6393ae] CFrameWnd::OnSetPreviewMode: hides the control bars, main
// pane, menu and accelerators for the print preview and brings them back.
// CPrintPreviewState: +0 nIDMainPane, +4 hMenu, +8 dwStates, +16
// lpfnCloseProc, +20 hAccelTable.
extern "C" void __fastcall FrameWnd_OnSetPreviewMode(BYTE* self, void*, BOOL preview, BYTE* state) {
    BYTE* active = CallVirt<BYTE*>(self, 192);                     // GetActiveFrame
    if (preview && F(BYTE*, active, 104)) CallVirt(F(BYTE*, active, 104), 92, FALSE);   // OnDocActivate
    DWORD visible = 0;
    for (void** node = F(void**, self, 112); node; ) {
        BYTE* bar = (BYTE*)node[2];
        node = (void**)node[0];
        UINT id = LOWORD(GetDlgCtrlID(HWND_OF(bar)));
        if (id < 59392 || id > 59423) continue;                    // AFX_IDW_CONTROLBAR_FIRST..LAST
        DWORD mask = 1u << (id - 59392);
        if (CallVirt<BOOL>(bar, 200)) visible |= mask;             // IsVisible
        if (!CallVirt<BOOL>(bar, 208) || id != 59423)              // IsDockBar
            FrameWnd_ShowControlBar(self, 0, bar, F(DWORD, state, 8) & mask, TRUE);
    }
    F(DWORD, state, 8) = visible;
    HWND pane;
    if (preview) {
        F(void*, self, 156) = F(void*, state, 16);                 // m_lpfnCloseProc
        FrameWnd_ShowOwnedWindows(self, 0, FALSE);
        pane = GetDlgItem(HWND_OF(self), F(int, state, 0));
        ShowWindow(pane, SW_HIDE);
        F(HMENU, state, 4) = GetMenu(HWND_OF(self));
        if (F(HMENU, state, 4)) {
            InvalidateRect(HWND_OF(self), 0, TRUE);
            SetMenu(HWND_OF(self), 0);
            F(DWORD, self, 184) &= ~1;                             // idleMenu
        }
        F(HACCEL, state, 20) = F(HACCEL, self, 72);
        F(HACCEL, self, 72) = 0;
        FrameWnd_LoadAccelTable(self, 0, MAKEINTRESOURCEA(30997)); // AFX_IDR_PREVIEW_ACCEL
        HWND first = F(int, state, 0) != 59648 ? GetDlgItem(HWND_OF(self), 59648) : pane;   // AFX_IDW_PANE_FIRST
        if (first) SetWindowLongA(first, GWL_ID, 59937);           // AFX_IDW_PANE_SAVE
        return;
    }
    F(void*, self, 156) = 0;
    pane = GetDlgItem(HWND_OF(self), 59937);
    if (pane) {
        HWND first = GetDlgItem(HWND_OF(self), 59648);
        if (first) SetWindowLongA(first, GWL_ID, 59937);
        SetWindowLongA(pane, GWL_ID, 59648);
    }
    if (F(HMENU, state, 4)) {
        InvalidateRect(HWND_OF(self), 0, TRUE);
        SetMenu(HWND_OF(self), F(HMENU, state, 4));
    }
    if (F(BYTE*, active, 104)) CallVirt(F(BYTE*, active, 104), 92, TRUE);
    CallVirt(self, 200, TRUE);                                     // RecalcLayout
    if (F(int, state, 0) != 59648) pane = GetDlgItem(HWND_OF(self), F(int, state, 0));
    ShowWindow(pane, SW_SHOW);
    F(HACCEL, self, 72) = F(HACCEL, state, 20);
    FrameWnd_ShowOwnedWindows(self, 0, TRUE);
}
