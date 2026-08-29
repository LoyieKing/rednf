// CUnlimitFatigueEvent —— 无限疲劳事件（CEventManager 叶事件）。
// ORIG vtable 0x08c0eb60（实测 0x40）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 虚覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。强符号（T）定义
// 于独立 TU（本 cpp）；C1 0x08285c44 / D1 0x08285c60 / D0 0x08285c90 /
// StartEvent() 0x08285cae / StartEvent(Word_Param) 0x08285d00 / EndEvent 0x08285d52。
// 布局：CEventBase 基区（sizeof 0x0c），无扩展成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_CUNLIMITFATIGUEEVENT_H_
#define DNF_GAME_CUNLIMITFATIGUEEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CUnlimitFatigueEvent : public CEventBase
{
public:
    CUnlimitFatigueEvent();
    virtual ~CUnlimitFatigueEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CUNLIMITFATIGUEEVENT_H_
