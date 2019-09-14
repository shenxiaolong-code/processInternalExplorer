#include <SystemExplorer/ProcessExplorer.h>
#include <MiniMPL/productSpecialDef.h>
#include "MiniMPL/valueCompare.hpp"
#include <SystemExplorer/ProcessInfoDef.h>
#include "SystemExplorer/WindowExplorer.h"

//http://geekswithblogs.net/BruceEitman/archive/2009/12/14/windows-ce-killing-an-application.aspx
//http://geekswithblogs.net/BruceEitman/archive/2008/05/14/windows-ce--using-toolhelpapi-to-list-running-processes.aspx

#ifdef UNDER_CE
#pragma comment(lib,"Toolhelp.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned ProcessFinder::EnumRunningProcesses( std::vector<DWORD>& rPIDs )
{
    DWORD aProcesses[1024]={0};
    DWORD pBytesReturned=0;
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &pBytesReturned ) )
        return 0;

    unsigned cProcesses = pBytesReturned / sizeof(DWORD);
    for (unsigned i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
            rPIDs.push_back( aProcesses[i] );
    }
    return rPIDs.size();
}

unsigned ProcessFinder::EnumRunningProcesses(std::vector<PROCESSENTRY32>& rProcess)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapShot == INVALID_HANDLE_VALUE)
    {
        outputTxtV(TEXT("GetRunningProcesses: Failed CreateToolhelp32Snapshot Error: %d\n"),GetLastError());
        return 0;
    }

    PROCESSENTRY32  oProcess={0};
    oProcess.dwSize = sizeof(PROCESSENTRY32);
    if(Process32First(hSnapShot, &oProcess))
    {
        rProcess.push_back(oProcess);
        while(Process32Next(hSnapShot, &oProcess))
        {
            rProcess.push_back(oProcess);
        }
    }

#ifdef UNDER_CE
    CloseToolhelp32Snapshot (hSnapShot);
#endif // UNDER_CE
    return rProcess.size();
}

bool ProcessFinder::findProcessByWnd( HWND hWnd,DWORD& dwPID )
{
    CHECK_NULL_ELSE_RETURN_VAL(hWnd,false);
    return TRUE==GetWindowThreadProcessId(hWnd,&dwPID);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProcessExplorer::ProcessExplorer(DWORD dwPID)
: m_processID(dwPID)
{
    ASSERT_AND_LOG(m_processID>0);
}

ProcessExplorer::~ProcessExplorer(void)
{
}

unsigned ProcessExplorer::getThreadsOfProcess(std::vector<ThreadInfo32>& rThrds)
{
    HANDLE  hThreadSnap= CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); 
    if (hThreadSnap == INVALID_HANDLE_VALUE) 
    {
        return 0;
    }	

    ThreadInfo32 oThrdInfo	= {0};
    oThrdInfo.m_entry.dwSize = sizeof(oThrdInfo.m_entry);
    if (Thread32First(hThreadSnap, &oThrdInfo.m_entry)) 
    { 
        do 
        { 		
            if (oThrdInfo.m_entry.th32OwnerProcessID == m_processID) 
            {
                rThrds.push_back(oThrdInfo);
            } 
        }while (Thread32Next(hThreadSnap, &oThrdInfo.m_entry)); 
    }

    CloseHandle (hThreadSnap); 		
    return rThrds.size()>0;
}

unsigned ProcessExplorer::getModulesOfProcess(std::vector<ModuleInfo32>& rModules )
{
    // Take a snapshot of all modules in the specified process. 	
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_processID); 
    if (hModuleSnap == INVALID_HANDLE_VALUE) 
    {
        return NULL;
    }        

    // Fill the size of the structure before using it. 
    ModuleInfo32 me32  = {0};
    me32.m_entry.dwSize = sizeof(me32.m_entry);     
    if (Module32First(hModuleSnap, &me32.m_entry)) 
    { 
        do 
        { 
            rModules.push_back(me32);
        } 
        while (Module32Next(hModuleSnap, &me32.m_entry)); 
    }	
    CloseHandle (hModuleSnap); 
    return rModules.size()>0;
}

