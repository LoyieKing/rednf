// df_game_r 刺死事件 CStabToDeathEvent 实现。
// 报告：docs/class_func_reports/CStabToDeathEvent/ 全部 6 个方法（已完整阅读
// CStabToDeathEvent / ~CStabToDeathEvent / ~CStabToDeathEvent_0827f554 /
// StartEvent / StartEvent_0827f5c4 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN17CStabToDeathEventC2Ev`（ORIG 0x827f6a7）；ctor 已是首强符号。
#include <iostream>

#include "CStabToDeathEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0827f508：仅基类构造 + vptr 置位（无自有成员初始化）。
CStabToDeathEvent::CStabToDeathEvent()
{
}

// ORIG 0x0827f524 / 0x0827f554：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CStabToDeathEvent::~CStabToDeathEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0827f572：SetEventFlag(true) +
// cMyTrace(&_ZZN17CStabToDeathEvent10StartEventEvE12__FUNCTION__,0x11,0)
// ("CStabToDeathEvent Start!")。
void CStabToDeathEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x11, 0);
    tr("CStabToDeathEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0827f5c4：与 StartEvent() 同体（SetEventFlag(true) + trace，
// file 串为 __FUNCTION__ 局部静态（E12__FUNCTION__），行 0x17）。
void CStabToDeathEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x17, 0);
    tr("CStabToDeathEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0827f616：SetEventFlag(false) +
// cMyTrace(&_ZZN17CStabToDeathEvent8EndEventEvE12__FUNCTION__,0x1d,0)
// ("CStabToDeathEvent End!")。
void CStabToDeathEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1d, 0);
    tr("CStabToDeathEvent End!");
}
