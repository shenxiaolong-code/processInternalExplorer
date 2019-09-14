#include <WinSysInfo/sysSpy.h>
#include <stdio.h>
#include <MiniMPL/macroDef.h>
#include "MiniMPL/scanPrint.hpp"

#pragma comment(lib,"Psapi.Lib")
#define  WM_STOPSPY     (WM_USER+100)

namespace Util
{

    //function declare
    extern BOOL CALLBACK WinPropProc(HWND hwndSubclass,stlChar const* lpszString,HANDLE hData);
    extern BOOL CALLBACK LookupHwndProc(HWND Hwnd,LPARAM lParam);   //lookup window handle from callback function

    DWORD static WINAPI GetClassInfoThrd(LPVOID lparam);    //thread independence  to run GetClassInfobyMouse

    //////////////////////////////////////////////////////////////////////
    CEnumSys::CEnumSys()
    {
    }

    CEnumSys::~CEnumSys()
    {
        stop();
    }

    lpThreadInfoNode CEnumSys::GetThreadInfoList(DWORD dwOwnerPID)
    {
        HANDLE				hThreadSnap		= NULL; 
        lpThreadInfoNode	pThrdListHead	= NULL;
        lpThreadInfoNode	pTailNode		= NULL;	 

        hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); 
        if (hThreadSnap == INVALID_HANDLE_VALUE) 
        {
            return (FALSE);
        }	

