#include "../testconfig_SystemExplorer.h"
/***********************************************************************************************************************
* Description         : test file for <SystemExplorer/exceptionhelper.h>
* Author              : Shen.Xiaolong (2009-2019)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : vs2015
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_EXCEPTIONHELPER to run this demo
***********************************************************************************************************************/
#define RUN_EXAMPLE_EXCEPTIONHELPER

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_EXCEPTIONHELPER
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_EXCEPTIONHELPER
#endif

#if defined(RUN_EXAMPLE_) && !defined(COMPILE_EXAMPLE_)
    #define COMPILE_EXAMPLE_EXCEPTIONHELPER
#endif

//#undef RUN_EXAMPLE_EXCEPTIONHELPER
//#undef COMPILE_EXAMPLE_EXCEPTIONHELPER

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_EXCEPTIONHELPER
#include <SystemExplorer/exceptionhelper.h>
#include <UnitTestKit/tc_tracer.h>
#include <MiniMPL/macro_init.h>
#include <Shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

namespace UnitTest
{
	void expTestFunc1()
	{
		RaiseException(1, 2, 0, NULL);
	}
	void expTestFunc22()
	{
		expTestFunc1();
	}
	void expTestFunc333()
	{
		expTestFunc22();
	}

	DWORD WINAPI ExpTestThread(PVOID)
	{
		ExceptionHelper eh;
		auto pBack = eh.enableExceptionWrapper(false, true);
		Sleep(500);
		expTestFunc333();	
		eh.disableExceptionWrapper(pBack);
		return 1;
	}

    inline void TestCase_exceptionhelper()
    {
        PrintTestcase();
		ExceptionHelper eh;
		auto pBack = eh.enableExceptionWrapper(true, true);
		HANDLE hThrd = CreateThread(NULL, 0, ExpTestThread, NULL, 0, NULL);
		WaitForSingleObject(hThrd, INFINITE);
		eh.disableExceptionWrapper(pBack);
    }

	inline void TestCase_DumpGenerater()
	{
		PrintTestcase();
				
		char modulename[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, modulename, MAX_PATH);
		PathRemoveFileSpecA(modulename);
		std::string dmpPath = modulename;
		dmpPath += "\\TestCase_DumpGenerater.dmp";

		DumpGenerater hlp(GetCurrentProcessId(), GetCurrentThreadId());
		hlp.WriteDump(dmpPath.c_str(), NULL);
		//hlp.WriteDump("D:\\temp\\A\\test.dmp", NULL);
	}

#ifdef RUN_EXAMPLE_EXCEPTIONHELPER
    InitRunFunc(TestCase_exceptionhelper);
	InitRunFunc(TestCase_DumpGenerater);
#else //else of RUN_EXAMPLE_EXCEPTIONHELPER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_)=(outputTxt((TXT("[Unit test run disabled] exceptionhelper.h\n%s(%d)\n"),TXT(__FILE__),__LINE__)),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"exceptionhelper.h\".")
    #endif
#endif  	// RUN_EXAMPLE_EXCEPTIONHELPER
}

#else //else of COMPILE_EXAMPLE_EXCEPTIONHELPER
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_)=(outputTxt((TXT("[Unit test compile disabled] exceptionhelper.h\n%s(%d)\n"),TXT(__FILE__),__LINE__)),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"exceptionhelper.h\".")
    #endif
#endif // COMPILE_EXAMPLE_EXCEPTIONHELPER

