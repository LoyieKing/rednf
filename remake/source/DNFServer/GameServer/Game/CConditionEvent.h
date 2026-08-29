// CConditionEvent —— 条件事件（CEventBase 叶事件）。
// ORIG vtable 0x08b8e5c8（0/1 虚析构，2 = StartEvent() 0x081848bc，3 = EndEvent()
// 0x08184928，4 = StartEvent(Word_Param) 0x0818490e —— thunk 虚调 StartEvent()）。
// C1 0x08184970 / D1 0x0818498c / D0 0x081849bc（强符号 T）。
// 布局：仅 CEventBase 基区（sizeof 0x0c），无自有成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_CCONDITIONEVENT_H_
#define DNF_GAME_CCONDITIONEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CConditionEvent : public CEventBase
{
public:
    CConditionEvent();
    virtual ~CConditionEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CCONDITIONEVENT_H_
