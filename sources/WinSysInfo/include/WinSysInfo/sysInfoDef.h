#ifndef __SYSINFODEF_H__
#define __SYSINFODEF_H__
/***********************************************************************************************************************
* Description         :
* Author              : Shen.Xiaolong (Shen Tony) (2009-2016)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016/05/30  9:04:36.19)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
*                       latest Version of The Code Project Open License (CPOL : http://www.codeproject.com/)
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <MiniMPL/productSpecialDef.h>

namespace Util
{
    typedef struct _tagWndInfo
    {
        DWORD dwPid;
        HWND hWnd;
    } WndInfo,*lpWndInfo;

    typedef struct _tagProcWildInfo
    {
        HWND Hwnd;
        stlChar szWndTitle[256];
        stlChar szClassName[128];
    } ProcWildInfo,*lpProcWildInfo;

    typedef struct _ThreadInfoNode
    {
        THREADENTRY32 ThrdEntry;
        _ThreadInfoNode *pNext;
    } ThreadInfoNode,*lpThreadInfoNode; 

    typedef struct _ModuleInfonode
    {
        MODULEENTRY32 ModuleEntry;
        _ModuleInfonode *pNext;
    } ModuleInfonode,*lpModuleInfonode;

    typedef struct _ProcInfoNode
    {
        PROCESSENTRY32			ProcInfo;
        PROCESS_MEMORY_COUNTERS ProcMemInfo;
        lpModuleInfonode		pModuleInfoList;
        lpThreadInfoNode		pThrdInfoList;
        lpProcWildInfo			pWildInfo;
        _ProcInfoNode			*pNext;
    } ProcInfoNode,*lpProcInfoNode;
}


#endif // __SYSINFODEF_H__
