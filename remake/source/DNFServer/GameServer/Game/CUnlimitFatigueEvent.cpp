// df_game_r 无限疲劳事件 CUnlimitFatigueEvent 实现。
// 报告：docs/class_func_reports/CUnlimitFatigueEvent/ 全部 6 个方法（已完整阅读
// CUnlimitFatigueEvent / ~CUnlimitFatigueEvent / ~CUnlimitFatigueEvent_08285c90 /
// StartEvent / StartEvent_08285d00 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN20CUnlimitFatigueEventC2Ev`（ORIG 0x8285de3）；ctor 已是首强符号。
#include <iostream>

#include "CUnlimitFatigueEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08285c44：仅基类构造 + vptr 置位（无自有成员初始化）。
CUnlimitFatigueEvent::CUnlimitFatigueEvent()
{
}

// ORIG 0x08285c60 / 0x08285c90：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CUnlimitFatigueEvent::~CUnlimitFatigueEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08285cae：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x12,0)
// ("Unlimit Fatigue Event Start!")。
void CUnlimitFatigueEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("Unlimit Fatigue Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08285d00：与 StartEvent() 同体（file 串 "StartEvent"，行 0x19）。
void CUnlimitFatigueEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x19, 0);
    tr("Unlimit Fatigue Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08285d52：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x20,0)
// ("Unlimit Fatigue Event End!")。
void CUnlimitFatigueEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x20, 0);
    tr("Unlimit Fatigue Event End!");
}
