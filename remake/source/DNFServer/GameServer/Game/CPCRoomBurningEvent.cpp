// df_game_r Game/ CPCRoomBurningEvent 实现。
// 依据 docs/class_func_reports/CPCRoomBurningEvent/*.md + ORIG elf 反汇编：
//   ctor 0x082671dc：CEventBase() + m_factor=100
//   D1 0x08267202：恢复 vptr 后调 CEventBase::~CEventBase()；D0 0x08267232 再 operator delete
//   StartEvent() 0x08267250 / StartEvent(Word_Param) 0x082672ac /
//   EndEvent() 0x0826730a：SetEventFlag + m_factor 写入 + cMyTrace 文本
//   IsEventing 0x08267366：基类判活且 user 非空时返回 isAffectedPremium(user,0xc)
//   GetExpFactor 0x0814aae4 W：return +0x0c
#include "CPCRoomBurningEvent.h"

#include "CUser.h"
#include "cMyTraceNoop.h"

CPCRoomBurningEvent::CPCRoomBurningEvent()
{
    // ORIG：CEventBase ctor 置基类 vptr 与 stADDINFO/+0x08 flag；本类 vptr 由编译器设置。
    m_factor = 100;
}

CPCRoomBurningEvent::~CPCRoomBurningEvent()
{
    // ORIG D1：恢复本类 vptr，再调 CEventBase::~CEventBase()（空）。
}

void CPCRoomBurningEvent::StartEvent()
{
    SetEventFlag(true);
    m_factor = 100;
    cMyTrace tr("StartEvent", 0x14, 0);
    tr("PC Room Burning Event Start!");
}

void CPCRoomBurningEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_factor = (int)(unsigned int)param.m_duration;
    cMyTrace tr("StartEvent", 0x1c, 0);
    tr("PC Room Burning Event Start!");
}

void CPCRoomBurningEvent::EndEvent()
{
    SetEventFlag(false);
    m_factor = 100;
    cMyTrace tr("EndEvent", 0x24, 0);
    tr("PC Room Burning Event End!");
}

bool CPCRoomBurningEvent::IsEventing(CUser* user) const
{
    // ORIG 0x08267366：先以 NULL 调基类 CEventBase::IsEventing 判总开关，
    // 再单独检查 user 非空；两者皆真时返回 isAffectedPremium(user, 0xc)。
    if (!CEventBase::IsEventing(0) || user == 0)
        return false;
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        return false;
    return true;
}

int CPCRoomBurningEvent::GetExpFactor()
{
    // ORIG 0x0814aae4：return *(int*)(this+0x0c)。
    return m_factor;
}
