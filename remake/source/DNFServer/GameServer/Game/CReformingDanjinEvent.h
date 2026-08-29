// CReformingDanjinEvent —— 改造丹金事件（CEventManager event 叶类）。
// ORIG vtable（报告 C1 0x0826e92c 实证）：0/1 虚析构，2/3 = StartEvent()/
// EndEvent() 纯虚覆盖，4 = StartEvent(Word_Param)（完整实现，同 StartEvent()
// 体仅 file/line 常量不同），其余槽继承 CEventBase。无扩展成员。
#ifndef DNF_GAME_CREFORMINGDANJINEVENT_H_
#define DNF_GAME_CREFORMINGDANJINEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CReformingDanjinEvent : public CEventBase
{
public:
    CReformingDanjinEvent();
    virtual ~CReformingDanjinEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CREFORMINGDANJINEVENT_H_
