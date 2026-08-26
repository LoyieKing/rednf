// ============================================================================
// CBloodDungeonRewardEvents.cpp —— 血之副本结算金币 repeat 事件实现
// （2026-08-24，EventRewardGoldC2 批次；独占 TU）
//
// 覆盖 ORIG 全部真实符号（nm 实证均为 W = 头内联强度，本 TU 以类外定义提供同名符号）：
//   CBloodDungeonRewardFirstEvent（event id 0x2d）：
//     C1/C2 0x0811bdde / D1/D2 0x0811be04 / D0 0x0811be34 / StartEvent() 0x0811bd12 /
//     StartEvent(Word_Param) 0x0811bd64 / EndEvent() 0x0811bd8c / GetGold W 0x085bff1a
//   CBloodDungeonRewardSecondEvent（event id 0x2e）：
//     C1/C2 0x0811bf1e / D1/D2 0x0811bf44 / D0 0x0811bf74 / StartEvent() 0x0811be52 /
//     StartEvent(Word_Param) 0x0811bea4 / EndEvent() 0x0811becc / GetGold W 0x085bff26
//
// 日志常量为 ORIG .rodata 实测：__PRETTY_FUNCTION__ 串与 "…Start!/End!" 格式串、
// cMyTrace 行号（Start=0x10；First End=0x2d、Second End=0x1b）。
// 唯一业务调用方 CParty::make_blood_dungeon_info（ORIG T 0x0859e0d2）。
// ============================================================================

#include "CBloodDungeonRewardFirstEvent.h"
#include "CBloodDungeonRewardSecondEvent.h"

// ---------------------------------------------------------------------------
// CBloodDungeonRewardFirstEvent
// ---------------------------------------------------------------------------

CBloodDungeonRewardFirstEvent::CBloodDungeonRewardFirstEvent()
{
    // ORIG C1 0x0811bdde：CEventBase() 后 vptr=0x8b52208（vtable+8），m_gold(+0xc)=0。
    m_gold = 0;
}

CBloodDungeonRewardFirstEvent::~CBloodDungeonRewardFirstEvent()
{
    // ORIG D1 0x0811be04：恢复本类 vptr 后调 CEventBase::~CEventBase()（空）。
}

void CBloodDungeonRewardFirstEvent::StartEvent()
{
    // ORIG 0x0811bd12：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x10, 0)。
    SetEventFlag(true);
    cMyTrace tr("virtual void CBloodDungeonRewardFirstEvent::StartEvent()", 0x10, 0);
    tr("CBloodDungeonRewardFirstEvent Start!");
}

void CBloodDungeonRewardFirstEvent::StartEvent(Word_Param param)
{
    // ORIG 0x0811bd64：movzwl 零扩展 param 低 ushort 写 +0x0c，再经 vptr 槽 2 虚调
    // StartEvent()。成员内未限定调用即为虚分派，与 ORIG call *vptr+8 一致。
    m_gold = (unsigned short)param.m_duration;
    StartEvent();
}

void CBloodDungeonRewardFirstEvent::EndEvent()
{
    // ORIG 0x0811bd8c：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__, 0x2d, 0)。
    SetEventFlag(false);
    cMyTrace tr("virtual void CBloodDungeonRewardFirstEvent::EndEvent()", 0x2d, 0);
    tr("CBloodDungeonRewardFirstEvent End!");
}

int CBloodDungeonRewardFirstEvent::GetGold()
{
    // ORIG W 0x085bff1a：return *(int*)(this+0xc)。
    return m_gold;
}

// ---------------------------------------------------------------------------
// CBloodDungeonRewardSecondEvent
// ---------------------------------------------------------------------------

CBloodDungeonRewardSecondEvent::CBloodDungeonRewardSecondEvent()
{
    // ORIG C1 0x0811bf1e：CEventBase() 后 vptr=0x8b521c8（vtable+8），m_gold(+0xc)=0。
    m_gold = 0;
}

CBloodDungeonRewardSecondEvent::~CBloodDungeonRewardSecondEvent()
{
    // ORIG D1 0x0811bf44：恢复本类 vptr 后调 CEventBase::~CEventBase()（空）。
}

void CBloodDungeonRewardSecondEvent::StartEvent()
{
    // ORIG 0x0811be52：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x10, 0)。
    SetEventFlag(true);
    cMyTrace tr("virtual void CBloodDungeonRewardSecondEvent::StartEvent()", 0x10, 0);
    tr("CBloodDungeonRewardSecondEvent Start!");
}

void CBloodDungeonRewardSecondEvent::StartEvent(Word_Param param)
{
    // ORIG 0x0811bea4：movzwl 零扩展 param 低 ushort 写 +0x0c，再经 vptr 槽 2 虚调
    // StartEvent()。
    m_gold = (unsigned short)param.m_duration;
    StartEvent();
}

void CBloodDungeonRewardSecondEvent::EndEvent()
{
    // ORIG 0x0811becc：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__, 0x1b, 0)。
    SetEventFlag(false);
    cMyTrace tr("virtual void CBloodDungeonRewardSecondEvent::EndEvent()", 0x1b, 0);
    tr("CBloodDungeonRewardSecondEvent End!");
}

int CBloodDungeonRewardSecondEvent::GetGold()
{
    // ORIG W 0x085bff26：return *(int*)(this+0xc)。
    return m_gold;
}