bool ProcessExplorer::getMemoryOfProcess(ProcessMemoryInfo& pmi )
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE, m_processID );
    if (NULL == hProcess)
        return false;
    BOOL bResult = GetProcessMemoryInfo( hProcess, &pmi.m_entry, sizeof(pmi.m_entry));
    CloseHandle(hProcess);
    return TRUE == bResult;
}

bool ProcessExplorer::closeProcess(bool bForce/*=false*/ )
{
    WindowFinder wf;
    HWND hwnd = wf.findWindowHwndByProcessID(m_processID);
    if (!bForce && hwnd)
    {   //try to close application graceful
        return TRUE==PostMessage(hwnd,WM_QUIT,0,0);
    }
    else
    {   //force to terminate process rough
        HANDLE hApp ;
        bool ReturnValue = false;

        hApp = OpenProcess(PROCESS_TERMINATE, FALSE, m_processID);
        if( hApp )
        {
            if( !TerminateProcess( hApp, 999 ) )
            {
                outputTxtV(TEXT("KillApp: Failed to kill app, error %d\n"), GetLastError() );
            }            
            else
            {
                ReturnValue = true;
            }
        }
        else
            outputTxtV(TEXT("KillApp: %X is not currently running\n"), m_processID  );

        CloseHandle(hApp);
        return ReturnValue;
    }    
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProcessInfoDisplayer::ProcessInfoDisplayer( std::vector<PROCESSENTRY32> const& rProcess )
:m_rProcess(rProcess)
{
}

void ProcessInfoDisplayer::print()
{
    m_maxLenOfExeName = GetMaxProcessNameLength();
    outputTxt((TEXT("%-*s %8s %13s %9s %9s %10s\n"),
        m_maxLenOfExeName, TEXT("Process"),
        TEXT("PID"),
        TEXT("Base Priority"),
        TEXT("# Threads"),
        TEXT("Base Addr"),
        TEXT("Access Key")
        )) ;

    for (std::vector<PROCESSENTRY32>::const_iterator it=m_rProcess.begin();
        it != m_rProcess.end() ; ++it)
    {
        print(*it);
    }
}

void ProcessInfoDisplayer::print( PROCESSENTRY32 const& rP )
{
#ifdef UNDER_CE
    outputTxt(( TEXT("%-*s %8X %13d %9d %9X %10X\n"), 
        m_maxLenOfExeName, rP.szExeFile,
        rP.th32ProcessID,
        rP.pcPriClassBase,
        rP.cntThreads,
        rP.th32MemoryBase,
        rP.th32AccessKey
        ));
#else
    outputTxtV( TEXT("%-*s %8X %13d %9d \n"), 
        m_maxLenOfExeName, rP.szExeFile,
        rP.th32ProcessID,
        rP.pcPriClassBase,
        rP.cntThreads);
#endif

}

void ProcessInfoDisplayer::print( PROCESS_MEMORY_COUNTERS const& pmc )
{           
    outputTxtV( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
    outputTxtV( "\tPeakWorkingSetSize: 0x%08X\n",pmc.PeakWorkingSetSize );
    outputTxtV( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
    outputTxtV( "\tQuotaPeakPagedPoolUsage: 0x%08X\n",pmc.QuotaPeakPagedPoolUsage );
    outputTxtV( "\tQuotaPagedPoolUsage: 0x%08X\n",pmc.QuotaPagedPoolUsage );
    outputTxtV( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n",pmc.QuotaPeakNonPagedPoolUsage );
    outputTxtV( "\tQuotaNonPagedPoolUsage: 0x%08X\n",pmc.QuotaNonPagedPoolUsage );
    outputTxtV( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage ); 
    outputTxtV( "\tPeakPagefileUsage: 0x%08X\n",pmc.PeakPagefileUsage );
}

unsigned ProcessInfoDisplayer::GetMaxProcessNameLength()
{
    DWORD MaxLength = 0;
    DWORD CurrentLength;
    for( std::vector<PROCESSENTRY32>::const_iterator it = m_rProcess.begin(); 
        it != m_rProcess.end();++it)
    {
        CurrentLength = wcslen( (*it).szExeFile );
        if( MaxLength <  CurrentLength )
            MaxLength = CurrentLength;
    }
    return MaxLength;
}
