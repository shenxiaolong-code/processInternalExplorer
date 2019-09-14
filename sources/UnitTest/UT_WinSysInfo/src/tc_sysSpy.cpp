#include "../testconfig_WinSysInfo.h"
#include <MiniMPL/macroDef.h>
/***********************************************************************************************************************
* Description         : test file for <WinSysInfo\sysSpy.h>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016/05/30  9:04:13.58)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_SYSSPY to run this demo
***********************************************************************************************************************/
#define RUN_EXAMPLE_SYSSPY

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_SYSSPY
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_SYSSPY
#endif

#if defined(RUN_EXAMPLE_SYSSPY) && !defined(COMPILE_EXAMPLE_SYSSPY)
    #define COMPILE_EXAMPLE_SYSSPY
#endif

//#undef RUN_EXAMPLE_SYSSPY
//#undef COMPILE_EXAMPLE_SYSSPY

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_SYSSPY
#include <WinSysInfo/sysSpy.h>
#include <UnitTestKit/tc_tracer.h>
#include <MiniMPL/arrayTrait.hpp>
#include <MiniMPL/macro_init.h>

namespace UnitTest
{

    inline void TestCase_sysSpy()
    {
        PrintTestcase();

        using namespace Util;

        CEnumSys obj;
        ASSERT_AND_LOG(obj.GetClassInfoByMouse());

        stlChar szOldTitle[MAX_PATH];
        GetConsoleTitle(szOldTitle, arraySize(szOldTitle));
        SetConsoleTitle(TXT("press any key to terminate."));
        getchar();
        obj.stop();
        SetConsoleTitle(szOldTitle);
    }

#ifdef RUN_EXAMPLE_SYSSPY
    InitRunFunc(TestCase_sysSpy);
#else //else of RUN_EXAMPLE_SYSSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_SYSSPY)=(outputTxtV(TXT("[Unit test run disabled] sysSpy.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"sysSpy.h\".")
    #endif
#endif  	// RUN_EXAMPLE_SYSSPY
}

#else //else of COMPILE_EXAMPLE_SYSSPY
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_SYSSPY)=(outputTxtV(TXT("[Unit test compile disabled] sysSpy.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"sysSpy.h\".")
    #endif
#endif // COMPILE_EXAMPLE_SYSSPY

