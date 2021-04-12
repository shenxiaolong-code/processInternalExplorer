#include <SystemExplorer/hook_event_win.h>

HMODULE get_self_module_handle()
{
    MEMORY_BASIC_INFORMATION mbi;
    memset(&mbi, 0, sizeof(mbi));

    DWORD ret = ::VirtualQuery(get_self_module_handle, &mbi, sizeof(mbi));

    return ((ret != 0) ? (HMODULE)mbi.AllocationBase : NULL);
}

//////////////////////////////////////////////////////////////////////////

LRESULT CMouseEventHook::onWinEvent(WPARAM wp, MOUSEHOOKSTRUCT* lp)
{
    return onMouseEvent(lp);
}

LRESULT CWndProcEventHook::onWinEvent(WPARAM wp, CWPSTRUCT* lp)
{
    return onWndProcEvent(lp);
}

LRESULT CWndUIChangeHook::onWinEvent(WPARAM wp, LPARAM lp)
{
    return onUIChangeEvent(wp, lp);
}
