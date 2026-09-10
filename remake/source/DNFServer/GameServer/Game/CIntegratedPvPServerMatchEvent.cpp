// df_game_r 整合 PvP 服务器匹配事件 CIntegratedPvPServerMatchEvent 实现。
// 报告：docs/class_func_reports/CIntegratedPvPServerMatchEvent/ 全部 6 个方法
// （CIntegratedPvPServerMatchEvent / ~CIntegratedPvPServerMatchEvent /
//  ~CIntegratedPvPServerMatchEvent_0811d484 / StartEvent /
//  StartEvent_0811d4f4 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN30CIntegratedPvPServerMatchEventC2Ev`（ORIG 0x811d5a1）；ctor 已是首强符号。
#include <iostream>

#include "CIntegratedPvPServerMatchEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811d438：仅基类构造 + vptr 置位（无自有成员）。
CIntegratedPvPServerMatchEvent::CIntegratedPvPServerMatchEvent()
{
}

// ORIG 0x0811d454 / 0x0811d484：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CIntegratedPvPServerMatchEvent::~CIntegratedPvPServerMatchEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811d4a2：SetEventFlag(true) +
// cMyTrace(__FUNCTION__,0x12,0)("CIntegratedPvPServerMatchEvent Start!")。
void CIntegratedPvPServerMatchEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("CIntegratedPvPServerMatchEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811d4f4：与 StartEvent() 同体（file 串 "StartEvent"，行 0x18；
// 忽略 param）。
void CIntegratedPvPServerMatchEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("CIntegratedPvPServerMatchEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811d546：仅 SetEventFlag(false)（无 trace）。
void CIntegratedPvPServerMatchEvent::EndEvent()
{
    SetEventFlag(false);
}
