#include "Python.h"
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include "compat.h"

/*
 * Note, designed for modules that are certainly loaded (kernel32.dll etc.), otherwise
 * rewrite to LoadLibrary[/FreeLibrary] instead of GetProcAddress(GetModuleHandleW()).
 */
#define DEFINE_COMPAT_FN(Module, RetType, Name, ArgList, CallList)                     \
                                                                                       \
RetType fallback_##Name ArgList;                                                       \
                                                                                       \
RetType COMPAT_FN(Name) ArgList                                                        \
{                                                                                      \
    typedef RetType (WINAPI *p##Name) ArgList;                                         \
    static p##Name pFunc = NULL;                                                       \
    static int initialized = 0;                                                        \
    if (!initialized) {                                                                \
        pFunc = (p##Name)GetProcAddress(GetModuleHandleA(#Module ".dll"), #Name);      \
        initialized = 1;                                                               \
    }                                                                                  \
    return pFunc ? pFunc CallList : fallback_##Name CallList;                          \
}                                                                                      \
                                                                                       \
RetType fallback_##Name ArgList

DEFINE_COMPAT_FN(kernel32, DLL_DIRECTORY_COOKIE, AddDllDirectory, (PCWSTR NewDirectory), (NewDirectory))
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return NULL;
}

DEFINE_COMPAT_FN(kernel32, BOOL, RemoveDllDirectory, (DLL_DIRECTORY_COOKIE Cookie), (Cookie))
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

DEFINE_COMPAT_FN(kernel32, HRESULT, CopyFile2, (PCWSTR pwszExistingFileName, PCWSTR pwszNewFileName, COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters), (pwszExistingFileName, pwszNewFileName, pExtendedParameters))
{
    if (pwszExistingFileName == NULL || pwszNewFileName == NULL) {
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    }

    if (!pExtendedParameters) {
        BOOL result = CopyFileW(pwszExistingFileName, pwszNewFileName, FALSE);
        return result ? S_OK : HRESULT_FROM_WIN32(GetLastError());
    }
    else {
        // TODO: PCOPYFILE2_PROGRESS_ROUTINE --> LPPROGRESS_ROUTINE
        BOOL result = CopyFileExW(
            pwszExistingFileName,
            pwszNewFileName,
            NULL,
            NULL,
            pExtendedParameters->pfCancel,
            pExtendedParameters->dwCopyFlags
        );
        return result ? S_OK : HRESULT_FROM_WIN32(GetLastError());
    }
}

DEFINE_COMPAT_FN(kernel32, BOOL, SetWaitableTimerEx,
    (HANDLE hTimer, const LARGE_INTEGER* lpDueTime, LONG lPeriod, PTIMERAPCROUTINE pfnCompletionRoutine, LPVOID lpArgToCompletionRoutine, PREASON_CONTEXT WakeContext, ULONG TolerableDelay),
    (hTimer, lpDueTime, lPeriod, pfnCompletionRoutine, lpArgToCompletionRoutine, WakeContext, TolerableDelay))
{
    return SetWaitableTimer(hTimer, lpDueTime, lPeriod, pfnCompletionRoutine, lpArgToCompletionRoutine, FALSE);
}

DEFINE_COMPAT_FN(kernel32, DWORD, GetActiveProcessorCount, (WORD GroupNumber), (GroupNumber))
{
    SYSTEM_INFO sinfo;
    GetSystemInfo(&sinfo);
    return sinfo.dwNumberOfProcessors;
}

#include "compat_pathcch.c"

DEFINE_COMPAT_FN(kernelbase, HRESULT, PathCchCombineEx, (WCHAR* out, SIZE_T size, const WCHAR* path1, const WCHAR* path2, DWORD flags), (out, size, path1, path2, flags))
{
    return _PathCchCombineEx(out, size, path1, path2, flags);
}

DEFINE_COMPAT_FN(kernelbase, HRESULT, PathCchSkipRoot, (const WCHAR* path, const WCHAR** root_end), (path, root_end))
{
    return _PathCchSkipRoot(path, root_end);
}
