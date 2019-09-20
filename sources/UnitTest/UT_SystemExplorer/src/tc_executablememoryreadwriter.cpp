#include "../testconfig_SystemExplorer.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <SystemExplorer/executablememoryreadwriter.h>
* Author              : Shen.Xiaolong (at Thu 09/19/2019) , xlshen@126.com  xlshen2002@hotmail.com
* Copyright           : free to use / modify / sale in free and commercial software with those head comments.
* usage demo          : #define RUN_EXAMPLE_EXECUTABLEMEMORYREADWRITER to run this demo
***********************************************************************************************************************/
#define RUN_EXAMPLE_EXECUTABLEMEMORYREADWRITER

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_EXECUTABLEMEMORYREADWRITER
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_EXECUTABLEMEMORYREADWRITER
#endif

#if defined(RUN_EXAMPLE_) && !defined(COMPILE_EXAMPLE_)
    #define COMPILE_EXAMPLE_EXECUTABLEMEMORYREADWRITER
#endif

//#undef RUN_EXAMPLE_EXECUTABLEMEMORYREADWRITER
//#undef COMPILE_EXAMPLE_EXECUTABLEMEMORYREADWRITER

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_EXECUTABLEMEMORYREADWRITER
#include <SystemExplorer/executablememoryreadwriter.h>
#include <UnitTestKit/tc_tracer.h>

namespace UnitTest
{

    inline void TestCase_executablememoryreadwriter()
    {
        PrintTestcase();
        ASSERT_AND_LOG_INFO(0,(TXT("Not implemented")));	
        Static_Assert(0);
        ALWAYS_COMPILE_MSG("NO Test context for <SystemExplorer/executablememoryreadwriter.h>");

    }

#ifdef RUN_EXAMPLE_EXECUTABLEMEMORYREADWRITER
    InitRunFunc(TestCase_executablememoryreadwriter);
#else //else of RUN_EXAMPLE_EXECUTABLEMEMORYREADWRITER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_)=(outputTxtV(TXT("[Unit test run disabled] executablememoryreadwriter.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"executablememoryreadwriter.h\".")
    #endif
#endif  	// RUN_EXAMPLE_EXECUTABLEMEMORYREADWRITER
}

#else //else of COMPILE_EXAMPLE_EXECUTABLEMEMORYREADWRITER
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_)=(outputTxtV(TXT("[Unit test compile disabled] executablememoryreadwriter.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"executablememoryreadwriter.h\".")
    #endif
#endif // COMPILE_EXAMPLE_EXECUTABLEMEMORYREADWRITER

