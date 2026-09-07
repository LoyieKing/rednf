// ===========================================================================
// W7_UserLinkTail.cpp —— W7_BuildHarmonize 链接收敛批次：ChildrensDay /
// PowerWarPartyVictory / GetUserListByParty 六个 ORIG 真实符号的落点 TU。
//
// 证据：
//   _ZN5CUser25SetChildrensDayEventValueEiii    W 0x083281be（objdump 逐指令转写）
//   _ZN5CUser30IsOKChildrensDayEventConditionEv W 0x08328220
//   _ZN5CUser34GetChildrensDayEventTargetItemSlotEi   W 0x08328240
//   _ZN5CUser36GetChildrensDayEventTargetItemPeriodEi W 0x08328262
//   _ZN5CUser22GetPowerWarPartyVictoryEv        T 0x0868856c
//       （报告 docs/class_func_reports/CUser/GetPowerWarPartyVictory.md）
//   _ZN5CUser17GetUserListByPartyERSt6vectorIPS_SaIS1_EE T 0x0868056a
//       （报告 docs/class_func_reports/CUser/GetUserListByParty.md）
// 布局注释：childrensDayEventState @0x8e098（byte 位集）；target item slot 表
//   @0x8e09c、period 表 @0x8e0c4（各 int[10]，slot>9 → -1）。
// ===========================================================================

#include "CUser.h"
#include "CParty.h"

// ---------------------------------------------------------------------------
// CUser::SetChildrensDayEventValue(int slot, int targetItem, int period)
// W 0x083281be。slot>9 直接返回；否则 state@(this+0x8e098) |= 1<<slot，
// 写 targetItem 表 @(slot+0x23824)*4+0xc 与 period 表 @(slot+0x23830)*4+4。
// ---------------------------------------------------------------------------
void CUser::SetChildrensDayEventValue(int slot, int targetItem, int period)
{
    if (slot > 9)
        return;
    m_childrensDayEventState |= 1u << slot;    // P0：ORIG 全寄存器无 state 槽
    m_childrensDaySlots[slot] = targetItem;
    m_childrensDayPeriods[slot] = period;
}

// ---------------------------------------------------------------------------
// CUser::IsOKChildrensDayEventCondition() const 语义（ORIG 无 const，按头）
// W 0x08328220：state 字节 == 0xff ⇒ true。
// ---------------------------------------------------------------------------
bool CUser::IsOKChildrensDayEventCondition()
{
    // ORIG 0x08328220：movzbl 0x8e098(%eax)（具名 m_childrensDayEventState）；==0xff → true。
    if (m_childrensDayEventState == 0xff)
        return true;
    return false;
}

// ---------------------------------------------------------------------------
// CUser::GetChildrensDayEventTargetItemSlot(int idx) W 0x08328240
// idx>9 → -1；否则读 slots 表 @0x8e09c + idx*4。
// ---------------------------------------------------------------------------
int CUser::GetChildrensDayEventTargetItemSlot(int idx)
{
    if (idx > 9)
        return -1;
    return m_childrensDaySlots[idx];           // P30 [i+K] 两步寻址源形
}

// ---------------------------------------------------------------------------
// CUser::GetChildrensDayEventTargetItemPeriod(int idx) W 0x08328262
// idx>9 → -1；否则读 periods 表 @0x8e0c4 + idx*4。
// ---------------------------------------------------------------------------
int CUser::GetChildrensDayEventTargetItemPeriod(int idx)
{
    if (idx > 9)
        return -1;
    return m_childrensDayPeriods[idx];
}

// ---------------------------------------------------------------------------
// CUser::GetPowerWarPartyVictory() T 0x0868856c
// 队伍存在用队伍连胜，否则角色连胜（报告转写）。
// ---------------------------------------------------------------------------
int CUser::GetPowerWarPartyVictory()
{
    CParty* party = reinterpret_cast<CParty*>(GetParty());
    if (party == 0)
        return getStraightVictories();
    return party->getStraightVictories();
}

// ---------------------------------------------------------------------------
// CUser::GetUserListByParty(std::vector<CUser*>& out) T 0x0868056a
// push self；有队伍时按槽 0..3 补齐他人（跳过空/自身），返回队伍指针。
// ---------------------------------------------------------------------------
CParty* CUser::GetUserListByParty(std::vector<CUser*>& out)
{
    out.push_back(this);
    CParty* party = reinterpret_cast<CParty*>(GetParty());
    if (party != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* u = party->get_user(i);
            if (u != 0 && u != this)
                out.push_back(u);
        }
    }
    return party;
}
