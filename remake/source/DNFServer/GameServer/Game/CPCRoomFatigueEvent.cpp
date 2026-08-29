// df_game_r Game/ CPCRoomFatigueEvent 实现（网吧疲劳活动事件）。
// 依据 docs/class_func_reports/CPCRoomFatigueEvent/*.md + ORIG elf 反汇编：
//   C1 0x082675c0：CEventBase() + vptr
//   D1 0x082675dc / D0 0x0826760c：vptr 恢复 + ~CEventBase
//   StartEvent 0x0826762a / StartEvent(Word_Param) 0x0826767c：SetEventFlag + cMyTrace
//   EndEvent 0x082676ce：SetEventFlag(false) + cMyTrace
//   IsEventing 0x08267720：基类判总开关(NULL 参数) + user 非空 + isAffectedPremium(0xc)
#include "CPCRoomFatigueEvent.h"

#include "CUser.h"          // isAffectedPremium

CPCRoomFatigueEvent::CPCRoomFatigueEvent()
{
    // ORIG：CEventBase ctor 置基类 vptr 与 stADDINFO/+0x08 flag；本类 vptr 由编译器设置。
}

CPCRoomFatigueEvent::~CPCRoomFatigueEvent()
{
    // ORIG D1：恢复本类 vptr，再调 CEventBase::~CEventBase()。
}

void CPCRoomFatigueEvent::StartEvent()
{
    // ORIG 0x0826762a：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x13, 0)。
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("PC Room Fatigue Event Start!");
}

void CPCRoomFatigueEvent::StartEvent(Word_Param /*param*/)
{
    // ORIG 0x0826767c：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x1a, 0)。
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x1a, 0);
    tr("PC Room Fatigue Event Start!");
}

void CPCRoomFatigueEvent::EndEvent()
{
    // ORIG 0x082676ce：SetEventFlag(false) + cMyTrace(__FUNCTION__, 0x21, 0)。
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x21, 0);
    tr("PC Room Fatigue Event End!");
}

bool CPCRoomFatigueEvent::IsEventing(CUser* user) const
{
    // ORIG 0x08267720：先以 NULL 调基类 CEventBase::IsEventing 判总开关，
    // 再单独检查 user 非空；两者皆真时返回 isAffectedPremium(user, 0xc)。
    if (!CEventBase::IsEventing(0) || user == 0)
        return false;
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        return false;
    return true;
}
