// CFatigueGiveItemEvent —— 疲劳赠送物品事件（CEventBase 叶事件）。
// ORIG vtable 0x08b52000（objdump 实证，报告缺失按 §3.2 兜底）：
// 0/1 虚析构，2/3 = StartEvent()/EndEvent() 覆盖，4 = StartEvent(Word_Param)
// （thunk：经 vptr slot2 虚调 StartEvent()），其余槽继承 CEventBase。
// 布局（ORIG ctor 0x0811c838 汇编实证）：仅 CEventBase 基区（sizeof 0x0c），
// 无扩展成员。
#ifndef DNF_GAME_CFATIGUEGIVEITEMEVENT_H_
#define DNF_GAME_CFATIGUEGIVEITEMEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CFatigueGiveItemEvent : public CEventBase
{
public:
    CFatigueGiveItemEvent();
    virtual ~CFatigueGiveItemEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CFATIGUEGIVEITEMEVENT_H_
