#ifndef __WINDOWEXPLORER_H__
#define __WINDOWEXPLORER_H__
/***********************************************************************************************************************
* Description         : query window UI info , include its child window , text , size.
* Author              : Shen.Xiaolong (2009-2016)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016-12-07)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
*                       latest Version of The Code Project Open License (CPOL : http://www.codeproject.com/)
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <vector>
#include <SystemExplorer/WindowinfoDef.h>

struct WindowFinder
{
    HWND findWindowHwndByName(char const* lpWindowName);
    HWND findMainHwndByPID(DWORD dwPID);									//reverse procedure of ProcessFinder::findProcessByHwnd
    HWND findWindowHwndByPoint( POINT pt ,bool bChkTransparentWnd=false);
    HWND findWindowHwndByMouse(bool bChkTransparentWnd=false);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WindowExplorer
{
public:
	WindowExplorer(HWND hWnd);
	virtual ~WindowExplorer();

    unsigned    getAllChildHwnd(stlVector<HWND>& subChildWnds);
    bool        getOverviewInfo(WndOverview& wi);
	bool        getWindInfo(SubWndInfo& wi);

    bool        closeWindow();

protected:
    bool        isValid();
    stlString   GetCtrlText(HWND hCtrl);

protected:
    HWND    m_hWnd;
};

class HoverWindowExplorer   : public WindowExplorer
{
public:
    HoverWindowExplorer();
    void onNewWindow(HWND hWnd,POINT pt);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WindowInfoDisplayer
{
public:
	WindowInfoDisplayer();
	virtual ~WindowInfoDisplayer();

    void        print(WndOverview const& wi);

    void        highLight(HWND hWnd);
    void        unhighLight(HWND hWnd);
};

#endif // __WINDOWEXPLORER_H__
