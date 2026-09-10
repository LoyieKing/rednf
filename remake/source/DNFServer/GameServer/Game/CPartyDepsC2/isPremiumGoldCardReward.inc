bool CParty::isPremiumGoldCardReward(CDungeon const* dungeon, unsigned int& param)
{
    // [b28d-59 判词] 封 N82（ORIG 0x085b11ee）：83/80 条，残差=帧内槽序 +
    // 条件极性（jle/jg 落点相同）。语义等价。
    // ORIG 0x085b11ee：dungeon->get_standard_level() 结果存槽（local -0x28）；
    // G_CDataManager() 每次取字段时重新调用（-O0 无 CSE，ORIG 每轮 loop 4 次重取），
    // 无命名 mgr/ev 局部（P0 槽律）。字段经 m_serverParameter(+0x68)
    // m_premiumGoldCard4894(+0x4894) m_conds[i]（+0x08 起，步长 0x10）。
    const int standardLevel = dungeon->get_standard_level();   // W 0x080f9810

    // movzwl 0x4900 + cwtl：count = m_conditionCount（short @ +0x04）扩为 int 上限
    const int condCount =
        (int)G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conditionCount;

    for (int i = 0; i < condCount; ++i)
    {
        const int minLevel =
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_field0;
        const int maxLevel =
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_field4;
        const int prob =
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_field8;
        const unsigned int rewardItem = (unsigned int)
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_fieldc;

        // [b28d-solo-CPartyDepsC2] ORIG 0x085b128e 域：三 guard 各 `jle` 跳尾部
        // nop 中继岛（共享尾 return 0 块），循环正常结束再 `jmp` 尾块。早返形 4.4
        // 恒反转成 `jg 续; mov $0; jmp`（P222）⇒ 改负条件 goto 共享尾标签（P223 定案形）。
        if (minLevel < 1)
            goto FAIL;
        if (maxLevel < 1)
            goto FAIL;
        if (prob < 1)
            goto FAIL;
        if (standardLevel >= minLevel && standardLevel <= maxLevel)
        {
            const int dice = get_rand_int(1000);   // get_rand_int T 0x086b1b87
            if (dice <= prob)
            {
                param = rewardItem;
                return true;
            }
        }
    }
FAIL:
    return false;
