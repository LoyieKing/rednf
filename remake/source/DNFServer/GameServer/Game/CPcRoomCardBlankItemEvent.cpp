// df_game_r PC 房卡片空白道具事件 CPcRoomCardBlankItemEvent 实现。
// 报告：docs/class_func_reports/CPcRoomCardBlankItemEvent/ 全部 6 个方法。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN25CPcRoomCardBlankItemEventC2Ev`（ORIG 0x826758f）；ctor 已是首强符号。
#include <iostream>

#include "CPcRoomCardBlankItemEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0826743c：仅基类构造 + vptr 置位。
CPcRoomCardBlankItemEvent::CPcRoomCardBlankItemEvent()
{
}

// ORIG 0x08267458 / 0x08267488：空体 + 虚析构由编译器生成同形。
CPcRoomCardBlankItemEvent::~CPcRoomCardBlankItemEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x082674a6：空函数（push/mov/pop/ret）。
void CPcRoomCardBlankItemEvent::StartEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x082674ac：置事件标志并打日志（忽略 param）。
void CPcRoomCardBlankItemEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("Pc Room Card Blank Item Event Start");
}

// ---------------------------------------------------------------------------
// ORIG 0x082674fe：清事件标志并打日志。
void CPcRoomCardBlankItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Pc Room Card Blank Item Event End");
}
