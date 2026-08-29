// CVendingMachineBonusEvent —— 自动售货机 Bonus 事件（CEventManager 叶事件）。
// ORIG vtable 0x08b4f420（实测 0x40）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 虚覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。弱符号（W）定义
// 于独立 TU（本 cpp）；C1 0x08116dbe / D1 0x08116dda / D0 0x08116e0a /
// StartEvent() 0x08116e28 / StartEvent(Word_Param) 0x08116e7a / EndEvent 0x08116e94。
// 布局：CEventBase 基区（sizeof 0x0c），无扩展成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_CVENDINGMACHINEBONUSEVENT_H_
#define DNF_GAME_CVENDINGMACHINEBONUSEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CVendingMachineBonusEvent : public CEventBase
{
public:
    CVendingMachineBonusEvent();
    virtual ~CVendingMachineBonusEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CVENDINGMACHINEBONUSEVENT_H_
