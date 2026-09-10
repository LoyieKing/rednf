// df_game_r 改造丹金事件 CReformingDanjinEvent 实现。
// 报告：docs/class_func_reports/CReformingDanjinEvent/ 全部 6 个方法（已完整
// 阅读 CReformingDanjinEvent / ~CReformingDanjinEvent /
//  ~CReformingDanjinEvent_0826e978 / StartEvent / StartEvent_0826e9e8 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CReformingDanjinEventC2Ev`（ORIG 0x826eacb）；ctor 已是首强符号。
#include <iostream>

#include "CReformingDanjinEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0826e92c：仅基类构造 + vptr 置位（无自有成员初始化）。
CReformingDanjinEvent::CReformingDanjinEvent()
{
}

// ORIG 0x0826e948 / 0x0826e978：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CReformingDanjinEvent::~CReformingDanjinEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0826e996：SetEventFlag(true) +
// cMyTrace("virtual void CReformingDanjinEvent::StartEvent()",0x13,0)
// ("Reforming Danjin Event Start!")。
void CReformingDanjinEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("Reforming Danjin Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0826e9e8：与 StartEvent() 同体（SetEventFlag(true) + trace，
// file 串为 "virtual void CReformingDanjinEvent::StartEvent(Word_Param)"，
// 行 0x19）。
void CReformingDanjinEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x19, 0);
    tr("Reforming Danjin Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0826ea3a：SetEventFlag(false) +
// cMyTrace("virtual void CReformingDanjinEvent::EndEvent()",0x1f,0)
// ("Reforming Danjin Event End!")。
void CReformingDanjinEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1f, 0);
    tr("Reforming Danjin Event End!");
}
