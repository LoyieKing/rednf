// df_game_r 台湾 GiveMeBox 事件 EventGiveMeBox 实现。
// 报告：docs/class_func_reports/EventGiveMeBox/ 全部 6 个方法（已完整阅读
// EventGiveMeBox / ~EventGiveMeBox / ~EventGiveMeBox_081649fc /
// StartEvent / StartEvent_08164a6c / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN14EventGiveMeBox12kRewardItem_E`（ORIG 0x8164b4f）；R 型数据符号为首强符号。
#include <iostream>

#include "EventGiveMeBox.h"

#include "GameTypes.h"             // cMyTrace

// ORIG R 0x08b7cd20（.rodata，4 字节 0x2920918c）：只读静态常量。
const int EventGiveMeBox::kRewardItem_ = 0x2920918c;

// ---------------------------------------------------------------------------
// ORIG 0x081649b0：仅基类构造 + vptr 置位（无自有成员初始化）。
EventGiveMeBox::EventGiveMeBox()
{
}

// ORIG 0x081649cc / 0x081649fc：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
EventGiveMeBox::~EventGiveMeBox()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08164a1a：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x12,0)（ORIG 局部静态
// _ZZN14EventGiveMeBox10StartEventEvE19__PRETTY_FUNCTION__ @0x08b7da00）
// ("[Taiwan, EventGiveMeBox] Event start!")。
void EventGiveMeBox::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("[Taiwan, EventGiveMeBox] Event start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08164a6c：与 StartEvent() 同体（同样 SetEventFlag(true) + trace，
// __PRETTY_FUNCTION__ 局部静态 @0x08b7d9c0，行 0x17）。
void EventGiveMeBox::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x17, 0);
    tr("[Taiwan, EventGiveMeBox] Event start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08164abe：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x1d,0)（ORIG 局部静态 @0x08b7d980）
// ("[Taiwan, EventGiveMeBox] Event stop!")。
void EventGiveMeBox::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1d, 0);
    tr("[Taiwan, EventGiveMeBox] Event stop!");
}
