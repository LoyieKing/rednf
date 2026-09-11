// ORIG T 0x0826d860（报告 get_random_buff_prob.md）：type∈[0,11] 且
// dungeon∈[0,6] 时返回 m_probs[type][dungeon]，否则返回 0。
// ORIG 仅上界比较（cmp $0xb/jg、cmp $0x6/jle），负值与原版一致落入查表路径。
int CQuickPartyRewardManager::get_random_buff_prob(RandomBuffType type,
                                                   RandomBuffDungeon dungeon)
{
    if ((int)type > 11 || (int)dungeon > 6)
        return 0;
    return m_probs[(int)type][(int)dungeon];
}