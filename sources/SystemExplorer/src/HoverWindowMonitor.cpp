#include <SystemExplorer/HoverWindowMonitor.h>
#include <SystemExplorer/WindowExplorer.h>
#include <thread/thread.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool operator!=( POINT const& p1,POINT const& p2 )
{
    return p1.x!=p2.x || p1.y!=p2.y;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HoverWindowMonitor::HoverWindowMonitor()
: m_bStop(false)
, m_pThrd(new CThread())
{
    m_pThrd->setCallback(&HoverWindowMonitor::loop,*this);
}

HoverWindowMonitor::~HoverWindowMonitor()
{
    if (!m_bStop)
    {
        m_bStop = stop();
    }    
}

void HoverWindowMonitor::onNewHoverWindow( HWND hWnd,POINT pt )
{  
    if (m_pReceivcer)
    {
        static POINT lastPt;
        static HWND  lastHwnd=NULL;

        if (lastPt != pt || hWnd !=lastHwnd)
        {
            m_pReceivcer->onNewHoverWindow(hWnd,pt);

            lastPt   = pt;
            lastHwnd = hWnd;
        }
    }
}

void HoverWindowMonitor::loop()
{
    POINT pt;
    WindowFinder        wf;    
    while (!m_bStop)
    {
        while(!m_bStop && FALSE==::GetCursorPos(&pt)){Sleep(50);};

        onNewHoverWindow(wf.findWindowHwndByPoint(pt),pt);

        Sleep(100);
    }    
}

void HoverWindowMonitor::setHoverWindowReceiver( TpHoverWindowReceiver pReceivcer )
{
    m_pReceivcer = pReceivcer;
}

bool HoverWindowMonitor::start()
{
    m_bStop = !m_pThrd->start();
    return !m_bStop;
}

bool HoverWindowMonitor::stop( unsigned int timeout/*=0xFFFFFFFE */ )
{
    m_bStop = true;
    return m_pThrd->stop(timeout);
}
