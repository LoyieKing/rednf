// ===========================================================================
// 经验主链
// ===========================================================================
// ORIG 0x0868b20c：结算前后 SP 快照 + gain_exp_sp；升级后按场景广播基础信息包，
// 成长事件触发 decide_growth_power_reward_system（producer=CUserOnlyOrig.cpp）。
void CUser::rewardExp(int expBase, int& bonusA, int& bonusB,
                      int& spBefore, int& spAfter,
                      ::eExpAddReason reason, bool triggerGrowth)
{
    PacketGuard packet;

    // P52/P24/P134：`spBefore = skillR->f(index)` 单语句内联实参，索引值先物化
    // callee-saved %ebx 并跨 getCurCharacSkillR 调用存活，不落栈槽（ORIG %ebx 形态）。
    spBefore = reinterpret_cast<SkillSlot*>(const_cast<char*>(getCurCharacSkillR()))
        ->get_remain_sp_at_index(
            static_cast<ENUM_SKILL_TREE_KIND>(GetCurCharacSkillTreeIndex()));

    calcHelpAbuseRatio(static_cast<unsigned int>(expBase), false);
    bool levelUp = gain_exp_sp(expBase, bonusA, bonusB,
                               (eExpAddReason)reason, 0, triggerGrowth);

    spAfter = reinterpret_cast<SkillSlot*>(const_cast<char*>(getCurCharacSkillR()))
        ->get_remain_sp_at_index(
            static_cast<ENUM_SKILL_TREE_KIND>(GetCurCharacSkillTreeIndex()));

    // 极性：`if (levelUp) {...}` 使 4.4 发 `cmpb $0x0,-0xd(%ebp); je`（ORIG 同款）；
    // `if (!levelUp) return;` 早退会发 movzbl/xor $1/test 反形。
    if (levelUp)
    {
        if (m_chState == 3)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(0);
            packet.put_short(1);
            make_basic_info(reinterpret_cast<char*>(&packet), '\0');
            packet.finalize(true);
            G_GameWorld()->send_all(packet, this);
        }
        else if (m_chState == 5 || m_chState == 8)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info(reinterpret_cast<char*>(&packet), '\1');
            packet.finalize(true);
            CParty* party = static_cast<CParty*>(GetParty());
            if (party != 0)
            {
                party->send_to_party(packet);
            }
        }

        if (triggerGrowth)
        {
            decide_growth_power_reward_system();
        }
    }
}