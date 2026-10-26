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
