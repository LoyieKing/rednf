// CPvPLiveEvent —— PvP 直播事件（CEventManager event 叶类）。
// ORIG vtable 0x08bf4688（报告 C1 0x08268eb8 实证）：0/1 虚析构，2/3 =
// StartEvent()/EndEvent() 纯虚覆盖，4 = StartEvent(Word_Param)（完整实现，
// 同 StartEvent() 体仅 file/line 常量不同），其余槽继承 CEventBase。
// 无扩展成员。EndEvent 仅 SetEventFlag(false)，无 trace（报告 0x08268fc6 实证）。
#ifndef DNF_GAME_CPVPLIVEEVENT_H_
#define DNF_GAME_CPVPLIVEEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CPvPLiveEvent : public CEventBase
{
public:
    CPvPLiveEvent();
    virtual ~CPvPLiveEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CPVPLIVEEVENT_H_
