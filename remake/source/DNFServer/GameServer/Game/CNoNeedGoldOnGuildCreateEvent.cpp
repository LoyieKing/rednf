// df_game_r 创建公会免金币事件 CNoNeedGoldOnGuildCreateEvent 实现。
// 报告：docs/class_func_reports/CNoNeedGoldOnGuildCreateEvent/ 全部 6 个方法。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN29CNoNeedGoldOnGuildCreateEventC2Ev`（ORIG 0x81b9d2b）；ctor 已是首强符号。
#include <iostream>

#include "CNoNeedGoldOnGuildCreateEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081b9b8c：仅基类构造 + vptr 置位。
CNoNeedGoldOnGuildCreateEvent::CNoNeedGoldOnGuildCreateEvent()
{
}

// ORIG 0x081b9ba8 / 0x081b9bd8：空体 + 虚析构由编译器生成同形。
CNoNeedGoldOnGuildCreateEvent::~CNoNeedGoldOnGuildCreateEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081b9bf6：置事件标志并打日志（__FUNCTION__ "StartEvent"，line 0x12）。
void CNoNeedGoldOnGuildCreateEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("No Need Money On Guild Create Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x081b9c48：同 StartEvent()（忽略 param，line 0x18）。
void CNoNeedGoldOnGuildCreateEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("No Need Money On Guild Create Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x081b9c9a：清事件标志并打日志（__FUNCTION__ "EndEvent"，line 0x1e）。
void CNoNeedGoldOnGuildCreateEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("No Need Money On Guild Create Event End!");
}
