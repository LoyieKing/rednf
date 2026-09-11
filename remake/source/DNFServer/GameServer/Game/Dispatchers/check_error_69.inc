// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825dd84 orig/ours=124/130 ndiff-blocks=11
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: jne    0825dda7 <+0x23> | setne  %al | mov    0x10(%ebp),%eax
int Dispatcher_UseRightOfChangeGrowType::check_error(CUser* user, MSG_BASE& msg)
{
    // [g05 语义修正 · ORIG 0x0825dd8b 直证] ORIG：`cmpl $0,user; je A` → A `mov $1,%eax`
    //   → `test %al; je 继续` ⇒ **user==0（或 getCurCharacR()==0）→ 返回 0xffffffff**；
    //   原写 `user != 0 && … != 0` 语义取反（Ghidra C 同证：`if (param_1 != 0) { if
    //   (getCurCharacR != 0) bVar1=false } bVar1=true` ⇒ 真值取反）。已按 ORIG 纠正。
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return 0xffffffff;
    }
    // [g05] ORIG 0x0825ddba 直接 `call get_state; cmp $3; setne; test; je`（**无 user==0
    //   前置比较**，该情形已被上一守卫覆盖）⇒ 去掉源码中冗余的 `user == 0 ||`。
    if (user->get_state() != 3)
    {
        return 0x1e;
    }
    // [g05] ORIG 0x0825ddd9：具名指针槽在 get_state 检查**之后**才赋值
    //   （`mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)`），随后所有 msg 访问读该槽。
    MSG_BASE* pMsg = &msg;
    if (user->get_charac_job() == 9)
    {
        return 0xea;
    }
    if (user->get_charac_job() == 10)
    {
        return 0xea;
    }
    if (user->getCurCharFirstGrowType() == 0)
    {
        return 0x13;
    }
    if (user->CheckInTrade() != 0)
    {
        return 0xd8;
    }
    // [g05] ORIG 0x0825ddd9 有**具名指针槽** `local_18 = param_2`（-0x14），后续访问
    //   一律 `mov -0x14(%ebp),%eax` + disp（`movzbl 0xf(%eax)` / `movzwl 0xd(%eax)`；
    //   0x0825de5f 二次读同槽）⇒ 源码写具名 `pMsg = &msg` + pack(1) 视图成员，
    //   消我方 `add $0xf,%eax` 与 `cmpb $5,mem`/`jbe`（ORIG 为 `cmp $5,%al;jle`）。
#pragma pack(push, 1)
    struct GrowTypeMsgView
    {
        char m_pad00[0xd];
        unsigned short m_slotIdx;   // +0x0d
        signed char m_growCount;    // +0x0f
    };
#pragma pack(pop)
    if (reinterpret_cast<const GrowTypeMsgView*>(pMsg)->m_growCount < 0 ||
        reinterpret_cast<const GrowTypeMsgView*>(pMsg)->m_growCount > 5)
    {
        return 1;
    }
    const Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(
        1, static_cast<int>(static_cast<signed short>(
               reinterpret_cast<const GrowTypeMsgView*>(pMsg)->m_slotIdx)));
    CItem* citem = G_CDataManager()->find_item(item.m_addInfo);
    if (citem == 0)
    {
        return 0x11;
    }
    UserQuest* quest = user->getCurCharacQuestRefR();
    if (quest->isDoingJobChangeQuest())
    {
        return 0x16;
    }
    if (!user->verifyStackableItemActionPlace((StackableAction::Type)0x2e, citem))
    {
        return 0x17;
    }
    return 0;
}