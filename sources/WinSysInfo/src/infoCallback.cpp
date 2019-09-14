#include <WinSysInfo/sysInfoDef.h>
#include <MiniMPL/scanPrint.hpp>

namespace Util
{
    //////////////////////////////////////////////////////////////////////////
    //implement code of declare function above
    BOOL CALLBACK WinPropProc(HWND hwndSubclass,stlChar const* lpszString,HANDLE hData)
    { 
        static int nProp = 1;		// property counter
        static HWND hOldHwnd;
        if (hOldHwnd == hwndSubclass) 
        {
            nProp++;
        }
        else
        {
            hOldHwnd	= hwndSubclass;
            nProp =1;
        }

        //lpszString might be a null-terminated string or a atom value set by API SetProp (see MSDN)
        stlChar buf[128]={0};
        if (GlobalGetAtomName((ATOM)lpszString,buf,arraySize(buf)))
        {            
            printConsole(TXT("\nwindow[0x%X] property list[%d]: 0x%08X [data handle],%s (atom value) "),hwndSubclass,nProp,hData,buf );	
        }
        else
        {
            printConsole(TXT("\nwindow[0x%X] property list[%d]: 0x%08X [data handle],%s"),hwndSubclass,nProp,hData,lpszString );	
        }
        return TRUE; 
    }

    BOOL CALLBACK LookupHwndProc(HWND Hwnd,LPARAM lParam)
    {
        DWORD dwPid = 0;
        GetWindowThreadProcessId(Hwnd,&dwPid);
        lpWndInfo pWndInfo = (lpWndInfo)lParam;
        if (dwPid == pWndInfo->dwPid)
        {
            pWndInfo->hWnd = Hwnd;
            return FALSE;
        }
        return TRUE;
    }

}
