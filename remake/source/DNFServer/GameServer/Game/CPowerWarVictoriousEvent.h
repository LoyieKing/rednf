// CPowerWarVictoriousEvent —— 力战胜利事件（CEventManager event idx 0x8b 定时器域）。
// 布局（ORIG ctor 0x0826891c + EndEvent 0x08268b4a 汇编实证）：
//   CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
//   sizeof 0x0c）；Word_Param m_param @ +0x0a（复用基类 tail padding：
//   EndEvent 以 movw×2 清零 +0xa/+0xc）。
#ifndef DNF_GAME_CPOWERWARVICTORIOUSEVENT_H_
#define DNF_GAME_CPOWERWARVICTORIOUSEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CPowerWarVictoriousEvent : public CEventBase
{
public:
    CPowerWarVictoriousEvent();
    virtual ~CPowerWarVictoriousEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    Word_Param m_param;   // +0x0a
};

#endif // DNF_GAME_CPOWERWARVICTORIOUSEVENT_H_