        THREADENTRY32 TmpThrdEntry	= {0};
        TmpThrdEntry.dwSize = sizeof(THREADENTRY32);
        if (Thread32First(hThreadSnap, &TmpThrdEntry)) 
        { 
            do 
            { 		
                if (TmpThrdEntry.th32OwnerProcessID == dwOwnerPID) 
                {
                    if (NULL == pThrdListHead) 
                    {
                        pThrdListHead = (lpThreadInfoNode)malloc(sizeof(ThreadInfoNode));
                        pTailNode = pThrdListHead;
                        memset(pTailNode,0,sizeof(ThreadInfoNode));
                        CopyMemory(&(pTailNode->ThrdEntry),&TmpThrdEntry,sizeof(THREADENTRY32));					
                        pTailNode->pNext = NULL;
                    }
                    else
                    {
                        pTailNode->pNext = (lpThreadInfoNode)malloc(sizeof(ThreadInfoNode));
                        pTailNode = pTailNode->pNext;
                        memset(pTailNode,0,sizeof(ThreadInfoNode));
                        pTailNode->pNext = NULL;
                        CopyMemory(&(pTailNode->ThrdEntry),&TmpThrdEntry,sizeof(THREADENTRY32));
                    }
                } 
            }while (Thread32Next(hThreadSnap, &TmpThrdEntry)); 
        } 
        CloseHandle (hThreadSnap); 		
        return pThrdListHead;
    }

    lpProcInfoNode CEnumSys::GetProcInfoList()
    {
        BOOL			bFound			= FALSE; 
        HANDLE			hProcessSnap	= NULL;		//Snapshot¾ä±ú
        HANDLE			hProcess		= NULL;		//½ø³Ì¾ä±ú
        lpProcInfoNode	pProcListHead	= NULL;
        lpProcInfoNode	pProcList		= NULL;
        PROCESS_MEMORY_COUNTERS pmc		= {0};

        DWORD ProcIDs[1024], cbNeeded, iNumProc;	
        if ( !EnumProcesses(ProcIDs, sizeof(ProcIDs), &cbNeeded ) )
        {
            return NULL;
        }

        iNumProc = cbNeeded / sizeof(DWORD);
        for (unsigned int i = 0; i < iNumProc; i++ )
        {		
            pProcListHead = GetProcInfo(ProcIDs[i]);
            pProcListHead->pNext = pProcList;
            pProcList = pProcListHead;
        }
        return pProcListHead;
    }

    MODULEENTRY32* CEnumSys::GetProcModuleInfo(DWORD dwPID, DWORD dwModuleID,MODULEENTRY32 * &pModuleInfo)
    {
        BOOL          bFound      = FALSE; 
        HANDLE        hModuleSnap = NULL; 
        MODULEENTRY32 me32        = {0}; 

        // Take a snapshot of all modules in the specified process. 
        pModuleInfo = NULL;

        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
        if (hModuleSnap == INVALID_HANDLE_VALUE) 
            return (FALSE); 

        // Fill the size of the structure before using it. 	
        me32.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hModuleSnap, &me32)) 
        { 
            do 
            { 
                if (me32.th32ModuleID == dwModuleID) 
                { 
                    pModuleInfo = (MODULEENTRY32*)malloc(sizeof(MODULEENTRY32));
                    CopyMemory (pModuleInfo,&me32, sizeof(MODULEENTRY32)); 
                    bFound = TRUE; 
                    break;
                } 
            }while (!bFound && Module32Next(hModuleSnap, &me32)); 
        } 	
        CloseHandle (hModuleSnap); 
        return pModuleInfo;
    }

    lpProcInfoNode CEnumSys::GetProcInfo(DWORD dwPID)
    {
        BOOL			bFound			= FALSE; 
        HANDLE			hProcessSnap	= NULL;		//Snapshot¾ä±ú
        HANDLE			hProcess		= NULL;		//½ø³Ì¾ä±ú
        HWND			hWnd			= NULL;		//½ø³Ì´°¿Ú¾ä±ú
        lpProcInfoNode	pProcInfo		= NULL;
        PROCESS_MEMORY_COUNTERS pmc		= {0};

        hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE) 
        {
            return NULL; 
        }	

        PROCESSENTRY32 TmpProcEntry	= {0};
        TmpProcEntry.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hProcessSnap, &TmpProcEntry)) 
        { 
            do 
            {
                if (TmpProcEntry.th32ProcessID == dwPID)
                {
                    pProcInfo = (lpProcInfoNode)malloc(sizeof(ProcInfoNode));
                    memset(pProcInfo,0,sizeof(ProcInfoNode));
                    CopyMemory(&(pProcInfo->ProcInfo),&TmpProcEntry,sizeof(PROCESSENTRY32));
                    pProcInfo->pModuleInfoList = GetModuleInfoList(dwPID);
                    pProcInfo->pThrdInfoList = GetThreadInfoList(dwPID);				
                    if ((hWnd = GetProcessMainHwndByPID(dwPID)) != NULL)
                    {
                        pProcInfo->pWildInfo = GetProcessWndInfo(hWnd);
                    }					

                    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE, dwPID);
                    GetProcessMemoryInfo(hProcess,&(pProcInfo->ProcMemInfo),sizeof(PROCESS_MEMORY_COUNTERS));
                    CloseHandle(hProcess);
                    bFound = TRUE; 
                    break;
                }								
            } 
            while (!bFound && Process32Next(hProcessSnap, &TmpProcEntry)); 		
        }	
        CloseHandle (hProcessSnap); 
        return pProcInfo;
    }

    lpModuleInfonode CEnumSys::GetModuleInfoList(DWORD dwPID)
    {
        HANDLE        hModuleSnap = NULL; 
        MODULEENTRY32 me32        = {0}; 
        lpModuleInfonode	pModuleInfoListHead	= NULL;
        lpModuleInfonode	pModuleInfoList	= NULL;

        // Take a snapshot of all modules in the specified process. 	
        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
        if (hModuleSnap == INVALID_HANDLE_VALUE) 
        {
            return NULL;
        }        

        // Fill the size of the structure before using it. 	
        me32.dwSize = sizeof(MODULEENTRY32);     

        if (Module32First(hModuleSnap, &me32)) 
        { 
            do 
            { 
                pModuleInfoListHead = (lpModuleInfonode)malloc(sizeof(ModuleInfonode));
                memset(pModuleInfoListHead,0,sizeof(ModuleInfonode));
                CopyMemory(&(pModuleInfoListHead->ModuleEntry),&me32,sizeof(MODULEENTRY32));
                pModuleInfoListHead->pNext = pModuleInfoList;
                pModuleInfoList = pModuleInfoListHead;
            } 
            while (Module32Next(hModuleSnap, &me32)); 
        }	
        CloseHandle (hModuleSnap); 	
        return pModuleInfoListHead; 

    }

    void CEnumSys::PrintThrdInfo(const THREADENTRY32 &ThrdInfo)
    {
        printConsole( TXT(" %c线程ID:\t\t%d\n"),16,ThrdInfo.th32ThreadID); 
        printConsole( TXT(" ├线程父进程ID(PID)：\t%d\n"), ThrdInfo.th32OwnerProcessID); 
        printConsole( TXT(" ├基本优先级：\t\t%d\n"), ThrdInfo.tpBasePri); 
        printConsole( TXT(" └优先级偏移量：\t%d\n"), ThrdInfo.tpDeltaPri);
    }

    void CEnumSys::PrintProcInfo(const PROCESSENTRY32 &ProcInfo)
    {
        printConsole(TXT(" %c进程ID：\t\t%d\n"),16,ProcInfo.th32ProcessID);
        printConsole(TXT(" ├进程执行程序：\t%s\n"),ProcInfo.szExeFile);
        printConsole(TXT(" ├进程缺省堆内存ID：\t%d\n"),ProcInfo.th32DefaultHeapID);
        printConsole(TXT(" ├进程拥有线程数：\t%d\n"),ProcInfo.cntThreads);
        printConsole(TXT(" └进程缺省优先级\t%d\n"),ProcInfo.pcPriClassBase);
    }

    void CEnumSys::PrintModuleInfo(const MODULEENTRY32 &ModInfo)
    {
        printConsole(TXT(" %c模块序号:\t\t%d\n"),16,ModInfo.th32ModuleID);
        printConsole(TXT(" ├模块进程ID:\t\t%d\n"),ModInfo.th32ProcessID);			
        printConsole(TXT(" ├系统中本模块引用总量:\t%d\n"),ModInfo.GlblcntUsage);
        printConsole(TXT(" ├此程序中本模块引用量:\t%d\n"),ModInfo.ProccntUsage);
        printConsole(TXT(" ├模块内存地址:\t\t0x%X\n"),ModInfo.modBaseAddr);
        printConsole(TXT(" ├模块占用内存大小:\t%d k\n"),ModInfo.modBaseSize/1024);
        printConsole(TXT(" ├模块句柄:\t\t%X\n"),ModInfo.hModule);
        printConsole(TXT(" ├模块名称:\t\t%s\n"),ModInfo.szModule);
        printConsole(TXT(" └模块执行路径:\t\t%s\n"),ModInfo.szExePath);
    }

    void CEnumSys::PrintProcMemInfo(const PROCESS_MEMORY_COUNTERS &pmc)
    {
        printConsole( TXT(" %c页面错误数量:\t\t%u\n"),16, pmc.PageFaultCount );
        printConsole( TXT(" ├内存使用量:\t\t%u k\t(高峰值：%u k)\n"), pmc.WorkingSetSize/1024,pmc.PeakWorkingSetSize/1024);
        printConsole( TXT(" ├页面缓冲池:\t\t%u k\t(高峰值：%u k)\n"), pmc.QuotaPagedPoolUsage/1024, pmc.QuotaPeakPagedPoolUsage/1024);
        printConsole( TXT(" ├非页面缓冲池:\t\t%u k\t(高峰值：%u k)\n"), pmc.QuotaNonPagedPoolUsage/1024, pmc.QuotaPeakNonPagedPoolUsage/1024 );
        printConsole( TXT(" └虚拟内存大小:\t\t%u k\t(高峰值：%u k)\n"), pmc.PagefileUsage/1024, pmc.PeakPagefileUsage/1024 ); 
    }

    void CEnumSys::PrintProcessWndInfo(lpProcWildInfo pInfo)
    {
        if (NULL != pInfo)
        {
            printConsole( TXT("  %c本进程具有窗口:\n"),16);
            printConsole( TXT(" ├窗口句柄:\t\t0x%x \n"),pInfo->Hwnd);
            printConsole( TXT(" ├窗口标题:\t\t%s \n"),pInfo->szWndTitle);
            printConsole( TXT(" └窗口类名:\t\t%s \n"),pInfo->szClassName);
        }
    }

    void CEnumSys::PrintProcInfo(lpProcInfoNode pProcInfoList)
    {
        int iIndex = 1;
        lpProcInfoNode pProc = pProcInfoList;
        lpModuleInfonode pMod = NULL;
        lpThreadInfoNode pThrd = NULL;

        printConsole(TXT("\n进程信息--------------------------------------------：\n"));
        while (NULL != pProc)
        {
            printConsole(TXT("\n第%d个进程：\n"),iIndex++);
            /*/打印信息太多，过滤掉
            if ((pProc->ProcInfo.cntThreads > 4) || (pProc->ProcInfo.th32ProcessID == 0)) 
            {
            printConsole("进程ID：\n",pProc->ProcInfo.th32ProcessID);
            pProc = pProc->pNext;			
            continue;		
            }
            //*/

            PrintProcInfo(pProc->ProcInfo);//*/		//打印基本进程信息
            printConsole(TXT("%c进程内存信息：\n"),16);
            PrintProcMemInfo(pProc->ProcMemInfo);	//打印内存信息	
            if (NULL != pProc->pWildInfo)
            {
                PrintProcessWndInfo(pProc->pWildInfo);	//打印杂类信息				
            }
            else
            {
                printConsole(TXT("%c本进程没有和窗口相关联：\n"),16);
            }

            pThrd = pProc->pThrdInfoList;
            printConsole(TXT("%c进程线程信息：\n"),16);
            while (NULL != pThrd)
            {		
                PrintThrdInfo(pThrd->ThrdEntry);
                pThrd = pThrd->pNext;			
            }

            //*/模块信息太多，过滤掉
            pMod = pProc->pModuleInfoList;	
            printConsole(TXT("%c进程模块信息：\n"),16);
            while (NULL != pMod)
            {			
                PrintModuleInfo(pMod->ModuleEntry);
                pMod = pMod->pNext;
            }
            //*/	

            pMod = NULL;
            pThrd = NULL;
            pProc = pProc->pNext;
        }
    }

    lpProcWildInfo CEnumSys::GetProcessWndInfo(HWND hwnd)
    {
        if (NULL == hwnd)
        {
            return NULL;
        }
        lpProcWildInfo pProcessWildInfo = (lpProcWildInfo)malloc(sizeof(ProcWildInfo));
        memset(pProcessWildInfo,0,sizeof(ProcWildInfo));
        pProcessWildInfo->Hwnd = hwnd;
        GetClassName(hwnd,pProcessWildInfo->szClassName,128);
        GetWindowText(hwnd,pProcessWildInfo->szWndTitle,256);
        return pProcessWildInfo;
    }

    HWND CEnumSys::GetHwndbyPoint(POINT pt)
    {
        HWND hChildWnd		= WindowFromPoint(pt);  //this function can't find nested window
        if (hChildWnd)
        {	
            HWND hParentWnd = NULL;
            do 
            {
                hParentWnd = hChildWnd;
                //hChildWnd = ChildWindowFromPointEx(hParentWnd,pt,CWP_SKIPTRANSPARENT|CWP_SKIPINVISIBLE);
                hChildWnd = RealChildWindowFromPoint(hParentWnd,pt);
            } while (hChildWnd && (hChildWnd!=hParentWnd));
            return hParentWnd;	
        }
        return hChildWnd;
    }

    HWND CEnumSys::GetProcessMainHwndByPID(DWORD dwPid)
    {
        WndInfo wi;
        wi.dwPid = dwPid;
        wi.hWnd = NULL;
        EnumWindows(LookupHwndProc,(LPARAM)&wi);
        return wi.hWnd;
    }

    void CEnumSys::ShowWindowInfoByPoint(POINT pt)
    {
        HWND				hwnd        = NULL;
        static HWND			hOldWnd     = NULL;	
        static int          iCounter    = 0;         
        hwnd	= CEnumSys::GetHwndbyPoint(pt);
        if (NULL == hwnd || hwnd == hOldWnd)
        {
            return;
        }

        iCounter++;
        printConsole(TXT("\n+++++++++++++++    %d    +++++++++++++++++++\n"),iCounter);
        RestoreHighLightedWindows(hOldWnd);	
        ShowWindowInfoByPoint(hwnd,pt);       
        hOldWnd		= hwnd;		
        return ;
    }

    void CEnumSys::ShowWindowInfoByPoint( HWND hWnd,POINT pt )
    {
        PrintWindowInfo(hWnd,&pt);	
        HighLightWindows(hWnd);

        if (!GetParent(hWnd)) //top level window
        {
            HWND hChild = NULL;	
            int iNo		= 1;
            for (hChild=GetWindow(hWnd,GW_CHILD);hChild!=NULL;hChild=GetWindow(hChild,GW_HWNDNEXT))
            {
                if (GetWindowLong(hChild,GWL_STYLE) & WS_VISIBLE)
                {
                    printConsole(TXT("\nNo.%d child window"),iNo++);
                    PrintWindowInfo(hChild);
                }
            }		
        }	
    }

    void CEnumSys::PrintProcInfo()
    {
        lpProcInfoNode pProcInfoList = GetProcInfoList();
        if (NULL != pProcInfoList)
        {
            PrintProcInfo(pProcInfoList);
        }
    }

    void CEnumSys::PrintProcInfo(unsigned int PID)
    {
        lpProcInfoNode pProcInfo = GetProcInfo(PID);
        if (NULL != pProcInfo)
        {
            PrintProcInfo(pProcInfo);
        }
    }

    void CEnumSys::PrintAllProcID()
    {
        DWORD ProcIDs[1024], cbNeeded, iNumProc;	
        if ( !EnumProcesses(ProcIDs, sizeof(ProcIDs), &cbNeeded ) )
        {
            return ;
        }
        iNumProc = cbNeeded/sizeof(DWORD);
        printConsole(TXT("\n系统中所有进程ID：\n"));
        for (DWORD i = 0; i < iNumProc ; i++)
        {
            printConsole(i%4==0 ? TXT("%d\t\n") : TXT("%d\t"),ProcIDs[i]);
        }
        printConsole(TXT("\n"));
    }

    void CEnumSys::GetAllCtrlOfWnd(HWND hWnd)
    {
        if (NULL == hWnd)
        {
            return;
        }

        int i					    = 0;
        HWND hCtrl				    = NULL;
        stlChar szClassName[256]	= {0};
        stlChar szWindowName[256]	= {0};
        stlChar szContext[256]      = {0};

        hCtrl = GetWindow(hWnd,GW_HWNDFIRST);
        if (NULL != hCtrl)
        {
            while (NULL != hCtrl)
            {
                GetClassName(hCtrl,szClassName,sizeof(szClassName));
                GetWindowText(hCtrl,szWindowName,sizeof(szWindowName));
                printConsole(TXT("\n\n第%d个控件:"),i++);
                printConsole(TXT("\n\t类名：%s"),szClassName);
                printConsole(TXT("\n\t句柄：0x%x"),hCtrl);
                if (!lstrcmp(szClassName,TXT("Edit")) || !lstrcmp(szClassName,TXT("Static")))
                {				
                    SendMessage(hCtrl,WM_GETTEXT,sizeof(szContext),(LPARAM)szContext);
                    printConsole(TXT("\n控件中内容：%s"),szContext);
                }
                if (!lstrcmp(szClassName,TXT("ComboBox"))) 
                {

                }
                hCtrl = GetWindow(hCtrl,GW_HWNDNEXT);
            }		
        }
    }

    void CEnumSys::GetAllWndHandle()
    {
        DWORD ProcIDs[1024] ={0}, cbNeeded, iNumProc,iNumHwnd = 0;
        WndInfo Hwnd[1024] = {0};
        HWND tmpHwnd = NULL;
        if ( !EnumProcesses(ProcIDs, sizeof(ProcIDs), &cbNeeded ) )
        {
            return ;
        }
        iNumProc = cbNeeded/sizeof(DWORD);
        for (DWORD i = 0; i < iNumProc ; i++)
        {
            tmpHwnd = GetProcessMainHwndByPID(ProcIDs[i]);
            if (NULL != tmpHwnd)
            {
                Hwnd[iNumHwnd].hWnd		= tmpHwnd;
                Hwnd[iNumHwnd++].dwPid	= ProcIDs[i];
            }
        }

        printConsole(TXT("\n进程总数：%d个"),iNumProc);
        printConsole(TXT("\n具有窗口的进程(共%d个)：\n"),iNumHwnd);
        for (unsigned int i = 0 ; i < iNumHwnd ; i++)
        {		
            printConsole(TXT("句柄:0x%x(ID:%u)\t"),Hwnd[i].hWnd,Hwnd[i].dwPid);
            printConsole((i+1)%2 ==0 ? TXT("\n"): TXT(""));
        }
        printConsole(TXT("\n"));
    }

    bool CEnumSys::GetClassInfoByMouse() 
    {	
        m_hThrd = CreateThread(NULL,0,GetClassInfoThrd,this,CREATE_SUSPENDED,&m_dwID);
        if (INVALID_HANDLE_VALUE == m_hThrd)
        {
            printConsole(TXT("Create thread independency fail!"));
        }
        else
        {
            ResumeThread(m_hThrd);
        }
        return INVALID_HANDLE_VALUE != m_hThrd;
    }

    void CEnumSys::stop()
    {
        if (INVALID_HANDLE_VALUE != m_hThrd)
        {
            PostThreadMessage(m_dwID,WM_STOPSPY,0,0);
            WaitForSingleObject(m_hThrd,INFINITE);
            CloseHandle(m_hThrd);
            m_hThrd = INVALID_HANDLE_VALUE;
        }
    }

    DWORD static WINAPI GetClassInfoThrd(LPVOID lparam)
    {
        POINT pt;
        printConsole(TXT("Create thread 0x%x indenpencily successfully to run GetClassInfoByMouse per 500ms!\n"),(DWORD*)lparam);
        printConsole(TXT("********************************************************************************\n"));

        CEnumSys* pCaller = (CEnumSys*)lparam;

        MSG     msg={0};    
        UINT    wMsgFilterMin=0;
        UINT    wMsgFilterMax=0;
        while (1)
        {
            if (PeekMessage(&msg,NULL,wMsgFilterMin,wMsgFilterMax,PM_REMOVE)
                && (WM_STOPSPY == msg.message))
            {                                
                break;
            }

            GetCursorPos(&pt);
            pCaller->ShowWindowInfoByPoint(pt);
            Sleep(100);
        }
        printConsole(TXT("GetClassInfoByMouse thread end !\n"));
        return 0;
    }

    void CEnumSys::PrintWindowInfo(HWND hwnd,LPPOINT lpMousePos)
    {
        CHECK_NULL_ELSE_RETURN_VAL(hwnd,);

        unsigned long		idProc				= 0;
        LONG				idWnd				= 0;

        stlChar				szClassName[64]		= {0};
        stlChar				szWindowName[128]	= {0};
        stlChar				szInfo[10]			= {0};

        GetClassName(hwnd,szClassName,sizeof(szClassName));
        GetWindowText(hwnd,szWindowName,sizeof(szWindowName));
        GetWindowThreadProcessId(hwnd,&idProc);
        idWnd	= GetWindowLong(hwnd,GWL_ID);	//GetDlgCtrlID(hwnd);

        //output information
        printConsole(TXT("\nhandle:\t0x%8.8X, Process ID:%8.8X, Window ID:%u"),hwnd,idProc,idWnd);
        printConsole(TXT("\nWindow name:\t%s"),szWindowName);
        printConsole(TXT("\nClass name:\t%s"),szClassName);	
        EnumProps(hwnd,WinPropProc);

        int iLen;
        iLen = SendMessage(hwnd,WM_GETTEXTLENGTH,0,0);
        stlChar* pText = new stlChar[iLen+2];
        if (NULL != pText)
        {
            memset(pText,0,iLen+2);
            if (SendMessage(hwnd,WM_GETTEXT,iLen+2,(LPARAM)pText))
            {
                printConsole(TXT("\nContent:\t%s (len:%d)"),pText,iLen);
            }
            delete pText;
        }

        WINDOWINFO			wi;	
        GetWindowInfo(hwnd,&wi);

        printConsole(TXT("\nClass style:\t0x%08X"),GetClassLong(hwnd,GCL_STYLE));
        printConsole(TXT("\nWindow style:\t0x%08X"),wi.dwStyle);
        printConsole(TXT("\nWndproc entry:\t0x%08X"),GetClassLong(hwnd,GCL_WNDPROC));
        printConsole(TXT("\nWindow rect:\t(%d,%d)-(%d,%d)\t%dx%d"),
            wi.rcWindow.left,wi.rcWindow.top,wi.rcWindow.right,
            wi.rcWindow.bottom,wi.rcWindow.right- wi.rcWindow.left,
            wi.rcWindow.bottom-wi.rcWindow.top);
        if (lpMousePos) 
        {
            printConsole(TXT("\tmouse(%d,%d)"),lpMousePos->x,lpMousePos->y);
        }
        printConsole(TXT("\n"));
    }

    void CEnumSys::RestoreHighLightedWindows( HWND hWnd )
    {
        if (GetWindowLong(hWnd,GWL_STYLE) & WS_CHILD)
        {
            InvalidateRect(GetParent(hWnd),NULL,true);
            //UpdateWindow(GetParent(hWnd));
        }
        else
        {
            InvalidateRect(hWnd,NULL,true);
            //UpdateWindow(hWnd);
        }
    }

    void CEnumSys::HighLightWindows( HWND hWnd )
    {
        static HWND hLastWnd = NULL;
        if (hLastWnd == hWnd || hWnd == NULL)
        {
            return;
        }
        if (hLastWnd != NULL)
        {
            RECT    oldRc; 
            GetWindowRect(hLastWnd,&oldRc);
            InvalidateRect(hLastWnd,&oldRc,true);

            FLASHWINFO fi;
            fi.cbSize = sizeof(fi);
            fi.hwnd = hWnd;
            fi.dwFlags = FLASHW_ALL;
            FlashWindowEx(&fi);   // flash window in task bar or control to remind user
        }

        //EnableWindow(hWnd,true);
        AnimateWindow(hWnd,5000,AW_SLIDE);   //flash window with simple animate

        HDC hdc = GetWindowDC(hWnd);					
        SelectObject(hdc,GetStockObject(NULL_BRUSH));				//use NULL Brush to fill internal	
        SelectObject(hdc,CreatePen(PS_INSIDEFRAME,2,RGB(255,0,0)));	//use red pen to draw frame and delete old pen

        WINDOWINFO wi;
        wi.cbSize = sizeof(WINDOWINFO);
        GetWindowInfo(hWnd,&wi);

        TEXTMETRIC			tm;
        GetTextMetrics(hdc,&tm);

        Rectangle(hdc,0,0,wi.rcWindow.right-wi.rcWindow.left,wi.rcWindow.bottom-wi.rcWindow.top);  //draw  Rectangle

        //	SetBkMode(hdc,TRANSPARENT);							    //Set background transparent
        SetROP2(hdc,R2_NOT);										//set color mix mode


        RECT ctrlRect;
        GetWindowRect(hWnd,&ctrlRect);
        ScreenToClient(hWnd,(POINT*)&ctrlRect);
        ScreenToClient(hWnd,(POINT*)&ctrlRect+1);
        stlChar szInfo[12]={0};
        TextOut(hdc,ctrlRect.right-10*tm.tmAveCharWidth,ctrlRect.bottom-tm.tmHeight,szInfo,wsprintf(szInfo,TXT("0x%X"),hWnd));

        ReleaseDC(hWnd,hdc);
        DeleteObject(GetCurrentObject(hdc,OBJ_PEN));
        DeleteObject(GetCurrentObject(hdc,OBJ_BRUSH));

        hLastWnd = hWnd;
    }

}
