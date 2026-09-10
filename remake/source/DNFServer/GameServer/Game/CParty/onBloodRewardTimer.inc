bool CParty::onBloodRewardTimer(TIMER_MESSAGE msg)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85b6a48
    if (gen_timer_key(msg) == 0)
        return 0;
    if (m_battleField.getBloodState() == 0)
        return 0;

    if ((int)msg == 0x23)
    {
        if (m_battleField.getBloodState() < 3)
            return 0;
        for (int slot = 0; slot < 4; ++slot)            // -0x3c
        {
            if (!(get_user(slot) == 0 || !checkValidUser(slot)))
            {
                // ORIG onBloodRewardTimer.md +0x1c5..+0x214 装载序：param_3=
                // count(0)…param_10=item(3)，0→3 升序（先 group0 后 group3）。
                BloodClearRewardPacket(
                    slot,
                    m_battleField.m_bloodReward.getCurrentBloodRound(),
                    m_battleField.m_bloodReward.getRewardItemCount(0),
                    m_battleField.m_bloodReward.getRewardItem(0),
                    m_battleField.m_bloodReward.getRewardItemCount(1),
                    m_battleField.m_bloodReward.getRewardItem(1),
                    m_battleField.m_bloodReward.getRewardItemCount(2),
                    m_battleField.m_bloodReward.getRewardItem(2),
                    m_battleField.m_bloodReward.getRewardItemCount(3),
                    m_battleField.m_bloodReward.getRewardItem(3));
            }
        }
        m_battleField.setBloodState((ENUM_BLOOD_STATE)4);
        // [G7-sub] P0 槽律：ORIG 0x085b6c9d `call gen_timer_key` 后 `mov %eax,%esi`
        // （0x085b6cca `mov %esi,0x14(%esp)`）—— 结果**无栈槽** ⇒ 原码为实参表达式，
        // 无 `int key` 具名量；我方原 -0x4c 槽（帧 +0x10 之一）已删。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x24, 8,
                                    gen_timer_key((TIMER_MESSAGE)0x24), 0x24);
    }
    else if ((int)msg == 0x24)
    {
        if (m_battleField.getBloodState() < 4)
            return 0;
        for (int slot = 0; slot < 4; ++slot)            // -0x38
        {
            if (!(get_user(slot) == 0 || !checkValidUser(slot)))
            {
                for (int i = 0; i < m_battleField.m_bloodReward.getRewardItemCount(slot);
                     ++i)                               // -0x30
                {
// §P331 第二形：ORIG 把 getRewardItem 返回基址存 -0x28(%ebp) 后，每次字段访问都重算
// `mov -0x2c(%ebp),%eax; imul $0x3d,%eax,%eax; add -0x28(%ebp),%eax`（共 12 处；本方原
// 1 —— 被 `item` 一步起基址吸收）。拆成基址存槽 + 宏，即逐次重物化；i 为循环变量、
// 体内不改 i ⇒ 寻址同物，无可观察行为变化。实测 599→543。
                    Inven_Item* rewardBase = m_battleField.m_bloodReward.getRewardItem(slot);
