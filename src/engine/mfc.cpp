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

ASM_PROC(FUN_636a07)                            // CHandleMap::DeleteTemp
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
            CallThis(FUN_636a07, F(void*, mts, 32));
            CallThis(FUN_636a07, F(void*, mts, 28));
            CallThis(FUN_636a07, F(void*, mts, 24));
            CallThis(FUN_636a07, F(void*, mts, 20));
            CallThis(FUN_636a07, F(void*, mts, 36));
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
ASM_PROC(FUN_6365c1)                            // AfxLoadString
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
        F(char*, self, 120) = StrDup(CallStd<int>(FUN_6365c1, 0xE000, title, 256) ? title : F(char*, self, 136));
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
ASM_PROC(FUN_632631)                            // AfxSetNewHandler
ASM_PROC(FUN_6368e3)                            // CHandleMap::CHandleMap
ASM_PROC(FUN_636948)                            // CHandleMap::FromHandle
ASM_PROC(FUN_6324f2)                            // CMapPtrToPtr lookup: value or 0
ASM_PROC(FUN_632525)                            // CMapPtrToPtr::operator[]
ASM_PROC(FUN_632575)                            // CMapPtrToPtr::RemoveKey
ASM_PROC(FUN_6337ad)                            // CWnd::IsWindowEnabled
ASM_PROC(FUN_633a2e)                            // _AfxPreInitDialog
ASM_PROC(FUN_633a51)                            // _AfxPostInitDialog
ASM_PROC(FUN_633aca)                            // _AfxHandleActivate
ASM_PROC(FUN_61c701)                            // CWnd::GetParentOwner part
ASM_PROC(FUN_61c4c1)                            // AfxGetMainWnd
ASM_PROC(FUN_636c7c)                            // CDC::FromHandle
ASM_PROC(FUN_636ade)                            // CMenu::FromHandle
ASM_PROC(FUN_639948)                            // AfxOleCanExitApp
ASM_PROC($L_633fa2)                             // _AfxActivationWndProc (CTL3D)
ASM_FUNC(Cdc_Construct, ??0CDC@@QAE@XZ)
ASM_FUNC(Cdc_Destruct, ??1CDC@@UAE@XZ)

#define HWND_OF(w) F(HWND, w, 28)

// [FUN_633cf0] afxMapHWND: this thread's HWND -> CWnd map
extern "C" BYTE* __stdcall Afx_MapHWND(BOOL create) {
    BYTE* mts = Afx_GetModuleThreadState();
    if (!F(BYTE*, mts, 20) && create) {
        void* old = CallStd<void*>(FUN_632631, (void*)$L_6373bb);
        BYTE* p = (BYTE*)operator new(72);
        F(BYTE*, mts, 20) = p ? CallThis<BYTE*>(FUN_6368e3, p, (void*)g_tempWndClass, 28, 1) : 0;
        CallStd(FUN_632631, old);
    }
    return F(BYTE*, mts, 20);
}

// [FUN_633810] lets the parent's control container attach a control site
extern "C" void __fastcall Wnd_AttachControlSite(BYTE* self, void*, BYTE* map) {
    if (!self || F(void*, self, 56)) return;
    BYTE* parent = CallThis<BYTE*>(FUN_6324f2, map + 4, GetParent(HWND_OF(self)));
    if (parent && F(void*, parent, 52)) CallVirt(F(void*, parent, 52), 140, self);
}

// [FUN_633d62] CWnd::FromHandle: permanent or temporary CWnd
extern "C" BYTE* __stdcall Wnd_FromHandle(HWND h) {
    BYTE* map = Afx_MapHWND(TRUE);
    BYTE* w = CallThis<BYTE*>(FUN_636948, map, h);
    Wnd_AttachControlSite(w, 0, map);
    return w;
}

// [FUN_633d89] CWnd::FromHandlePermanent
extern "C" BYTE* __stdcall Wnd_FromHandlePermanent(HWND h) {
    BYTE* map = Afx_MapHWND(FALSE);
    return map ? CallThis<BYTE*>(FUN_6324f2, map + 4, h) : 0;
}

// [FUN_633da7] CWnd::Attach
extern "C" BOOL __fastcall Wnd_Attach(BYTE* self, void*, HWND h) {
    if (!h) return FALSE;
    BYTE* map = Afx_MapHWND(TRUE);
    HWND_OF(self) = h;
    *CallThis<BYTE**>(FUN_632525, map + 4, h) = self;
    Wnd_AttachControlSite(self, 0, map);
    return TRUE;
}

// [FUN_633de0] CWnd::Detach
extern "C" HWND __fastcall Wnd_Detach(BYTE* self) {
    HWND h = HWND_OF(self);
    if (h) {
        BYTE* map = Afx_MapHWND(FALSE);
        if (map) CallThis(FUN_632575, map + 4, h);
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
    BYTE* permanent = CallThis<BYTE*>(FUN_6324f2, map + 4, HWND_OF(self));
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
    if (!last || last == Wnd_FromHandle(GetForegroundWindow()) || !CallThis<BOOL>(FUN_6337ad, last))
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
    if (msg == WM_ACTIVATE) CallStd(FUN_633aca, self, w, Wnd_FromHandle((HWND)l));
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
        case 1: r = CallThis<LRESULT>(pfn, self, CallStd<void*>(FUN_636c7c, w)); break;
        case 2: r = CallThis<LRESULT>(pfn, self, w); break;
        case 3: case 8:
            r = CallThis<LRESULT>(pfn, self, Wnd_FromHandle((HWND)w), (int)(short)ll, (UINT)lh); break;
        case 4: {                                   // WM_CTLCOLOR: AFX_CTLCOLOR* in lParam
            DWORD* cc = (DWORD*)l;
            BYTE dc[16], tmp[60];
            CallThis(Cdc_Construct, dc);
            F(DWORD, dc, 4) = cc[1];
            Wnd_Construct(tmp);
            HWND_OF(tmp) = (HWND)cc[0];
            BYTE* wnd = Wnd_FromHandlePermanent((HWND)cc[0]);
            if (!wnd) {
                if (F(BYTE*, self, 52)) {
                    void* site = CallThis<void*>(FUN_6324f2, F(BYTE*, self, 52) + 32, (HWND)cc[0]);
                    if (site) F(void*, tmp, 56) = site;
                }
                wnd = tmp;
            }
            r = CallThis<LRESULT>(pfn, self, dc, wnd, (UINT)cc[2]);
            F(DWORD, dc, 4) = 0;
            HWND_OF(tmp) = 0;
            Wnd_Destruct(tmp);
            CallThis(Cdc_Destruct, dc);
            break;
        }
        case 5: {
            DWORD* cc = (DWORD*)l;
            BYTE dc[16];
            CallThis(Cdc_Construct, dc);
            F(DWORD, dc, 4) = cc[1];
            r = CallThis<LRESULT>(pfn, self, dc, (UINT)cc[2]);
            F(DWORD, dc, 4) = 0;
            CallThis(Cdc_Destruct, dc);
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
        case 20: CallThis(pfn, self, CallStd<void*>(FUN_636c7c, w)); break;
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
        CallStd(FUN_633aca, Wnd_FromHandle(h), w, other);
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
