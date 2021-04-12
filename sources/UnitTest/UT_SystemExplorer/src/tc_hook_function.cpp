#include "../testconfig_SystemExplorer.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <SystemExplorer/functionhook.h>
* Author              : Shen.Xiaolong (2009-2019)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : vs2015
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_FUNCTIONHOOK to run this demo
***********************************************************************************************************************/
//#define RUN_EXAMPLE_FUNCTIONHOOK

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_FUNCTIONHOOK
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_FUNCTIONHOOK
#endif

#if defined(RUN_EXAMPLE_) && !defined(COMPILE_EXAMPLE_)
    #define COMPILE_EXAMPLE_FUNCTIONHOOK
#endif

//#undef RUN_EXAMPLE_FUNCTIONHOOK
//#undef COMPILE_EXAMPLE_FUNCTIONHOOK

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_FUNCTIONHOOK
#include <SystemExplorer/hook_function.h>
#include <UnitTestKit/tc_tracer.h>
#include "tc_functionhook_material.h"

std::map<PVOID, PVOID> hookApis;

namespace UnitTest
{
    HINSTANCE __stdcall MyShellExecuteA(
        HWND   hwnd,
        LPCSTR lpOperation,
        LPCSTR lpFile,
        LPCSTR lpParameters,
        LPCSTR lpDirectory,
        INT    nShowCmd
    )
    {
        std::stringstream ss;
        ss << "successful to hook ShellExecuteA!";
        if (lpOperation)
        {
            ss << "\r\nlpOperation : " << lpOperation;
        }
        if (lpFile)
        {
            ss << "\r\nlpFile      : " << lpFile;
        }
        if (lpParameters)
        {
            ss << "\r\nlpParameters: " << lpParameters;
        }
        if (lpDirectory)
        {
            ss << "\r\nlpDirectory : " << lpDirectory;
        }
        ss << std::endl;
        
        MessageBoxA(NULL, ss.str().c_str(), "hook system function as user function:", MB_OK);
        return 0;
    }

	//////////////////////////////////////////////////////////////////////////
    inline void TestCase_functionhook_system_as_user()
    {
        PrintTestcase();
        using namespace Win_x86;

        UserFuncHook ufh;
        UserFuncHook::ThunkType pThunk_sys_as_user = ufh.hookUserFunc(ShellExecuteA, MyShellExecuteA);
        hookApis.insert(std::make_pair(MyShellExecuteA, getOldFunc(pThunk_sys_as_user)));
        ::ShellExecuteA(0, "open", "www.163.com", 0, 0, SW_SHOWNORMAL);
        ufh.restoreHook(pThunk_sys_as_user);
        hookApis.erase(MyShellExecuteA);
    }

    inline void TestCase_functionhook_user()
    {
        PrintTestcase();

		using namespace Win_x86;

		//test user function
		oldFunc("call oldFunc before hook.\r\n");		
		UserFuncHook ufh;
		UserFuncHook::ThunkType pThunk_user = ufh.hookUserFunc(oldFunc, newFunc);
		hookApis.insert(std::make_pair(newFunc, getOldFunc(pThunk_user)));
		oldFunc("call oldFunc after hook.\r\n");
		ufh.restoreHook(pThunk_user);
		oldFunc("call oldFunc after store hook.\r\n");
		hookApis.erase(newFunc);
    }

	inline void TestCase_functionhook_exported()
	{
		PrintTestcase();

		using namespace Win_x86;
		//test system exported API
		OutputDebugStringA("call OutputDebugStringA before hook.\r\n");

		CExportedFuncHook pe;
		CExportedFuncHook::ThunkType pThunk_exported = pe.hookImportedAPI(OutputDebugStringA, myOutputDebugStringA);
		hookApis.insert(std::make_pair(myOutputDebugStringA, getOldFunc(pThunk_exported)));
		OutputDebugStringA("call OutputDebugStringA after hook.\r\n");
		pe.restoreHook(pThunk_exported);
		OutputDebugStringA("call OutputDebugStringA when restore hook.\r\n");
		hookApis.erase(myOutputDebugStringA);
	}

	inline void TestCase_functionhook_MessageBoxA()
	{
		PrintTestcase();

		using namespace Win_x86;
		//test system exported API
		OutputDebugStringA("call OutputDebugStringA before hook.\r\n");

		CExportedFuncHook pe;
		CExportedFuncHook::ThunkType pThunk_exported = pe.hookImportedAPI(OutputDebugStringA, myOutputDebugStringA);
		hookApis.insert(std::make_pair(myOutputDebugStringA, getOldFunc(pThunk_exported)));
		OutputDebugStringA("call OutputDebugStringA after hook.\r\n");
		pe.restoreHook(pThunk_exported);
		OutputDebugStringA("call OutputDebugStringA when restore hook.\r\n");
		hookApis.erase(myOutputDebugStringA);
	}

#ifdef RUN_EXAMPLE_FUNCTIONHOOK
    InitRunFunc(TestCase_functionhook_exported);
	//InitRunFunc(TestCase_functionhook_MessageBoxA);
	InitRunFunc(TestCase_functionhook_user);
    InitRunFunc(TestCase_functionhook_system_as_user);
#else //else of RUN_EXAMPLE_FUNCTIONHOOK

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_)=(outputTxtV(TXT("[Unit test run disabled] functionhook.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"functionhook.h\".")
    #endif
#endif  	// RUN_EXAMPLE_FUNCTIONHOOK
}

#else //else of COMPILE_EXAMPLE_FUNCTIONHOOK
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_)=(outputTxt((TXT("[Unit test compile disabled] functionhook.h\n%s(%d)\n"),TXT(__FILE__),__LINE__)),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"functionhook.h\".")
    #endif
#endif // COMPILE_EXAMPLE_FUNCTIONHOOK

