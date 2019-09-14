#include "../testconfig_Excel.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <Excel\ExcelDataType.h>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016/04/20 11:22:42.74)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_EXCELDATATYPE to run this demo
***********************************************************************************************************************/
#define RUN_EXAMPLE_EXCELDATATYPE

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_EXCELDATATYPE
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_EXCELDATATYPE
#endif

#if defined(RUN_EXAMPLE_EXCELDATATYPE) && !defined(COMPILE_EXAMPLE_EXCELDATATYPE)
    #define COMPILE_EXAMPLE_EXCELDATATYPE
#endif

//#undef RUN_EXAMPLE_EXCELDATATYPE
#undef COMPILE_EXAMPLE_EXCELDATATYPE

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_EXCELDATATYPE
#include <Excel/ExcelDataType.h>
#include <UnitTestKit/tc_tracer.h>

namespace UnitTest
{

    inline void TestCase_ExcelDataType()
    {
        PrintTestcase();
        ASSERT_AND_LOG_INFO(0,(TXT("Not implemented")));	
        Static_Assert(0);

    }

#ifdef RUN_EXAMPLE_EXCELDATATYPE
    InitRunFunc(TestCase_ExcelDataType);
#else //else of RUN_EXAMPLE_EXCELDATATYPE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_EXCELDATATYPE)=(outputTxtV(TXT("[Unit test run disabled] ExcelDataType.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"ExcelDataType.h\".")
    #endif
#endif  	// RUN_EXAMPLE_EXCELDATATYPE
}

#else //else of COMPILE_EXAMPLE_EXCELDATATYPE
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    #include <MiniMPL/productSpecialDef.h>
    #include <MiniMPL/macroDef.h>
    GLOBALVAR(COMPILE_EXCELDATATYPE)=(outputTxtV(TXT("[Unit test compile disabled] ExcelDataType.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"ExcelDataType.h\".")
    #endif
#endif // COMPILE_EXAMPLE_EXCELDATATYPE

