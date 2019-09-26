#include <SystemExplorer/WindowExplorer.h>
#include <MiniMPL/macro_assert.h>
#include <MiniMPL/valueCompare.hpp>
#include <MiniMPL/macro_class.h>

//////////////////////////////////////////////////////////////////////////
namespace 
{
	struct HwndPid
	{
		DWORD   m_dwPid;
		HWND    m_hWnd;

		HwndPid() { ZeroThis(); }
	};

	BOOL CALLBACK EnumWindowsCallback(HWND Hwnd, LPARAM lParam)
	{
		DWORD dwPid = 0;
		if (!GetWindowThreadProcessId(Hwnd, &dwPid))
		{
			return TRUE;	//continue to enumerate
		}

		HwndPid* pHwndPid = (HwndPid*)lParam;
		if (dwPid == pHwndPid->m_dwPid)
		{
			pHwndPid->m_hWnd = Hwnd;
			return FALSE;
		}
		return TRUE;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HWND WindowFinder::findMainHwndByPID( DWORD dwPID )
{
    if (-1==dwPID)
    {
        return NULL;
    }

    HwndPid hp;
    hp.m_dwPid = dwPID;
    hp.m_hWnd = NULL;
    EnumWindows(EnumWindowsCallback,(LPARAM)&hp);
    return hp.m_hWnd;
}

HWND WindowFinder::findWindowHwndByPoint( POINT pt,bool bChkTransparentWnd)
{     
    HWND hParent     = WindowFromPoint(pt);  //this function can't find nested window 
    HWND hChildWnd  = NULL;
    while(1)
    {
        if (bChkTransparentWnd)
        {
            hChildWnd = ChildWindowFromPointEx(hParent,pt,CWP_ALL);
        }
        else
        {
            hChildWnd = RealChildWindowFromPoint(hParent,pt);
        }
        if (!hChildWnd || (hChildWnd==hParent))
        {
            break;
        }
        hParent = hChildWnd;
    }
    return hParent;

    //RealChildWindowFromPoint and ChildWindowFromPointEx all only search direct children
    //don't search deeper child window(s)
    //those differ lies that:
    //if current pt is in one transparent window :
    //ChildWindowFromPointEx will search the transparent window
    //RealChildWindowFromPoint will not, it will search the windows behind transparent window
}

HWND WindowFinder::findWindowHwndByMouse(bool bChkTransparentWnd)
{
    POINT pt;
    while(FALSE==GetCursorPos(&pt)){Sleep(50);};
    return findWindowHwndByPoint(pt,bChkTransparentWnd);
}

HWND WindowFinder::findWindowHwndByName( char const* lpWindowName )
{
    return FindWindowA(NULL,lpWindowName);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowExplorer::WindowExplorer( HWND hWnd )
: m_hWnd(hWnd)
{
}

WindowExplorer::~WindowExplorer()
{

}

unsigned WindowExplorer::getAllChildHwnd( stlVector<HWND>& subChildWnds )
{
    CHECK_TRUE_ELSE_RETURN_VAL(isValid(),0);

    HWND hSubCtrl = GetWindow(m_hWnd,GW_HWNDFIRST);
    while (NULL != hSubCtrl)
    {
		subChildWnds.push_back(hSubCtrl);
        hSubCtrl = GetWindow(hSubCtrl,GW_HWNDNEXT);
    }
    return subChildWnds.size();
}

bool WindowExplorer::getWindInfo(SubWndInfo& oInfo)
{
	oInfo.m_hSubWnd = m_hWnd;

	stlChar szCharBuf[512] = { 0 };
	GetWindowText(m_hWnd, szCharBuf, sizeof(szCharBuf));
	oInfo.m_szWindowTitle = szCharBuf;
	ZeroArray(szCharBuf);

	GetClassName(m_hWnd, szCharBuf, sizeof(szCharBuf));
	oInfo.m_szClassName = szCharBuf;
	ZeroArray(szCharBuf);

	MiniMPL::CEqual eq;
	if (eq(oInfo.m_szClassName, TXT("Edit")) ||
		eq(oInfo.m_szClassName, TXT("Static")))
	{
		SendMessage(m_hWnd, WM_GETTEXT, sizeof(szCharBuf), (LPARAM)szCharBuf);
		oInfo.m_szText = szCharBuf;
		ZeroArray(szCharBuf);
	}
	if (eq(oInfo.m_szClassName, TXT("ComboBox")))
	{
	}
	return true;
}

bool WindowExplorer::getOverviewInfo( WndOverview& wi )
{
    CHECK_TRUE_ELSE_RETURN_VAL(isValid(),false);

    wi.m_hWnd = m_hWnd;
    wi.m_winInfo.cbSize = sizeof(wi.m_winInfo);
    GetWindowInfo(m_hWnd,&wi.m_winInfo);

    return true;
}

bool WindowExplorer::closeWindow()
{
    return WindowExplorer::isValid() 
        && (TRUE==PostMessage(m_hWnd,WM_QUIT,0,0));
}

bool WindowExplorer::isValid()
{
    return INVALID_HANDLE_VALUE != m_hWnd && ::IsWindow(m_hWnd);
}

stlString WindowExplorer::GetCtrlText( HWND hCtrl )
{
    CHECK_TRUE_ELSE_RETURN_VAL(isValid(),L"");

    stlString txt;

    int iLen;
    iLen = SendMessage(m_hWnd,WM_GETTEXTLENGTH,0,0);
    if (iLen>0)
    {
        stlChar* pText = new stlChar[iLen+2];
        memset(pText,0,iLen+2);
        if (SendMessage(m_hWnd,WM_GETTEXT,iLen+2,(LPARAM)pText))
        {
            txt = pText;
        }
        delete [] pText;
    }

    return txt;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern bool operator!=( POINT const& p1,POINT const& p2 );
HoverWindowExplorer::HoverWindowExplorer()
: WindowExplorer(NULL)
{

}

void HoverWindowExplorer::onNewWindow( HWND hWnd,POINT pt)
{
    static POINT lastPt;
    static HWND  lastHwnd=NULL;

    WindowInfoDisplayer wid;
    if (lastPt!=pt)
    {
        outputTxtV(TXT("[%d-%d]\n"),pt.x,pt.y);
        if (NULL !=lastHwnd)
        {
            wid.unhighLight(lastHwnd);
        }
        wid.highLight(hWnd);

        if (lastHwnd != hWnd)
        {
            WndOverview wi;
            m_hWnd = hWnd;
            if (getOverviewInfo(wi))
            {
                outputTxtV(TXT("HWND:0X%8X\n"),hWnd);
            }
            lastHwnd = hWnd;
        }

        lastPt = pt;
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WindowInfoDisplayer::WindowInfoDisplayer()
{
}

WindowInfoDisplayer::~WindowInfoDisplayer()
{
}

void WindowInfoDisplayer::print( WndOverview const& wi )
{

}

void WindowInfoDisplayer::highLight(HWND hWnd)
{
    CHECK_TRUE_ELSE_RETURN_VAL(IsWindow(hWnd),);

    FLASHWINFO fi;
    fi.cbSize = sizeof(fi);
    fi.hwnd = hWnd;
    fi.dwFlags = FLASHW_ALL|FLASHW_TIMER;
    FlashWindowEx(&fi);         // flash window in task bar or control to remind user

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
}

void WindowInfoDisplayer::unhighLight(HWND hWnd)
{
    CHECK_TRUE_ELSE_RETURN_VAL(IsWindow(hWnd),);

    /*
    HDC hdc = GetWindowDC(hWnd);
    SelectObject(hdc,GetStockObject(NULL_BRUSH));				//use NULL Brush to fill internal	

    WINDOWINFO wi;
    wi.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(hWnd,&wi);

    TEXTMETRIC			tm;
    GetTextMetrics(hdc,&tm);
    Rectangle(hdc,0,0,wi.rcWindow.right-wi.rcWindow.left,wi.rcWindow.bottom-wi.rcWindow.top);  //draw  Rectangle
    ReleaseDC(hWnd,hdc);
    DeleteObject(GetCurrentObject(hdc,OBJ_BRUSH));
    */
    /*
    HWND hParent = GetParent(hWnd);
    if (hParent)
    {
        hWnd = hParent;
    }
    */

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
