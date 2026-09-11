// ----------------------------------------------------------------------------
// 21. ORIG T 0x081b3046 sz 0x22f。金奖励：
// base=Find_max_value_lower_N(...).first；gold=(int)(term*base*period*avatar+0.01)；
// criticalDice=randInt(m_field3c)；critical=GetRewardCritical；
// GM 打印；result.m_0=(int)(term*base*period*critical+0.01)；
// nCritical=(int)(gold*critical+0.01)；result.m_4=nCritical-result.m_0。
// ----------------------------------------------------------------------------
int CHARAC_LOAD_MERCENARY::GetRewardGold(CUser* user,
                                         competition_reward_result& result) const
{
    // 引用局部槽形：ORIG `call; add $0xa8e4,%eax; mov %eax,-0x3c` 首句缓存
    // （同 GetRewardCritical 探针形）。
    const std::vector<std::pair<int, std::pair<int, int> > >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec8;
    unsigned long range;    // ORIG 槽 -0x40（高地址）：4.4 POD 槽逆序分配，
                            // 提前声明使其落 basePair sret 槽之后
    std::pair<int, int> basePair = Find_max_value_lower_N<std::pair<int, int> >(
        m_level, vec);
    int base = basePair.first;
    float period = GetPeriodBonus();
    float avatar = GetAvatarBonus();
    int term = GetCompetitionPeriodTerm(0);
    // 乘序按 ORIG x87 折叠序：fildl base→fmuls period→fildl term→fmulp→fmuls avatar
    int gold = (int)(base * period * term * avatar + 0.01f);

    static CMTRand probabilityRewardItem;
    range = static_cast<unsigned long>(
        G_CDataManager()->m_mercenarySystem.m_field3c);
    int criticalDice = probabilityRewardItem.randInt(range);
    float critical = GetRewardCritical(criticalDice);
    if (user->isGMUser())
    {
        static char buffer[0x100];
        sprintf(buffer,
                "term:%d, base gold:%d\nperiod:%.2f, avatar:%.2f\n"
                "%d * %.2f * %d * %.2f = %d\n"
                "critical Dice:%d/10,000 value:%.2f",
                term, base, (double)period, (double)avatar, base, (double)period,
                term, (double)avatar, gold, criticalDice, (double)critical);
        SendMessageToCUser(user, buffer);
    }
    int nCritical = (int)((float)gold * critical + 0.01f);
    result.m_0 = (int)(base * period * term * critical + 0.01f);
    result.m_4 = nCritical - result.m_0;
    return nCritical;
}