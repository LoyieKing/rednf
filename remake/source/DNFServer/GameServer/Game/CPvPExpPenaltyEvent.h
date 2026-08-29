// CPvPExpPenaltyEvent —— PvP 经验惩罚事件（CEventManager event 叶类）。
// ORIG vtable（报告 C1 0x08268d1c 实证）：0/1 虚析构，2/3 = StartEvent()/
// EndEvent() 纯虚覆盖，4 = StartEvent(Word_Param)（完整实现，同 StartEvent()
// 体仅 file/line 常量不同），其余槽继承 CEventBase。无扩展成员。
// EndEvent 仅 SetEventFlag(false)，无 trace（报告 0x08268e2a 实证）。
#ifndef DNF_GAME_CPVPEXPPENALTYEVENT_H_
#define DNF_GAME_CPVPEXPPENALTYEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CPvPExpPenaltyEvent : public CEventBase
{
public:
    CPvPExpPenaltyEvent();
    virtual ~CPvPExpPenaltyEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CPVPEXPPENALTYEVENT_H_
