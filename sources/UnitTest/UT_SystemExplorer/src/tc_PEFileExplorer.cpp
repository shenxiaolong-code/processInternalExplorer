#include "../testconfig_SystemExplorer.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <SystemExplorer\PEFileExplorer.h>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016-12-07 18:28:02.88)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_PEFILEEXPLORER to run this demo
***********************************************************************************************************************/
//#define RUN_EXAMPLE_PEFILEEXPLORER

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_PEFILEEXPLORER
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_PEFILEEXPLORER
#endif

#if defined(RUN_EXAMPLE_) && !defined(COMPILE_EXAMPLE_)
    #define COMPILE_EXAMPLE_PEFILEEXPLORER
#endif

//#undef RUN_EXAMPLE_PEFILEEXPLORER
//#undef COMPILE_EXAMPLE_PEFILEEXPLORER

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_PEFILEEXPLORER
#include <SystemExplorer/PEFileExplorer.h>
#include <UnitTestKit/tc_tracer.h>

namespace UnitTest
{

    inline void TestCase_PEFileExplorer()
    {
        PrintTestcase();
        PEFileExplorer pfe;
        PEHeader ph;
        pfe.getPEHeader(ph);

        std::vector<FuncThunk> fts;
        pfe.enumFunc(PEFileExplorer::F_All);
        

    }

#ifdef RUN_EXAMPLE_PEFILEEXPLORER
    InitRunFunc(TestCase_PEFileExplorer);
#else //else of RUN_EXAMPLE_PEFILEEXPLORER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_)=(outputTxtV(TXT("[Unit test run disabled] PEFileExplorer.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"PEFileExplorer.h\".")
    #endif
#endif  	// RUN_EXAMPLE_PEFILEEXPLORER
}

#else //else of COMPILE_EXAMPLE_PEFILEEXPLORER
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_)=(outputTxtV(TXT("[Unit test compile disabled] PEFileExplorer.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"PEFileExplorer.h\".")
    #endif
#endif // COMPILE_EXAMPLE_PEFILEEXPLORER

