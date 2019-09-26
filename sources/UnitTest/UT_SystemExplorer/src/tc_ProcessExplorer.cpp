#include "../testconfig_SystemExplorer.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <SystemExplorer\ProcessExplorer.h>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016-12-07 13:49:33.12)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_PROCESSEXPLORER to run this demo
***********************************************************************************************************************/
//#define RUN_EXAMPLE_PROCESSEXPLORER

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_PROCESSEXPLORER
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_PROCESSEXPLORER
#endif

#if defined(RUN_EXAMPLE_) && !defined(COMPILE_EXAMPLE_)
    #define COMPILE_EXAMPLE_PROCESSEXPLORER
#endif

//#undef RUN_EXAMPLE_PROCESSEXPLORER
//#undef COMPILE_EXAMPLE_PROCESSEXPLORER

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_PROCESSEXPLORER
#include <SystemExplorer/ProcessExplorer.h>
#include <UnitTestKit/tc_tracer.h>

namespace UnitTest
{
    inline void TestCase_ProcessExplorer()
    {
        PrintTestcase();
        ProcessFinder pf;
        std::vector<PROCESSENTRY32> ps;
        pf.enumRunningProcesses(ps);

        ProcessInfoDisplayer pd(ps);
        pd.print();

        DWORD pid=0;
        pf.findProcessByName("notepad.exe",pid);
        if (0!=pid)
        {
            ProcessExplorer pe(pid);
            pe.closeProcess(false);
        }

        ProcessExplorer pe1(ps[5].th32ProcessID);
        std::vector<ThreadInfo32> rThrds;
        pe1.getThreadsOfProcess(rThrds);

        ProcessMemoryInfo pmc;
        pe1.getMemoryOfProcess(pmc);        
    }

#ifdef RUN_EXAMPLE_PROCESSEXPLORER
    InitRunFunc(TestCase_ProcessExplorer);
#else //else of RUN_EXAMPLE_PROCESSEXPLORER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_)=(outputTxtV(TXT("[Unit test run disabled] ProcessExplorer.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"ProcessExplorer.h\".")
    #endif
#endif  	// RUN_EXAMPLE_PROCESSEXPLORER
}

#else //else of COMPILE_EXAMPLE_PROCESSEXPLORER
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_)=(outputTxtV(TXT("[Unit test compile disabled] ProcessExplorer.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"ProcessExplorer.h\".")
    #endif
#endif // COMPILE_EXAMPLE_PROCESSEXPLORER

