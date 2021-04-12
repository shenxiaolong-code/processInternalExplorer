#ifndef __HOOK_EVENT_WIN_H__
#define __HOOK_EVENT_WIN_H__
/***********************************************************************************************************************
* Description         :
* Author              : Shen.Xiaolong (at Wed 03/10/2021) , xlshen@126.com  xlshen2002@hotmail.com
* Copyright           : free to use / modify / sale in free and commercial software with those head comments.
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <windows.h>
#include <SystemExplorer/hook_event_win_def.h>
#include <SystemExplorer/callback_cpp2c.h>
#include <functional>

// https://docs.microsoft.com/en-us/windows/win32/winmsg/using-hooks#monitoring-system-events
class IWinEventHook
{
public:
    virtual bool install(HWND wnd)
    {   //in general, win event is put into the the UI thread (it creates the window)
        if (wnd == nullptr)
        {
            return false;
        }
        return install(::GetWindowThreadProcessId(wnd, NULL));
    }
    virtual bool install(DWORD  threadID) = 0;   //it threadID is 0,  it means all threads in current process
    virtual void uninstall() = 0;
    virtual bool is_valid_hook() = 0;
};

HMODULE get_self_module_handle();

template <int HK_Type>  
struct TIWinEventHookImpl  : public IWinEventHook
{   
    typedef     typename GetLParamType<HK_Type>::type       LParam_T;

    virtual LRESULT onWinEvent(WPARAM wp, LParam_T lp) = 0;

    virtual bool install(DWORD  threadID)
    {
        if (m_hook)
        {   //install & uninstall should be called in pair
            return false;
        }
        typedef LRESULT(__stdcall *TStdCallback) (int nCode, WPARAM wParam, LPARAM lParam);
        TStdCallback  m_callBack = m_thunk.getStdCallBack(this, &TIWinEventHookImpl::hook_procedure);
        m_hook = ::SetWindowsHookEx(HK_Type, (HOOKPROC)m_callBack, get_self_module_handle(), threadID);
        return nullptr != m_hook;
    }

    virtual void uninstall()
    {
        if (m_hook != nullptr)
        {
            ::UnhookWindowsHookEx(m_hook);
            m_hook = nullptr;
        }
    }
    virtual bool is_valid_hook()
    {
        return m_hook != nullptr;
    }

protected:
    virtual LRESULT hook_procedure(int nCode, WPARAM wp, LPARAM lp)
    {
        if (isHookedEvent(nCode))
        {
            onWinEvent(wp, LParam_T(lp));
        }
        if (m_hook != NULL)
        {
            return ::CallNextHookEx(m_hook, nCode, wp, lp);
        }
        return 0;
    }

    virtual bool isHookedEvent(int nCode)
    { 
        return  nCode == HC_ACTION;
    }

protected:
    HHOOK                   m_hook = NULL;
    CInstructionThunk       m_thunk;
};

//////////////////////////////////////////////////////////////////////////
struct CKeyBoardEventHook   : public  TIWinEventHookImpl<WH_KEYBOARD>
{
    //virtual LRESULT onWinEvent(WPARAM wp, LPARAM lp);
};
struct CMouseEventHook      : public  TIWinEventHookImpl<WH_MOUSE>
{
    virtual LRESULT onWinEvent(WPARAM wp, MOUSEHOOKSTRUCT* lp);
    virtual LRESULT onMouseEvent(MOUSEHOOKSTRUCT* const& lp) = 0;
};
struct CWndProcEventHook    : public  TIWinEventHookImpl<WH_CALLWNDPROC>
{
    virtual LRESULT onWinEvent(WPARAM wp, CWPSTRUCT* lp);
    virtual LRESULT onWndProcEvent(CWPSTRUCT* const& lp) = 0;
};

struct CWndUIChangeHook : public  TIWinEventHookImpl<WH_CBT>
{   // computer-based training (CBT) applications
    virtual LRESULT onWinEvent(WPARAM wp, LPARAM lp);
    virtual LRESULT onUIChangeEvent(WPARAM wp, LPARAM lp) = 0;
};




#endif // __HOOK_EVENT_WIN_H__
