// df_game_r 防脱离事件 CBreakAwayPreventEvent 实现。
// 报告：docs/class_func_reports/CBreakAwayPreventEvent/ 全部 6 个方法已读。
//   C1 0x08109dbc / D1 0x08109dd8 / D0 0x08109e08 / StartEvent 0x08109e26 /
//   StartEvent(Word_Param) 0x08109e78（完整实现）/ EndEvent 0x08109ed6。
// 依赖：DB_LoadBreakAwayPreventData::makeRequest(bool)（ORIG T 0x08438920），
//   定义缺失 → 上报 Main 分派实现批次（声明在本 TU，U 引用合规）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN22CBreakAwayPreventEventC2Ev`（ORIG 0x8109f73）；ctor 已是首强符号。
#include <iostream>

#include "CBreakAwayPreventEvent.h"

#include "GameTypes.h"   // cMyTrace

class DB_LoadBreakAwayPreventData
{
public:
    static void makeRequest(bool flag);   // ORIG T 0x08438920（定义属 DB 任务批次）
};

// ORIG 0x08109dbc：仅基类构造 + vptr 置位。
CBreakAwayPreventEvent::CBreakAwayPreventEvent()
{
}

// ORIG 0x08109dd8 / 0x08109e08：空体 + 虚析构由编译器生成同形。
CBreakAwayPreventEvent::~CBreakAwayPreventEvent()
{
}

// ORIG 0x08109e26：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x14, 0)
//   ("CBreakAwayPreventEvent Start!")。__FUNCTION__ 名（0x0b 长度符号）。
void CBreakAwayPreventEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x14, 0);
    trace("CBreakAwayPreventEvent Start!");
}

// ORIG 0x08109e78：SetEventFlag(true) + makeRequest(true) +
//   cMyTrace(__FUNCTION__, 0x1b, 0)("CBreakAwayPreventEvent Start!")。
void CBreakAwayPreventEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    DB_LoadBreakAwayPreventData::makeRequest(true);
    cMyTrace trace(__FUNCTION__, 0x1b, 0);
    trace("CBreakAwayPreventEvent Start!");
}

// ORIG 0x08109ed6：SetEventFlag(false) + makeRequest(false) +
//   cMyTrace(__FUNCTION__, 0x23, 0)("CBreakAwayPreventEvent End!")。
void CBreakAwayPreventEvent::EndEvent()
{
    SetEventFlag(false);
    DB_LoadBreakAwayPreventData::makeRequest(false);
    cMyTrace trace(__FUNCTION__, 0x23, 0);
    trace("CBreakAwayPreventEvent End!");
}
