#include "../testconfig_SystemExplorer.h"
/***********************************************************************************************************************
* Description         : test file for <StackExplorer\StackExplorer.h>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016/04/20 10:11:05.38)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_STACKEXPLORER to run this demo
***********************************************************************************************************************/
//#define RUN_EXAMPLE_STACKEXPLORER

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_STACKEXPLORER
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_STACKEXPLORER
#endif

#if defined(RUN_EXAMPLE_STACKEXPLORER) && !defined(COMPILE_EXAMPLE_STACKEXPLORER)
    #define COMPILE_EXAMPLE_STACKEXPLORER
#endif

//#undef RUN_EXAMPLE_STACKEXPLORER
//#undef COMPILE_EXAMPLE_STACKEXPLORER

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_STACKEXPLORER
#include <SystemExplorer/StackExplorer.h>
#include <UnitTestKit/tc_tracer.h>
#include <MiniMPL/macro_init.h>
#include <time.h>

namespace UnitTest
{
    int     g_int = 0;
    int*    g_intP = NULL;

    int Func5()
    {
        StackExplorer se(GetCurrentProcess());
		StackInfoDisplayer sip;

		PVOID pHere = Func5;
		pHere = PVOID(DWORD(pHere)+0x100);
		AddressInfo		iAddr;
		se.resloveAddrInfo(pHere, iAddr);
		sip.print(iAddr);
		pHere = se.getCallerAddr(1);
		se.resloveAddrInfo(pHere, iAddr);
		sip.print(iAddr);

		CallStackS rStacks;
		se.resloveStacks(rStacks);
		
		sip.print(rStacks);
        //     se.DumpStack(GetCurrentThread(),NULL);

        //RaiseException(1,0,2,0);    
        *g_intP = 0x5678;
        //return 5/g_int ;

        return 0;
    }

    void Func4()
    {
        Func5();
    }


    void Func3()
    {
        g_intP = (int*)0x1234;
        Func4();
    }

    void Func2()
    {
        Func3();
    }

    int testException1()
    {
        outputTxtV(TXT(FmtBegin),TXT(__FUNCTION__));outputTxt(HEREINFO_POS() TXT("\n") );

        __try
        {
            Func2();        
        }
        __except(handleException(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER)
        {            
            outputTxt(HEREINFO_POS()); outputTxtV(TXT(FmtEnd),TXT(__FUNCTION__));	
        }
        return 0;
    }


    int testException2()
    {
        outputTxtV(TXT(FmtBegin),TXT(__FUNCTION__));outputTxt(HEREINFO_POS() TXT("\n") );
        StackInfoDisplayer sid;
        __try
        {
            Func2();        
        }
        __except(sid.printExceptStack(*(GetExceptionInformation())->ExceptionRecord), EXCEPTION_EXECUTE_HANDLER)
        {
            outputTxt(HEREINFO_POS()); outputTxtV(TXT(FmtEnd),TXT(__FUNCTION__));
        }
        return 0;
    }

    void testErrorCode()
    {
        PrintTestcase();
        srand( (unsigned)time( NULL ) );

        DWORD dw=rand();
        stlString errStr;

        StackInfoDisplayer sid;
        sid.printErrorCode(dw,&errStr);
        outputTxtV(TXT("Error code[%d] : %s\n"),dw,errStr.c_str());

        sid.printErrorCode(rand());
    }
    
    void testCallerAddr()
    {
        PrintTestcase();
        StackExplorer   se;
        PVOID pAddr=se.getCallerAddr();
        outputTxtStreamW(L"return address is 0x"<< std::hex << pAddr << std::endl);
        AddressInfo ai;
        se.resloveAddrInfo(pAddr,ai);
        outputTxtStreamW(L"current function[" TXTW(__FUNCTION__) << "] caller is "<<ai.m_func.m_name<<std::endl);
    }

    void testEnumSyms()
    {
        PrintTestcase();
        StackExplorer   se;
        StackInfoDisplayer sid;
        stlVector<MOD_SYMBOL_INFO> syms;
        se.enumModuleSyms("c:\\windows\\system32\\kernel32.dll",syms);
        outputTxtV(TXT("enum module[c:\\windows\\system32\\kernel32.dll] symbls:\n"));
        sid.print(syms);
    }


#ifdef RUN_EXAMPLE_STACKEXPLORER
    InitRunFunc(testException1);
    InitRunFunc(testException2);
    InitRunFunc(testErrorCode);
    InitRunFunc(testCallerAddr);
    InitRunFunc(testEnumSyms);
    //InitRunFunc(TestCase_StackExplorer);
#else //else of RUN_EXAMPLE_STACKEXPLORER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_STACKEXPLORER)=(outputTxtV(TXT("[Unit test run disabled] StackExplorer.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"StackExplorer.h\".")
    #endif
#endif  	// RUN_EXAMPLE_STACKEXPLORER
}

#else //else of COMPILE_EXAMPLE_STACKEXPLORER
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_STACKEXPLORER)=(outputTxtV(TXT("[Unit test compile disabled] StackExplorer.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"StackExplorer.h\".")
    #endif
#endif // COMPILE_EXAMPLE_STACKEXPLORER

