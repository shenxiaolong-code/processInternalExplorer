#ifndef hook_event_win_def_h__
#define hook_event_win_def_h__
#include <MiniMPL/kitType.hpp>

#define HookTypeList(_)                                             \
_(WH_CALLWNDPROC    ,       WPARAM      ,   CWPSTRUCT*          )   \
_(WH_KEYBOARD       ,       WPARAM      ,   LPARAM              )   \
_(WH_MOUSE          ,       WPARAM      ,   MOUSEHOOKSTRUCT*    )   \
_(WH_CBT            ,       WPARAM      ,   LPARAM              )

template<int HK_Type>                           struct GetLParamType;
#define  GetLParamTypeDef(x,w,l)  template<>    struct GetLParamType< x > : public MiniMPL::Type2Type< l > {};
HookTypeList(GetLParamTypeDef);

#endif // hook_event_win_def_h__
