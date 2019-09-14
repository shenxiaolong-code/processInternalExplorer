#ifndef WINDOWINFODEF_H__
#define WINDOWINFODEF_H__
#include <MiniMPL\productSpecialDef.h>
#include <Windows.h>

typedef struct _SubWndInfo 
{
    HWND        m_hSubWnd;
    stlString   m_szClassName;
    stlString   m_szWindowTitle;
    stlString   m_szText;
    //stlString   m_szClassName;
}SubWndInfo,*LPSubWndInfo;

typedef struct _WndOverview 
{
    HWND            m_hWnd;
    WINDOWINFO      m_winInfo;
}WndOverview,*LPWndOverview;



#endif // WINDOWINFODEF_H__
