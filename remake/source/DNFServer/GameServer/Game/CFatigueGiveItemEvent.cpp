// df_game_r 疲劳赠送物品事件 CFatigueGiveItemEvent 实现。
// 报告缺失（docs/class_func_reports/CFatigueGiveItemEvent 不存在），按 §3.2
// 兜底：nm 定址（C1 0x0811c838 / D1 0x0811c854 / D0 0x0811c884 /
// StartEvent 0x0811c8a2 / StartEvent(Word_Param) 0x0811c8f4 /
// EndEvent 0x0811c90e，全部 W 弱符号）+ objdump 区间逐指令转写；
// __PRETTY_FUNCTION__ 字符串与 fmt 字符串取自 .rodata（nm 符号
// CFatigueGiveItemEvent::StartEvent()::__PRETTY_FUNCTION__ @0x08b51f20、
// ::EndEvent()::__PRETTY_FUNCTION__ @0x08b51ee0、fmt @0x08b4fcfc/0x08b4fd20）。
#include "CFatigueGiveItemEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811c838：仅基类构造 + vptr 置位（无自有成员）。
CFatigueGiveItemEvent::CFatigueGiveItemEvent()
{
}

// ORIG 0x0811c854 / 0x0811c884：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CFatigueGiveItemEvent::~CFatigueGiveItemEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c8a2：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__@0x08b51f20,0x12,0)
// ("CFatigueGiveItemEvent Event Start!"@0x08b4fcfc)。
void CFatigueGiveItemEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CFatigueGiveItemEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c8f4：thunk —— 经 vptr slot2 虚调 StartEvent()（忽略 param）。
void CFatigueGiveItemEvent::StartEvent(Word_Param param)
{
    this->StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c90e：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__@0x08b51ee0,0x1c,0)
// ("CFatigueGiveItemEvent Event End!"@0x08b4fd20)。
void CFatigueGiveItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1c, 0);
    tr("CFatigueGiveItemEvent Event End!");
}
