#ifndef __HOVERWINDOWMONITOR_H__
#define __HOVERWINDOWMONITOR_H__
/***********************************************************************************************************************
* Description         : monitor window which is hovered by mouse.
* Author              : Shen.Xiaolong (2009-2016)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016-12-08  9:44:36.45)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
*                       latest Version of The Code Project Open License (CPOL : http://www.codeproject.com/)
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <windows.h>
#include <SystemExplorer/HoverWindowReceiver.hpp>

class CThread;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HoverWindowMonitor
{
public:
	HoverWindowMonitor();
	virtual ~HoverWindowMonitor();

    void loop();

    void    setHoverWindowReceiver(TpHoverWindowReceiver pReceivcer);

    virtual bool start();

    virtual bool stop( unsigned int timeout=0xFFFFFFFE );

protected:
    virtual void onNewHoverWindow(HWND hWnd,POINT pt);  //derived or setHoverWindowReceiver

protected:
    volatile bool               m_bStop;
    TpHoverWindowReceiver       m_pReceivcer;
    stlSmartptr<CThread>        m_pThrd;
};

#endif // __HOVERWINDOWMONITOR_H__
