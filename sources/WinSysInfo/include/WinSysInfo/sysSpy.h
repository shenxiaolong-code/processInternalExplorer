#ifndef __SYSSPY_H__
#define __SYSSPY_H__
/***********************************************************************************************************************
* Description         :
* Author              : Shen.Xiaolong (Shen Tony) (2009-2016)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016/05/30  9:04:13.39)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
*                       latest Version of The Code Project Open License (CPOL : http://www.codeproject.com/)
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <WinSysInfo/sysInfoDef.h>

namespace Util
{
    class CEnumSys
    {
    public:	
        CEnumSys();
        virtual ~CEnumSys();

        //demo functions
        bool GetClassInfoByMouse();
        void stop();

        //get information function
        void GetAllWndHandle();
        void PrintAllProcID();
        void PrintProcInfo();

        void GetAllCtrlOfWnd(HWND hWnd);        
        void ShowWindowInfoByPoint(POINT pt);
        void ShowWindowInfoByPoint(HWND hWnd,POINT pt);

        HWND GetHwndbyPoint(POINT pt);
        HWND GetProcessMainHwndByPID(DWORD dwPid);
        lpProcWildInfo GetProcessWndInfo(HWND hwnd);
        lpProcInfoNode GetProcInfo(DWORD dwPID);
        lpProcInfoNode GetProcInfoList();
        MODULEENTRY32* GetProcModuleInfo(DWORD dwPID,DWORD dwModuleID,MODULEENTRY32 * &pModuleInfo);	
        lpModuleInfonode GetModuleInfoList(DWORD dwPID);
        lpThreadInfoNode GetThreadInfoList(DWORD dwOwnerPID);

        //display information function	
        void PrintProcMemInfo(const PROCESS_MEMORY_COUNTERS &pmc);
        void PrintModuleInfo(const MODULEENTRY32 &ModInfo);	
        void PrintProcessWndInfo(lpProcWildInfo pInfo);
        void PrintThrdInfo(const THREADENTRY32 &ThrdInfo);
        void PrintProcInfo(unsigned int PID);
        void PrintProcInfo(lpProcInfoNode pProcInfoList);
        void PrintProcInfo(const PROCESSENTRY32 &ProcInfo);	
        void PrintWindowInfo(HWND hWnd,LPPOINT lpMousePos=NULL);
        void HighLightWindows(HWND hWnd);
        void RestoreHighLightedWindows(HWND hWnd);

    protected:
        HANDLE  m_hThrd;
        DWORD   m_dwID;
    };
}


#endif // __SYSSPY_H__
