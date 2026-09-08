// df_game_r DnF 联赛晋级第一事件 CDnFLeaguePromoteFirstEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写，地址见头文件注释）。
#include "CDnFLeaguePromoteFirstEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811c0ba（弱 W）：仅基类构造 + vptr 置位。
CDnFLeaguePromoteFirstEvent::CDnFLeaguePromoteFirstEvent()
{
}

// ORIG 0x0811c0d6 / 0x0811c106（弱 W）：标准虚析构（空体编译器生成同形）。
CDnFLeaguePromoteFirstEvent::~CDnFLeaguePromoteFirstEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c124：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x13,0)
// ("CDnFLeaguePromoteFirstEvent Event Start!")。
void CDnFLeaguePromoteFirstEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x13, 0);
    tr("CDnFLeaguePromoteFirstEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c176：thunk——经 vtable slot 2（StartEvent()）虚调用。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（12=12）。
void CDnFLeaguePromoteFirstEvent::StartEvent(Word_Param param)
{
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c190：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x1d,0)
// ("CDnFLeaguePromoteFirstEvent Event End!")。
void CDnFLeaguePromoteFirstEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1d, 0);
    tr("CDnFLeaguePromoteFirstEvent Event End!");
}
