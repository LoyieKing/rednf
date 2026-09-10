// df_game_r PvP 直播事件 CPvPLiveEvent 实现。
// 报告：docs/class_func_reports/CPvPLiveEvent/ 全部 6 个方法（已完整阅读
// CPvPLiveEvent / ~CPvPLiveEvent / ~CPvPLiveEvent_08268f04 / StartEvent /
// StartEvent_08268f74 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN13CPvPLiveEventC2Ev`（ORIG 0x8269021）；ctor 已是首强符号。
#include <iostream>

#include "CPvPLiveEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08268eb8：仅基类构造 + vptr 置位（无自有成员初始化）。
CPvPLiveEvent::CPvPLiveEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08268ed4 / 0x08268f04：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CPvPLiveEvent::~CPvPLiveEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08268f22：SetEventFlag(true) +
// cMyTrace(__FUNCTION__, 0x13, 0) + "CPvPLiveEvent Start!"。
// ORIG 汇编取 &_ZZN13CPvPLiveEvent10StartEventEvE12__FUNCTION__ 局部静态
// （11 字节 "StartEvent"，nm 0x08bf465b）——必须传 __FUNCTION__ 而非手写
// 签名串（P118/P139 反例：手写串只生成匿名 .rodata，符号差集残留）。
void CPvPLiveEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("CPvPLiveEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08268f74：与 StartEvent() 同体（SetEventFlag(true) + trace，
// __FUNCTION__ 局部静态 _ZZN13CPvPLiveEvent10StartEventE10Word_ParamE12__FUNCTION__
// @0x08bf4650，行 0x19）。param 未被使用（ORIG 亦未引用形参）。
void CPvPLiveEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x19, 0);
    tr("CPvPLiveEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08268fc6：仅 SetEventFlag(false)，无 trace（sub $0x18 帧实证）。
void CPvPLiveEvent::EndEvent()
{
    SetEventFlag(false);
}
