#include "../testconfig_SystemExplorer.h"
#include <MiniMPL/macro_init.h>
/***********************************************************************************************************************
* Description         : test file for <SystemExplorer/hook_event_win.h>
* Author              : Shen.Xiaolong (at Wed 03/10/2021) , xlshen@126.com  xlshen2002@hotmail.com
* Copyright           : free to use / modify / sale in free and commercial software with those head comments.
* usage demo          : #define RUN_EXAMPLE_HOOK_EVENT_WIN to run this demo
***********************************************************************************************************************/
//#define RUN_EXAMPLE_HOOK_EVENT_WIN

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_HOOK_EVENT_WIN
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_HOOK_EVENT_WIN
#endif

#if defined(RUN_EXAMPLE_) && !defined(COMPILE_EXAMPLE_)
    #define COMPILE_EXAMPLE_HOOK_EVENT_WIN
#endif

//#undef RUN_EXAMPLE_HOOK_EVENT_WIN
//#undef COMPILE_EXAMPLE_HOOK_EVENT_WIN

////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_HOOK_EVENT_WIN
#include <SystemExplorer/hook_event_win.h>
#include <UnitTestKit/tc_tracer.h>

namespace UnitTest
{
    template<typename T>
    struct Test_GUI
    {  
        HWND m_hWnd = nullptr;

        void test()
        {
            outputTxtStreamA("\r\n\r\n------------------ IWinEventHook test case  -------------------\r\n");
            IWinEventHook* pHook = ((T*)this);
            bool bRet = pHook->install(CreateTestWnd_Edit());
            ASSERT_AND_LOG(bRet == true);

            waitHookMessage();
        }

        HWND CreateTestWnd_Edit()
        {
            //HWND hEdit = CreateWindowA("button", "button", WS_VISIBLE | WS_POPUP, 400, 400, 100, 100, NULL, 0, NULL, 0);
            m_hWnd = CreateWindowA("edit", "test edit control", WS_VISIBLE | WS_POPUP, 1200, 400, 300, 100, NULL, 0, NULL, 0);
            ASSERT_AND_LOG(m_hWnd != NULL);
            outputTxtStreamA("Create test window , hand=" << m_hWnd << std::endl);
            return m_hWnd;
        }

        virtual void waitHookMessage()
        {
            MSG msg;
            while (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            outputTxtStreamA("\r\nmessage loop exit!\r\n");
        }
    };

    struct Test_CMouseEventHook : public CMouseEventHook , public Test_GUI<Test_CMouseEventHook>
    {
        virtual LRESULT onMouseEvent(MOUSEHOOKSTRUCT* const& lp)
        {
            uninstall();

            outputTxtStreamA("\r\nMouse hook msg is received! destroy window handle="<<lp->hwnd<<std::endl);
            ::PostMessage(lp->hwnd, WM_QUIT, 0, 0);
            return 0;
        }        
    };

    struct Test_CKeyBoardEventHook : public CKeyBoardEventHook, public Test_GUI<Test_CKeyBoardEventHook>
    {
        virtual LRESULT onWinEvent(WPARAM wp, LPARAM lp)
        {
            uninstall();

            outputTxtStreamA("\r\nKeyboard hook msg is received! destroy window handle=" << m_hWnd << std::endl);
            ::PostMessage(m_hWnd, WM_QUIT, 0, 0);
            
            return 0;
        }

        virtual void waitHookMessage()
        {
            //::SetWindowTextA(m_hWnd, "set test message");
            PostMessage(m_hWnd, WM_KEYUP, VK_RETURN, 0);
            __super::waitHookMessage();
        }
    };

    struct Test_CWndProcEventHook : public CWndProcEventHook, public Test_GUI<Test_CWndProcEventHook>
    {
        virtual LRESULT onWndProcEvent(CWPSTRUCT* const& lp)
        {
            uninstall();

            outputTxtStreamA("\r\nWndProc hook msg is received! destroy window handle=" << lp->hwnd << std::endl);
            ::PostMessage(lp->hwnd, WM_QUIT, 0, 0);
            
            return 0;
        }
    };

    struct Test_CWndUIChangeHook : public CWndUIChangeHook, public Test_GUI<Test_CWndUIChangeHook>
    {
        virtual bool isHookedEvent(int nCode)
        {
            return nCode == HCBT_ACTIVATE;
        }

        virtual LRESULT onUIChangeEvent(WPARAM wp, LPARAM lp)
        {
            uninstall();

            outputTxtStreamA("\r\nWnd UI change hook msg is received! destroy window handle=" << m_hWnd << std::endl);
            ::PostMessage(m_hWnd, WM_QUIT, 0, 0);
            
            return 0;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    inline void TestCase_hook_event_win()
    {
        PrintTestcase();
        
        //mouse and keyboard needs user interaction, it will block the test procedure without simulator.
        /*/
        Test_CMouseEventHook        mH;
        mH.test();

        Test_CKeyBoardEventHook     kH;
        kH.test();
        //*/

        Test_CWndProcEventHook      PH;
        PH.test();

        Test_CWndUIChangeHook      UH;
        UH.test();
    }

#ifdef RUN_EXAMPLE_HOOK_EVENT_WIN
    InitRunFunc(TestCase_hook_event_win);
#else //else of RUN_EXAMPLE_HOOK_EVENT_WIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_)=(outputTxtV(TXT("[Unit test run disabled] hook_event_win.h\n%s(%d)\n") TXT(__FILE__) MAKESTR(__LINE__)),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"hook_event_win.h\".")
    #endif
#endif  	// RUN_EXAMPLE_HOOK_EVENT_WIN
}

#else //else of COMPILE_EXAMPLE_HOOK_EVENT_WIN
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_)=(outputTxtV(TXT("[Unit test compile disabled] hook_event_win.h\n%s(%d)\n") TXT(__FILE__) MAKESTR(__LINE__)),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"hook_event_win.h\".")
    #endif
#endif // COMPILE_EXAMPLE_HOOK_EVENT_WIN

