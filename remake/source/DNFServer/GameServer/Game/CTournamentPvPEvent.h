// CTournamentPvPEvent —— 锦标赛 PvP 频道事件（CEventManager 叶事件）。
// ORIG vtable 0x08c0cb80（实测 0x40）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 虚覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。强符号（T）定义
// 于独立 TU（本 cpp）；C1 0x082845cc / D1 0x082845e8 / D0 0x08284618 /
// StartEvent() 0x08284636（空体）/ StartEvent(Word_Param) 0x0828463c /
// EndEvent 0x0828468e。
// 布局：CEventBase 基区（sizeof 0x0c），无扩展成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_CTOURNAMENTPVPEVENT_H_
#define DNF_GAME_CTOURNAMENTPVPEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CTournamentPvPEvent : public CEventBase
{
public:
    CTournamentPvPEvent();
    virtual ~CTournamentPvPEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CTOURNAMENTPVPEVENT_H_
