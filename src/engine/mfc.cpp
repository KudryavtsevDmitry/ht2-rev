// mfc.cpp - the parts of MFC 4.2 (statically linked into king.exe) that the
// game runs, rewritten so that king.exe stops depending on the MFC library.
// The object layouts and vtable slots stay those of MFC 4.2, because the
// game's own classes derive from CWinApp and CFrameWnd.
//
// Contents: MFC's thread local and process local storage
// (CThreadSlotData, CThreadLocalObject, CProcessLocalObject) and the global
// locks (AfxLockGlobals).
//
// Building with  nmake ASM_MFC=1  makes king.masm assemble the original
// procedures again.

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

ASM_VAR(int, g_afxThreadState, $L_72db7c)       // CThreadLocal<_AFX_THREAD_STATE>
ASM_VAR(void*, g_afxBaseModuleState, $L_72db78) // CProcessLocal<_AFX_BASE_MODULE_STATE>
ASM_VAR(AsmLabel, g_threadStateVtbl, $L_654284)
ASM_VAR(AsmLabel, g_moduleThreadStateVtbl, $L_654294)
ASM_VAR(AsmLabel, g_moduleStateVtbl, $L_65428c)
ASM_VAR(AsmLabel, g_baseModuleStateVtbl, $L_65429c)
ASM_PROC(FUN_632629)                            // AFX_MODULE_THREAD_STATE's handler

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

ASM_PROC(FUN_620815)                            // _msize

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

ASM_VAR(AsmLabel, g_winThreadVtbl, $L_653570)
ASM_VAR(AsmLabel, g_winAppVtbl, $L_6534a8)
ASM_VAR(void*, g_afx72c160, $L_72c160)
ASM_VAR(void*, g_afx72c164, $L_72c164)
ASM_PROC(FUN_62003b)                            // _strdup

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
ASM_PROC(FUN_620066)                            // _beginthreadex
ASM_PROC(FUN_62016d)                            // _endthreadex
ASM_VAR(int, g_ctl3dThreadState, $L_72dd08)     // CThreadLocal<_AFX_CTL3D_THREAD>
ASM_VAR(void*, g_ctl3dState, $L_72dd0c)         // CProcessLocal<_AFX_CTL3D_STATE>
ASM_PROC($L_639de0)                             // new _AFX_CTL3D_STATE
ASM_PROC($L_639df5)                             // new _AFX_CTL3D_THREAD
ASM_PROC($L_632f34)                             // _AfxMsgFilterHook

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
    ThreadLocal_GetData(&g_ctl3dThreadState, 0, (void* (__stdcall*)())$L_639df5);
}

// ---------------------------------------------------------------------------
// WinMain

ASM_PROC(FUN_620d0e)                            // _tcsrchr
ASM_PROC(FUN_620585)                            // _tcschr
ASM_PROC(FUN_620903)                            // _tcsinc
ASM_VAR(char, g_extHlp, $L_6546dc)
ASM_VAR(char, g_extIni, $L_6546d4)

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

ASM_VAR(MsgCache, g_msgCache, $L_72c170)        // MsgCache[512]
ASM_VAR(AsmLabel, g_wndVtbl, $L_653b00)
ASM_VAR(char, g_oldWndProcProp, $L_6539f8)      // "AfxOldWndProc423"
ASM_VAR(char, g_imeClass, $L_653bb8)            // "ime"
ASM_VAR(int, g_afxDBCS, $L_72e0e4)
ASM_VAR(int, g_afxCtl3dOff, $L_72e0d4)
ASM_VAR(AsmLabel, g_wndTop, $L_72da58)          // the static wndTop, wndBottom,
ASM_VAR(AsmLabel, g_wndBottom, $L_72da18)       // wndTopMost and wndNoTopMost,
ASM_VAR(AsmLabel, g_wndTopMost, $L_72d9d8)      // never destroyed
ASM_VAR(AsmLabel, g_wndNoTopMost, $L_72d998)
ASM_VAR(AsmLabel, g_tempWndClass, $L_653708)    // CTempWnd's CRuntimeClass
ASM_PROC($L_6373bb)                             // new handler that throws
ASM_PROC(FUN_633a2e)                            // _AfxPreInitDialog
ASM_PROC(FUN_633a51)                            // _AfxPostInitDialog
ASM_PROC(FUN_61c701)                            // CWnd::GetParentOwner part
ASM_PROC(FUN_61c4c1)                            // AfxGetMainWnd
ASM_PROC(FUN_636ade)                            // CMenu::FromHandle
ASM_PROC(FUN_639948)                            // AfxOleCanExitApp
ASM_PROC($L_633fa2)                             // _AfxActivationWndProc (CTL3D)

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


