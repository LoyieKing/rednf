// df_game_r 条件事件 CConditionEvent 实现。
// 报告：docs/class_func_reports/CConditionEvent/（6 函数全读）。
#include "CConditionEvent.h"

#include "GameTypes.h"             // cMyTrace
#include "LogManager.h"            // LogManager::logFormat

// ---------------------------------------------------------------------------
// ORIG 0x08184970：仅基类构造 + vptr 置位（无自有成员初始化）。
CConditionEvent::CConditionEvent()
{
}

// ORIG 0x0818498c / 0x081849bc：D1 重置 vptr 后调 CEventBase::~CEventBase（末尾
// __in_chrg 常量折叠不 delete）；D0 调 D1 后 operator delete。空体 + 虚析构由
// 编译器生成同形。
CConditionEvent::~CConditionEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081848bc：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0xd,0)
// ("[ConditionEvent] Start!")。
void CConditionEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0xd, 0);
    tr("[ConditionEvent] Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0818490e：thunk——经 vtable slot 2（StartEvent()）虚调用。
void CConditionEvent::StartEvent(Word_Param param)
{
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x08184928：logFormat(1,"localjapan/Arad_ConditionEvent.cpp",
// __PRETTY_FUNCTION__,0x17,"[CConditionEvent] End Event.") 后 SetEventFlag(false)。
void CConditionEvent::EndEvent()
{
    LogManager::logFormat(1, "localjapan/Arad_ConditionEvent.cpp",
        __PRETTY_FUNCTION__, 0x17, "[CConditionEvent] End Event.");
    SetEventFlag(false);
}
