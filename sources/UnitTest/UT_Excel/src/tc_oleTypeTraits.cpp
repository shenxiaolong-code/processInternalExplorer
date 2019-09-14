#include "../testconfig_Excel.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <Excel\oleTypeTraits.hpp>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016/04/20 11:23:24.48)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_OLETYPETRAITS to run this demo
***********************************************************************************************************************/
#define RUN_EXAMPLE_OLETYPETRAITS

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_OLETYPETRAITS
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_OLETYPETRAITS
#endif

#if defined(RUN_EXAMPLE_OLETYPETRAITS) && !defined(COMPILE_EXAMPLE_OLETYPETRAITS)
    #define COMPILE_EXAMPLE_OLETYPETRAITS
#endif

//#undef RUN_EXAMPLE_OLETYPETRAITS
#undef COMPILE_EXAMPLE_OLETYPETRAITS

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_OLETYPETRAITS
#include <Excel/oleTypeTraits.hpp>
#include <UnitTestKit/tc_tracer.h>

namespace UnitTest
{

    inline void TestCase_oleTypeTraits()
    {
        PrintTestcase();
        ASSERT_AND_LOG_INFO(0,(TXT("Not implemented")));	
        Static_Assert(0);

    }

#ifdef RUN_EXAMPLE_OLETYPETRAITS
    InitRunFunc(TestCase_oleTypeTraits);
#else //else of RUN_EXAMPLE_OLETYPETRAITS

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_OLETYPETRAITS)=(outputTxtV(TXT("[Unit test run disabled] oleTypeTraits.hpp\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"oleTypeTraits.hpp\".")
    #endif
#endif  	// RUN_EXAMPLE_OLETYPETRAITS
}

#else //else of COMPILE_EXAMPLE_OLETYPETRAITS
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    #include <MiniMPL/productSpecialDef.h>
    #include <MiniMPL/macroDef.h>
    GLOBALVAR(COMPILE_OLETYPETRAITS)=(outputTxtV(TXT("[Unit test compile disabled] oleTypeTraits.hpp\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"oleTypeTraits.hpp\".")
    #endif
#endif // COMPILE_EXAMPLE_OLETYPETRAITS

