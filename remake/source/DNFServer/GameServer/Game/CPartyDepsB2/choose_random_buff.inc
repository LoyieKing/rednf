int CParty::choose_random_buff(QuickParty::RandomBuffDungeon dungeon)
{
    if (!is_quick_party())
        return 11;
    if ((int)dungeon > 6)
        return 11;

    int totalProb =
        G_CGameManager()->GetQuickPartyRewardManager()
            ->get_random_buff_prob((QuickParty::RandomBuffType)11,
                                   dungeon);            // 行 11=汇总行
    if (totalProb == 0)
        return 11;

    int roll = get_rand_int(totalProb);                 // -0x14
    int acc = 0;                                        // -0x10
    for (int type = 0; type <= 10; ++type)              // -0xc
    {
        // ORIG 循环体内每次重新取管理器（无中间存储的链式调用）
        acc += G_CGameManager()->GetQuickPartyRewardManager()
                   ->get_random_buff_prob((QuickParty::RandomBuffType)type,
                                          dungeon);
        if (roll <= acc)
        {
            m_field1ae8 = type;                         // +0x1ae8
            return m_field1ae8;
        }
    }
    return 11;
