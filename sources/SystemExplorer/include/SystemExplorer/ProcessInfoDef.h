#ifndef PROCESSINFODEF_H__
#define PROCESSINFODEF_H__
//redefine struct to hold more info
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

typedef struct _ThreadInfo32 
{         
    THREADENTRY32               m_entry;     //tlhelp32.h
} ThreadInfo32,*LPThreadInfo32;

typedef struct _ModuleInfo32 
{
    MODULEENTRY32               m_entry;     //tlhelp32.h
} ModuleInfo32,*LPModuleInfo32;

typedef struct _ProcessMemoryInfo 
{
    PROCESS_MEMORY_COUNTERS     m_entry;     //psapi.h
} ProcessMemoryInfo,*LPProcessMemoryInfo;

typedef struct _tagProcWildInfo
{
    HWND Hwnd;
    stlChar szWndTitle[256];
    stlChar szClassName[128];
} ProcWildInfo,*lpProcWildInfo;

/*/
typedef struct _ProcessInfo32 
{
    stlSmartptr<PROCESSENTRY32>     ProcInfo;
    stlSmartptr<ProcessMemoryInfo>  ProcMemInfo;
    stlSmartptr<ProcWildInfo>       pWildInfo; 
    std::vector<ModuleInfo32>       m_moduleEntrys;
    std::vector<ThreadInfo32>       m_thrdInfos;
} ProcessInfo32,*LPProcessInfo32;
//*/
#endif // PROCESSINFODEF_H__
