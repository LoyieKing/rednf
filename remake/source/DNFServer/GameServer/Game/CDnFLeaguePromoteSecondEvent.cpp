// df_game_r DnF 联赛晋级第二事件 CDnFLeaguePromoteSecondEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写，地址见头文件注释）。
#include "CDnFLeaguePromoteSecondEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811c1e2（弱 W）：仅基类构造 + vptr 置位。
CDnFLeaguePromoteSecondEvent::CDnFLeaguePromoteSecondEvent()
{
}

// ORIG 0x0811c1fe / 0x0811c22e（弱 W）：标准虚析构（空体编译器生成同形）。
CDnFLeaguePromoteSecondEvent::~CDnFLeaguePromoteSecondEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c24c：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x2b,0)
// ("CDnFLeaguePromoteSecondEvent Event Start!")。
void CDnFLeaguePromoteSecondEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x2b, 0);
    tr("CDnFLeaguePromoteSecondEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c29e：thunk——经 vtable slot 2（StartEvent()）虚调用。
void CDnFLeaguePromoteSecondEvent::StartEvent(Word_Param param)
{
    // [N7 封顶][b28c-33 复核 12/12] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c2b8：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x35,0)
// ("CDnFLeaguePromoteSecondEvent Event End!")。
void CDnFLeaguePromoteSecondEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x35, 0);
    tr("CDnFLeaguePromoteSecondEvent Event End!");
}
