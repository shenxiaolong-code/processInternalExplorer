#include "../testconfig_Excel.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <Excel\ExcelFileFormat.h>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016/04/20 11:23:11.92)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_EXCELFILEFORMAT to run this demo
***********************************************************************************************************************/
#define RUN_EXAMPLE_EXCELFILEFORMAT

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_EXCELFILEFORMAT
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_EXCELFILEFORMAT
#endif

#if defined(RUN_EXAMPLE_EXCELFILEFORMAT) && !defined(COMPILE_EXAMPLE_EXCELFILEFORMAT)
    #define COMPILE_EXAMPLE_EXCELFILEFORMAT
#endif

//#undef RUN_EXAMPLE_EXCELFILEFORMAT
#undef COMPILE_EXAMPLE_EXCELFILEFORMAT

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_EXCELFILEFORMAT
#include <Excel/ExcelFileFormat.h>
#include <UnitTestKit/tc_tracer.h>

namespace UnitTest
{

    inline void TestCase_ExcelFileFormat()
    {
        PrintTestcase();
        ASSERT_AND_LOG_INFO(0,(TXT("Not implemented")));	
        Static_Assert(0);

    }

#ifdef RUN_EXAMPLE_EXCELFILEFORMAT
    InitRunFunc(TestCase_ExcelFileFormat);
#else //else of RUN_EXAMPLE_EXCELFILEFORMAT

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_EXCELFILEFORMAT)=(outputTxtV(TXT("[Unit test run disabled] ExcelFileFormat.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"ExcelFileFormat.h\".")
    #endif
#endif  	// RUN_EXAMPLE_EXCELFILEFORMAT
}

#else //else of COMPILE_EXAMPLE_EXCELFILEFORMAT
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    #include <MiniMPL/productSpecialDef.h>
    #include <MiniMPL/macroDef.h>
    GLOBALVAR(COMPILE_EXCELFILEFORMAT)=(outputTxtV(TXT("[Unit test compile disabled] ExcelFileFormat.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"ExcelFileFormat.h\".")
    #endif
#endif // COMPILE_EXAMPLE_EXCELFILEFORMAT

