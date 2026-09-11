// ----------------------------------------------------------------------------
// ORIG T 0x081446a2 _ZN8WongWork10CBossTower15InsertRewardExpEv：
// 逐有效成员：bonusExp/extraFlag 局部清零后 gain_exp_sp(m_rewardExp[i], ...,
// eExpAddReason 2, 0, false)；成功时 SendNotiPacket(eSendTarget 2,
// ENUM_NOTIPACKET 2, 1)。恒返 1。
// [batch-19 AE] 4→0：两处 P0 形订正——
//  ① ORIG +0x53 起 `xor$1;test%al,%al;jne +0xe2` 直测 %eax（choose 无槽）⇒
//    成员有效性判定必为 `if (member==0 || checkValidUser(i)!=1) continue;`
//    内联形；写 `bool invalid = ...;` 具名 bool 局部 ⇒ 4.4 多
//    `mov %al,-0xa; cmpb $0,-0xa` 两段（+2/−1）。
//  ② ORIG +0x60 `movb $0x0,-0x9(%ebp)` 在 gain_exp_sp call **之前** ⇒ 原码
//    `bool gained = false;` 后接 `gained = member->gain_exp_sp(...)` 两句；
//    合写成 `bool gained = call();` 丢预清行（−1）。
// ----------------------------------------------------------------------------
bool CBossTower::InsertRewardExp()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        if ((member == 0) || (m_party->checkValidUser(i) != 1))
            continue;

        int bonusExp = 0;                        // ebp-0x18
        int extraFlag = 0;                       // ebp-0x1c
        bool gained = false;
        gained = member->gain_exp_sp(
            m_rewardExp[i], bonusExp, extraFlag,
            (CUser::eExpAddReason)2, 0, false);
        if (gained)
        {
            member->SendNotiPacket((CUser::eSendTarget)2,
                                   (ENUM_NOTIPACKET)2, 1);
        }
    }
    return 1;
}