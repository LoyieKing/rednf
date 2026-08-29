// df_game_r PvP 经验惩罚事件 CPvPExpPenaltyEvent 实现。
// 报告：docs/class_func_reports/CPvPExpPenaltyEvent/ 全部 6 个方法（已完整
// 阅读 CPvPExpPenaltyEvent / ~CPvPExpPenaltyEvent /
//  ~CPvPExpPenaltyEvent_08268d68 / StartEvent / StartEvent_08268dd8 / EndEvent）。
#include "CPvPExpPenaltyEvent.h"

#include <iostream>          // std::ios_base::Init 静态初始化（ORIG _GLOBAL__I__ZN19CPvPExpPenaltyEventC2Ev 0x08268e85）
#include "GameTypes.h"       // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08268d1c：仅基类构造 + vptr 置位（无自有成员初始化）。
CPvPExpPenaltyEvent::CPvPExpPenaltyEvent()
{
}

// ORIG 0x08268d38 / 0x08268d68：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CPvPExpPenaltyEvent::~CPvPExpPenaltyEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08268d86：SetEventFlag(true) +
// cMyTrace(__FUNCTION__,0x12,0)("CPvPExpPenaltyEvent Start!")。
// __FUNCTION__ 生成局部静态 _ZZN19CPvPExpPenaltyEvent10StartEventEvE12__FUNCTION__
// （.rodata 0x08bf3f1b 内容 "StartEvent"），勿用手写字面量。
void CPvPExpPenaltyEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("CPvPExpPenaltyEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08268dd8：与 StartEvent() 同体（SetEventFlag(true) + trace，
// __FUNCTION__ 静态 0x08bf3f10 __FUNCTION__，行 0x18）。
void CPvPExpPenaltyEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("CPvPExpPenaltyEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08268e2a：仅 SetEventFlag(false)，无 trace（sub $0x18 帧实证）。
void CPvPExpPenaltyEvent::EndEvent()
{
    SetEventFlag(false);
}
