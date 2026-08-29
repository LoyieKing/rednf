// CDnFLeaguePromoteFirstEvent —— DnF 联赛晋级第一事件（CEventBase 叶事件）。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写）：C1 0x0811c0ba / D1 0x0811c0d6 /
// D0 0x0811c106 / StartEvent() 0x0811c124 / StartEvent(Word_Param) 0x0811c176（thunk）/
// EndEvent 0x0811c190（均弱符号 W；vtable 0x08b52140）。
// StartEvent(Word_Param) 经 vtable slot 2 虚调 StartEvent()（0x0811c18c ff d2）。
// 布局：仅 CEventBase 基区（sizeof 0x0c），无自有成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_CDNFLEAGUEPROMOTEFIRSTEVENT_H_
#define DNF_GAME_CDNFLEAGUEPROMOTEFIRSTEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CDnFLeaguePromoteFirstEvent : public CEventBase
{
public:
    CDnFLeaguePromoteFirstEvent();
    virtual ~CDnFLeaguePromoteFirstEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CDNFLEAGUEPROMOTEFIRSTEVENT_H_
