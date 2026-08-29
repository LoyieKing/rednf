// CDnFLeaguePromoteSecondEvent —— DnF 联赛晋级第二事件（CEventBase 叶事件）。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写）：C1 0x0811c1e2 / D1 0x0811c1fe /
// D0 0x0811c22e / StartEvent() 0x0811c24c / StartEvent(Word_Param) 0x0811c29e（thunk）/
// EndEvent 0x0811c2b8（均弱符号 W；vtable 0x08b52100）。
// StartEvent(Word_Param) 经 vtable slot 2 虚调 StartEvent()（0x0811c2b4 ff d2）。
// 布局：仅 CEventBase 基区（sizeof 0x0c），无自有成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_CDNFLEAGUEPROMOTESECONDEVENT_H_
#define DNF_GAME_CDNFLEAGUEPROMOTESECONDEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CDnFLeaguePromoteSecondEvent : public CEventBase
{
public:
    CDnFLeaguePromoteSecondEvent();
    virtual ~CDnFLeaguePromoteSecondEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CDNFLEAGUEPROMOTESECONDEVENT_H_
