// df_game_r 锦标赛 PvP 频道事件 CTournamentPvPEvent 实现。
// 报告：docs/class_func_reports/CTournamentPvPEvent/ 全部 6 个方法（已完整阅读
// CTournamentPvPEvent / ~CTournamentPvPEvent / ~CTournamentPvPEvent_08284618 /
// StartEvent / StartEvent_0828463c / EndEvent）。
#include "CTournamentPvPEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x082845cc：仅基类构造 + vptr 置位（无自有成员初始化）。
CTournamentPvPEvent::CTournamentPvPEvent()
{
}

// ORIG 0x082845e8 / 0x08284618：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CTournamentPvPEvent::~CTournamentPvPEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08284636：空体（仅 push/mov/pop/ret，无 SetEventFlag、无 trace）。
void CTournamentPvPEvent::StartEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0828463c：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x18,0)
// ("Tournament PvP Channel Open Start")。
void CTournamentPvPEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("Tournament PvP Channel Open Start");
}

// ---------------------------------------------------------------------------
// ORIG 0x0828468e：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x1e,0)
// ("Tournament PvP Channel Open End")。
void CTournamentPvPEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Tournament PvP Channel Open End");
}
