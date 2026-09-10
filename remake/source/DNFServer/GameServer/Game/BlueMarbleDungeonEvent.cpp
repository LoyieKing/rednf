// df_game_r 蓝大理石地下城事件 BlueMarbleDungeonEvent 实现。
// 报告：docs/class_func_reports/BlueMarbleDungeonEvent/ 全部 6 个方法已读。
//   C1 0x080dc470 / D1 0x080dc48c / D0 0x080dc4bc / StartEvent 0x080dc4da /
//   StartEvent(Word_Param) 0x080dc52c（完整实现）/ EndEvent 0x080dc57e。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN22BlueMarbleDungeonEventC2Ev`（ORIG 0x80dc60f）；ctor 已是首强符号。
#include <iostream>

#include "BlueMarbleDungeonEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x080dc470：仅基类构造 + vptr 置位。
BlueMarbleDungeonEvent::BlueMarbleDungeonEvent()
{
}

// ORIG 0x080dc48c / 0x080dc4bc：空体 + 虚析构由编译器生成同形。
BlueMarbleDungeonEvent::~BlueMarbleDungeonEvent()
{
}

// ORIG 0x080dc4da：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x16, 0)
//   ("BlueMarbleDungeonEvent Start!")。__FUNCTION__ 名（0x0b 长度符号）。
void BlueMarbleDungeonEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x16, 0);
    trace("BlueMarbleDungeonEvent Start!");
}

// ORIG 0x080dc52c：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x1c, 0)
//   ("BlueMarbleDungeonEvent Start!")。完整实现，非 thunk。
void BlueMarbleDungeonEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x1c, 0);
    trace("BlueMarbleDungeonEvent Start!");
}

// ORIG 0x080dc57e：SetEventFlag(false) + cMyTrace(__FUNCTION__, 0x22, 0)
//   ("BlueMarbleDungeonEvent End!")。
void BlueMarbleDungeonEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x22, 0);
    trace("BlueMarbleDungeonEvent End!");
}
