// LevelupSupportEvent —— 升级支持事件（CEventManager 叶事件）。
// ORIG vtable 0x08b4f4e0（实测 0x40）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 虚覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。弱符号（W）定义
// 于独立 TU（本 cpp）；C1 0x081169f8 / D1 0x08116a14 / D0 0x08116a44 /
// StartEvent() 0x08116a62 / StartEvent(Word_Param) 0x08116ab4 / EndEvent 0x08116ace。
// 布局：CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
// sizeof 0x0c），无扩展成员（C1 仅基类构造 + vptr，objdump 实证）。
#ifndef DNF_GAME_LEVELUPSUPPORTEVENT_H_
#define DNF_GAME_LEVELUPSUPPORTEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class LevelupSupportEvent : public CEventBase
{
public:
    LevelupSupportEvent();
    virtual ~LevelupSupportEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_LEVELUPSUPPORTEVENT_H_
