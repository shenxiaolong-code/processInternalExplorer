#include "../testconfig_SystemExplorer.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <SystemExplorer/callback_cpp2c.h>
* Author              : Shen.Xiaolong (at Fri 04/02/2021) , xlshen@126.com  xlshen2002@hotmail.com
* Copyright           : free to use / modify / sale in free and commercial software with those head comments.
* usage demo          : #define RUN_EXAMPLE_CALLBACK_CPP2C to run this demo
***********************************************************************************************************************/
//#define RUN_EXAMPLE_CALLBACK_CPP2C

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_CALLBACK_CPP2C
#endif

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_CALLBACK_CPP2C
#endif

#if defined(RUN_EXAMPLE_) && !defined(COMPILE_EXAMPLE_)
    #define COMPILE_EXAMPLE_CALLBACK_CPP2C
#endif

//#undef RUN_EXAMPLE_CALLBACK_CPP2C
//#undef COMPILE_EXAMPLE_CALLBACK_CPP2C

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_CALLBACK_CPP2C
#include <SystemExplorer/callback_cpp2c.h>
#include <UnitTestKit/tc_tracer.h>

namespace UnitTest
{
    struct TestInstructionThunk
    {        
        TestInstructionThunk(int d)            : m_d(d) {};
        int thisCallFunc(int a, float b)
        {
            ALWAYS_LOG_HERE();
            return int(m_d+a + b);
        }

        void thisCallFunc_void(int a, float b)
        {
            m_d += 100;
            ALWAYS_LOG_HERE();
        }

        int  m_d = 0;
    };


    typedef int(__stdcall *StdCall_T)(int a, float b);
    StdCall_T    pCStdCall = nullptr;
    int __stdcall runThunkFunc(int a, float b)
    {
        ALWAYS_LOG_HERE();
        if (pCStdCall)
        {
            return pCStdCall(a, b);
        }
        return 0;
    }

    typedef void (__stdcall *StdCallVoid_T)(int a, float b);
    StdCallVoid_T    pCStdCall_void = nullptr;
    void __stdcall runThunkFunc_Void(int a, float b)
    {
        ALWAYS_LOG_HERE();
        if (pCStdCall_void)
        {
            pCStdCall_void(a, b);
        }        
    }

    inline void TestCase_callback_cpp2c()
    {
        PrintTestcase();
        TestInstructionThunk    obj(5000);

        CInstructionThunk       helper;
        pCStdCall = helper.getStdCallBack(&obj, &TestInstructionThunk::thisCallFunc);
        auto ret = runThunkFunc(5,4.5f);
        ASSERT_AND_LOG(ret == 5009);

        CInstructionThunk       helperVoid;
        pCStdCall_void = helperVoid.getStdCallBack(&obj, &TestInstructionThunk::thisCallFunc_void);
        runThunkFunc_Void(54, 100.5f);
        ASSERT_AND_LOG(obj.m_d == 5100);
    }

#ifdef RUN_EXAMPLE_CALLBACK_CPP2C
    InitRunFunc(TestCase_callback_cpp2c);
#else //else of RUN_EXAMPLE_CALLBACK_CPP2C

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_)=(outputTxtV(TXT("[Unit test run disabled] callback_cpp2c.h\n%s(%d)\n") TXT(__FILE__) MAKESTR(__LINE__)),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"callback_cpp2c.h\".")
    #endif
#endif  	// RUN_EXAMPLE_CALLBACK_CPP2C
}

#else //else of COMPILE_EXAMPLE_CALLBACK_CPP2C
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_)=(outputTxtV(TXT("[Unit test compile disabled] callback_cpp2c.h\n%s(%d)\n") TXT(__FILE__) MAKESTR(__LINE__)),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"callback_cpp2c.h\".")
    #endif
#endif // COMPILE_EXAMPLE_CALLBACK_CPP2C

