// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W7_UserLinkTail/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CUser.h"
#include "CParty.h"

// ---------------------------------------------------------------------------
// CUser::SetChildrensDayEventValue(int slot, int targetItem, int period)
// W 0x083281be。slot>9 直接返回；否则 state@(this+0x8e098) |= 1<<slot，
// 写 targetItem 表 @(slot+0x23824)*4+0xc 与 period 表 @(slot+0x23830)*4+4。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/SetChildrensDayEventValue.inc"
{
    if (slot > 9)
        return;
    // [b28d-27] ORIG 0x083281be：`1 << slot`（**int** 移位，非 1u）令 4.4 走
    // eax 累加器形（movzbl→eax; mov %eax,%edx; …; or %edx,%eax; mov %eax,%edx），
    // 与 ORIG 逐条全等（探针 /tmp/d27_p2.cpp f7；1u 形折单段 edx 见 f1）。
    m_childrensDayEventState =
        (unsigned char)(m_childrensDayEventState | 1 << slot);  // P0：无 state 槽
    m_childrensDaySlots[slot] = targetItem;
    m_childrensDayPeriods[slot] = period;
}

// ---------------------------------------------------------------------------
// CUser::IsOKChildrensDayEventCondition() const 语义（ORIG 无 const，按头）
// W 0x08328220：state 字节 == 0xff ⇒ true。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/IsOKChildrensDayEventCondition.inc"
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
#include "W7_UserLinkTail/GetChildrensDayEventTargetItemSlot.inc"
{
    if (idx > 9)
        return -1;
    return m_childrensDaySlots[idx];           // P30 [i+K] 两步寻址源形
}

// ---------------------------------------------------------------------------
// CUser::GetChildrensDayEventTargetItemPeriod(int idx) W 0x08328262
// idx>9 → -1；否则读 periods 表 @0x8e0c4 + idx*4。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetChildrensDayEventTargetItemPeriod.inc"
{
    if (idx > 9)
        return -1;
    return m_childrensDayPeriods[idx];
}

// ---------------------------------------------------------------------------
// CUser::GetPowerWarPartyVictory() T 0x0868856c
// 队伍存在用队伍连胜，否则角色连胜（报告转写）。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetPowerWarPartyVictory.inc"
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
#include "W7_UserLinkTail/GetUserListByParty.inc"
{
    out.push_back(this);
    CParty* party = reinterpret_cast<CParty*>(GetParty());
    if (party != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* u = party->get_user(i);
            if (u != 0 && this != u)   // P222 两形互试：4.1 产 `cmp %eax,0x8(%ebp)` mem-dst
                out.push_back(u);
        }
    }
    return party;
}
