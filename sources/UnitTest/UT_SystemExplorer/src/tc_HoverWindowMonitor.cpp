#include "../testconfig_SystemExplorer.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <SystemExplorer\HoverWindowMonitor.h>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016-12-08  9:44:36.52)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_HOVERWINDOWMONITOR to run this demo
***********************************************************************************************************************/
//#define RUN_EXAMPLE_HOVERWINDOWMONITOR

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_HOVERWINDOWMONITOR
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_HOVERWINDOWMONITOR
#endif

#if defined(RUN_EXAMPLE_) && !defined(COMPILE_EXAMPLE_)
    #define COMPILE_EXAMPLE_HOVERWINDOWMONITOR
#endif

//#undef RUN_EXAMPLE_HOVERWINDOWMONITOR
//#undef COMPILE_EXAMPLE_HOVERWINDOWMONITOR

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_HOVERWINDOWMONITOR
#include <SystemExplorer/HoverWindowMonitor.h>
#include <UnitTestKit/tc_tracer.h>

namespace UnitTest
{
    void onNewHoverWindow(HWND hWnd,POINT pt)
    {
        static HWND hLastHwnd = NULL;        
        if (hLastHwnd != hWnd)
        {
            outputTxtV(L"%-21s[0x%08X]\n",L"current window HWND ",hWnd);
            outputTxtV(L"%-21s[%04d-%04d]\n",L"current mouse point ",hWnd,pt.x,pt.y);
            hLastHwnd = hWnd;
        }
        else
        {
            outputTxtV(L"%-21s[%04d-%04d]\n",L" ",pt.x,pt.y);
        }        
    }

    inline void TestCase_HoverWindowMonitor()
    {
        PrintTestcase();

        HoverWindowMonitor hwm;
        hwm.setHoverWindowReceiver(makeHoverWindowReceiver2p(onNewHoverWindow));
        hwm.start();

        bool bTest = true;
        //while(bTest)
        {
            Sleep(5000);
        }
    }

#ifdef RUN_EXAMPLE_HOVERWINDOWMONITOR
    InitRunFunc(TestCase_HoverWindowMonitor);
#else //else of RUN_EXAMPLE_HOVERWINDOWMONITOR

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_)=(outputTxtV(TXT("[Unit test run disabled] HoverWindowMonitor.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"HoverWindowMonitor.h\".")
    #endif
#endif  	// RUN_EXAMPLE_HOVERWINDOWMONITOR
}

#else //else of COMPILE_EXAMPLE_HOVERWINDOWMONITOR
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_)=(outputTxtV(TXT("[Unit test compile disabled] HoverWindowMonitor.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"HoverWindowMonitor.h\".")
    #endif
#endif // COMPILE_EXAMPLE_HOVERWINDOWMONITOR

