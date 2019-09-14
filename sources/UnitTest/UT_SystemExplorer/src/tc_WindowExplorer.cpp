#include "../testconfig_SystemExplorer.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <SystemExplorer\WindowExplorer.h>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016-12-07 17:22:19.33)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_WINDOWEXPLORER to run this demo
***********************************************************************************************************************/
//#define RUN_EXAMPLE_WINDOWEXPLORER

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_WINDOWEXPLORER
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_WINDOWEXPLORER
#endif

#if defined(RUN_EXAMPLE_) && !defined(COMPILE_EXAMPLE_)
    #define COMPILE_EXAMPLE_WINDOWEXPLORER
#endif

//#undef RUN_EXAMPLE_WINDOWEXPLORER
//#undef COMPILE_EXAMPLE_WINDOWEXPLORER

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_WINDOWEXPLORER
#include <SystemExplorer/WindowExplorer.h>
#include <UnitTestKit/tc_tracer.h>
#include <SystemExplorer/HoverWindowMonitor.h>

namespace UnitTest
{

    inline void TestCase_WindowExplorer()
    {
        PrintTestcase();
        HoverWindowMonitor      hwm;
        HoverWindowExplorer     hwd;
        hwm.setHoverWindowReceiver(makeHoverWindowReceiver2p(hwd,&HoverWindowExplorer::onNewWindow));
        hwm.start();
        
        bool bTest = true;
        //while(bTest)
        {
            Sleep(5000);
        }
    }

#ifdef RUN_EXAMPLE_WINDOWEXPLORER
    InitRunFunc(TestCase_WindowExplorer);
#else //else of RUN_EXAMPLE_WINDOWEXPLORER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_)=(outputTxtV(TXT("[Unit test run disabled] WindowExplorer.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"WindowExplorer.h\".")
    #endif
#endif  	// RUN_EXAMPLE_WINDOWEXPLORER
}

#else //else of COMPILE_EXAMPLE_WINDOWEXPLORER
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_)=(outputTxtV(TXT("[Unit test compile disabled] WindowExplorer.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"WindowExplorer.h\".")
    #endif
#endif // COMPILE_EXAMPLE_WINDOWEXPLORER

