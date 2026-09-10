int CQuickPartyRewardManager::get_random_buff_prob(RandomBuffType type,
                                                   RandomBuffDungeon dungeon)
{
    if ((int)type > 11 || (int)dungeon > 6)
        return 0;
    return m_probs[(int)type][(int)dungeon];