ASM_VAR(AsmLabel, g_ptrListVtbl, $L_6546c8)
ASM_VAR(AsmLabel, g_ptrMapVtbl, $L_65461c)
ASM_VAR(void*, g_afxNewHandler, $L_685e30)
ASM_VAR(AsmLabel, g_winAppClass, $L_653418)

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

ASM_VAR(AsmLabel, g_handleMapVtbl, $L_6545ec)
ASM_VAR(AsmLabel, g_tempDcClass, $L_6542c8)
ASM_VAR(AsmLabel, g_tempGdiClass, $L_6542a0)

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

ASM_VAR(BYTE*, g_afxDataNil, $L_685fc4)
ASM_VAR(AsmLabel, g_alloc64, $L_72db30)        // CFixedAlloc for strings up to 64,
ASM_VAR(AsmLabel, g_alloc128, $L_72db08)       // 128, 256 and 512 characters
ASM_VAR(AsmLabel, g_alloc256, $L_72dae0)
ASM_VAR(AsmLabel, g_alloc512, $L_72dab8)
ASM_VAR(AsmLabel, g_dcVtbl, $L_6543a8)
ASM_PROC(FUN_61cb1e)                           // CFixedAlloc::Free

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

ASM_VAR(AsmLabel, g_frameWndVtbl, $L_654184)
ASM_VAR(char, g_settingsSection, $L_653490)
ASM_VAR(char, g_previewPagesEntry, $L_653480)
ASM_PROC(FUN_6371de)                           // _AfxIsComboBoxControl
ASM_PROC(FUN_639958)                           // AfxOleSetUserCtrl
ASM_PROC(FUN_638fc2)                           // CFrameWnd::OnUpdateFrameTitle part
ASM_PROC(FUN_6377ac)                           // CWinApp::WriteProfileInt
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

ASM_VAR(char*, g_afxPchNil, $L_685fc0)          // the empty string's characters
ASM_PROC(FUN_61ca8f)                           // CFixedAlloc::Alloc
ASM_PROC(FUN_636471)                           // CString::GetBuffer
ASM_PROC(FUN_6364c0)                           // CString::ReleaseBuffer

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

ASM_VAR(HCURSOR, g_afxArrowCursor, $L_72e0b8)
ASM_VAR(char, g_classWnd, $L_653a0c)           // "AfxWnd42s"
ASM_VAR(char, g_classOleControl, $L_653a50)    // "AfxOleControl42s"
ASM_VAR(char, g_classControlBar, $L_653a18)    // "AfxControlBar42s"
ASM_VAR(char, g_classMDIFrame, $L_653a2c)      // "AfxMDIFrame42s"
ASM_VAR(char, g_classFrameOrView, $L_653a3c)   // "AfxFrameOrView42s"
ASM_PROC(FUN_635dea)                           // _AfxInitCommonControls

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

ASM_PROC(FUN_632202)                           // CPtrList::CPtrList

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

ASM_VAR(char, g_ctl3dDll, $L_6544e4)           // "CTL3D32.DLL"
ASM_VAR(void*, g_winState, $L_72c15c)          // CProcessLocal<_AFX_WIN_STATE>
ASM_PROC($L_63a003)                            // new _AFX_WIN_STATE
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
