#ifndef __PROCESSEXPLORER_H__
#define __PROCESSEXPLORER_H__
/***********************************************************************************************************************
* Description         : explorer process information in current system
* Author              : Shen.Xiaolong (Shen Tony) (2009-2016)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016-12-07 13:49:33.06)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
*                       latest Version of The Code Project Open License (CPOL : http://www.codeproject.com/)
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <MiniMPL/valueCompare.hpp>
#include <SystemExplorer/ProcessInfoDef.h>
#include <vector>
#include <MiniMPL/stdAlgorithmEx.hpp>
#include <MiniMPL/tester.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ProcessFinder
{
    unsigned EnumRunningProcesses(std::vector<DWORD>& rPIDs);
    unsigned EnumRunningProcesses(std::vector<PROCESSENTRY32>& rProcess);
    bool     findProcessByWnd(HWND hWnd,DWORD& dwPID);

    template<typename T>    
    bool     findProcessByName( T const& rExeName,DWORD& dwPID )
    {
        return findProcessBy(rExeName,&PROCESSENTRY32::szExeFile,dwPID);
    }

    template<typename T,typename TM>    
    bool     findProcessBy(T const& rVal,TM PROCESSENTRY32::*const ptr,DWORD& dwPID )
    {
        std::vector<PROCESSENTRY32> ps;
        if (EnumRunningProcesses(ps)>0)
        { 
            PROCESSENTRY32* pP=MiniMPL::find(ps,MiniMPL::makeUnaryTester_Result(ptr,rVal,MiniMPL::CEqual()));
            if (pP)
            {
                dwPID = pP->th32ProcessID;
                return true;
            }
        }
        return false;
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ProcessExplorer
{
public:
    ProcessExplorer(DWORD dwPID);
    virtual ~ProcessExplorer(void);

    unsigned getThreadsOfProcess(std::vector<ThreadInfo32>& rThrds);
    unsigned getModulesOfProcess(std::vector<ModuleInfo32>& rModules);
    bool     getMemoryOfProcess(ProcessMemoryInfo& pmi);

    bool     closeProcess(bool bForce=false);

protected:
    DWORD       m_processID;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ProcessInfoDisplayer 
{
    ProcessInfoDisplayer(std::vector<PROCESSENTRY32> const& rProcess);
    void print();
    void print(PROCESSENTRY32 const& rP);
    void print(PROCESS_MEMORY_COUNTERS const& pmc);

protected:
    unsigned GetMaxProcessNameLength();

protected:
    unsigned                            m_maxLenOfExeName;
    std::vector<PROCESSENTRY32> const&  m_rProcess;
};


#endif // __PROCESSEXPLORER_H__
