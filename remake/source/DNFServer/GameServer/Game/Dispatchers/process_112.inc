// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e3724 orig/ours=119/124 ndiff-blocks=16
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: setne  %al | mov    %eax,-0xc(%ebp) | cmpl   $0x0,-0xc(%ebp)
int Dispatcher_TutorialLevelUp::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    // [g02 试形A] ORIG 0x081e372a 直证：无 `cmpl $0x0,0xc(%ebp)` null 检查，
    //   直接 `mov 0xc(%ebp),%eax; mov %eax,(%esp); call get_state` ⇒ 源码无 `user == 0`。
    if (user->get_state() != 5)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    if (user->get_charac_level() != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    // [g02 试形A] P183/P-g03-b：ORIG 每处 `mov -0xc(%ebp),%eax; add $0xb24,%eax`
    //   重算地址（不缓存指针）⇒ 去掉 `CBattle_Field& bf` 引用缓存，成员访问原地重物化。
    if (party->m_battleField.get_cur_map() != 0xee49 &&
        party->m_battleField.get_cur_map() != 0xee51 &&
        party->m_battleField.get_cur_map() != 0xee58)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    char itemState = 0;                          // -0x0d
    int needExp = 0;                             // -0x14
    if (!user->CalLevelUpItemState(itemState, needExp, 1, 0x3b))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    int spBefore = 0;                            // -0x18（ORIG -0x18/-0x1c 置 0）
    int spAfter = 0;                             // -0x1c
    int expBonus1;                               // -0x20（ORIG 未出线）
    int expBonus2;                               // -0x24（ORIG 未出线）
    user->rewardExp(needExp, spBefore, spAfter, expBonus1, expBonus2,
                    (eExpAddReason)0xd, true);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x1e4);
    return 0;
}