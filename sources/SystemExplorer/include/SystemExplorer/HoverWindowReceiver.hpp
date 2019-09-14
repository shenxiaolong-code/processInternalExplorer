#ifndef HOVERWINDOWRECEIVER_H__
#define HOVERWINDOWRECEIVER_H__
#include <Windows.h>
#include <MiniMPL\productSpecialDef.h>

struct  HoverWindowReceiver_I
{
    virtual void onNewHoverWindow(HWND hWnd,POINT pt) = 0;
    virtual ~HoverWindowReceiver_I(){};
};

typedef stlSmartptr<HoverWindowReceiver_I>    TpHoverWindowReceiver;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HoverWindowReceiver1P only support one parameter
template<typename TUF>
struct HoverWindowReceiver1P : public HoverWindowReceiver_I
{
    HoverWindowReceiver1P(TUF uf) : m_uf(uf){};

    virtual void onNewHoverWindow(HWND hWnd,POINT pt)
    {
        m_uf(hWnd);
    }

    TUF m_uf;
};

template<typename TUF>
TpHoverWindowReceiver makeHoverWindowReceiver1p(TUF uf)
{                                                      
    return TpHoverWindowReceiver(new HoverWindowReceiver1P<TUF>(uf));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HoverWindowReceiver1P support two parameter : 
template<typename TBF>
struct HoverWindowReceiver2P : public HoverWindowReceiver_I
{
    HoverWindowReceiver2P(TBF bf) : m_bf(bf){};

    virtual void onNewHoverWindow(HWND hWnd,POINT pt)
    {
        m_bf(hWnd,pt);
    }

    TBF     m_bf;
};
template<typename TBF>
TpHoverWindowReceiver makeHoverWindowReceiver2p(TBF uf)
{                                                      
    return TpHoverWindowReceiver(new HoverWindowReceiver2P<TBF>(uf));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename THost,typename TBMF>
struct HoverWindowReceiver2P_Obj : public HoverWindowReceiver_I
{
    HoverWindowReceiver2P_Obj(THost& obj,TBMF mbf) : m_obj(obj),m_mbf(mbf){};

    virtual void onNewHoverWindow(HWND hWnd,POINT pt)
    {
        (m_obj.*m_mbf)(hWnd,pt);
    }

    THost   m_obj;
    TBMF      m_mbf;
};

template<typename THost,typename TBMF>
TpHoverWindowReceiver makeHoverWindowReceiver2p(THost& obj,TBMF bf)
{                                                      
    return TpHoverWindowReceiver(new HoverWindowReceiver2P_Obj<THost,TBMF>(obj,bf));
}



#endif // HOVERWINDOWRECEIVER_H__
