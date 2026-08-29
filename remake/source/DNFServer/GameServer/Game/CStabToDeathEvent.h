// CStabToDeathEvent —— 刺死事件（CEventManager event 叶类）。
// ORIG vtable 0x08c09708（报告 C1 0x0827f508 实证）：0/1 虚析构，2/3 =
// StartEvent()/EndEvent() 纯虚覆盖，4 = StartEvent(Word_Param)（完整实现，
// 同 StartEvent() 体仅 file/line 常量不同），其余槽继承 CEventBase。
// 无扩展成员（C1 仅基类构造+vptr）。
#ifndef DNF_GAME_CSTABTODEATHEVENT_H_
#define DNF_GAME_CSTABTODEATHEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CStabToDeathEvent : public CEventBase
{
public:
    CStabToDeathEvent();
    virtual ~CStabToDeathEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CSTABTODEATHEVENT_H_
