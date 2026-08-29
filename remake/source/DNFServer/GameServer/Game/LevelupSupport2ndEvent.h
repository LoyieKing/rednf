// LevelupSupport2ndEvent —— 升级支持二期事件（CEventManager 叶事件）。
// ORIG vtable 0x08b522c0（实测 0x40）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 虚覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。弱符号（W）定义
// 于独立 TU（本 cpp）；C1 0x0811b988 / D1 0x0811b9a4 / D0 0x0811b9d4 /
// StartEvent() 0x0811b9f2 / StartEvent(Word_Param) 0x0811ba44 / EndEvent 0x0811ba5e。
// 布局：CEventBase 基区（sizeof 0x0c），无扩展成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_LEVELUPSUPPORT2NDEVENT_H_
#define DNF_GAME_LEVELUPSUPPORT2NDEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class LevelupSupport2ndEvent : public CEventBase
{
public:
    LevelupSupport2ndEvent();
    virtual ~LevelupSupport2ndEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_LEVELUPSUPPORT2NDEVENT_H_
