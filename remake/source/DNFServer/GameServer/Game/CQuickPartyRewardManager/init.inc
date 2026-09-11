// ORIG 0x0826d6b0（报告 QuickParty__CQuickPartyRewardManager/init.md）：
// 导入 Etc/QuickParty.etc 失败 → cMyTrace 报错返回 false；成功则 new 六个奖励
// 对象写入 m_rewards 表（basic[11]=CardGold、basic[12]=HellDifficulty、
// random[0]=KillMonsterExp、random[3]=EquipDropRate、random[9]=CoinLimit、
// random[10]=ReturnTicket），最后 set_rand_buff_total_prob 并返回 true。
// b28d-20: 修前 167 insn（每个 new 后 ~9 条 EH landing pad：operator delete +
// _Unwind_Resume）；按 P216 给 6 个奖励类 ctor 声明/定义加 throw()（见 .h 同批
// 判词）后 109=109 逐条 AE（ORIG 0x0826d6b0 无任何 EH cleanup）。
bool CQuickPartyRewardManager::init()
{
    if (!importQuickPartyScript("Etc/QuickParty.etc", this))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x92, 0);
        trace("QUICK_PARTY_LOG : QuickParty.etc FILE LOAD FAIL!!");
        return false;
    }

    m_rewards[0][11] = new RewardCardGold(BASIC_REWARD_CARD_GOLD, *this);          // +0x1ec
    m_rewards[0][12] = new HellPartyDifficulty(BASIC_REWARD_HELL_DIFFICULTY, *this);  // +0x1f0
    m_rewards[1][0]  = new KillMonsterExp(RANDOM_BUFF_INCREASE_EXP, *this);        // +0x1f4
    m_rewards[1][3]  = new EquipItemDropRate(RANDOM_BUFF_INCREASE_DROPPROB, *this);  // +0x200
    m_rewards[1][9]  = new CointLimitCount(RANDOM_BUFF_INCREASE_COINLIMIT, *this);   // +0x218
    m_rewards[1][10] = new ReturnAncientDungeonTicket(RANDOM_BUFF_RETURN_TICKET, *this);  // +0x21c

    set_rand_buff_total_prob();
    return true;
}