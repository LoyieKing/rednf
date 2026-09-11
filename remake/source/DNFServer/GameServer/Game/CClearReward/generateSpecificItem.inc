void CClearReward::generateSpecificItem(stGenerateRefData_t const& ref,
                                        stGenerateResult_t& result)
{
    // [b28d-solo-CClearReward] verdict=AE。与 power_war_dungeon_gold_clear_reward 同族修形：
    // ORIG 0x08538c9x 段 rc 具名指针局部（-0x24），fA/fB/rankIdx/sp/sum/baseCount
    // 全部无槽（表达式内联，P0/P183）；ushort 与档位经具名视图 disp 访问。
    unsigned char const* rc =
        reinterpret_cast<unsigned char const*>(&ref) + kRefClearRewardCtx;
    float weight =
        (m_partyBonus[reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelParty - 1] *
         m_dungeonGoldBonus[reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelGold]) *
        (1.0f + G_CDataManager()->m_serverParameter.GetExpBonusWeight(
                    reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_rankBonusIndex));
    stGoldGenTable_t* goldTable = reinterpret_cast<stGoldGenTable_t*>(
        m_common->getGoldGenTable(reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field00));
    int base = (goldTable->m_base * 0xaf) / 1000;
    // ORIG 0x08538cfx：sum 无槽（表达式内联到 (sum*base)/2）。
    int gold = static_cast<int>(
        static_cast<float>(
            (static_cast<int>(reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field08 +
                              (reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field0e * 2 +
                               reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field0a) * 2) *
             base) / 2) *
        weight);
    static const float kGoldDivisor = 2.0f;   // _DAT_08c9a6d8 = 0x40000000 = 2.0f
    // ORIG 循环条件每轮重读 rc[2]（`movzbl 0x2(%eax),%eax`）⇒ baseCount 不具名。
    for (int i = 0; i < reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelParty; ++i)
    {
        // piece/randSpan 均无槽（FPU 链连续 + GetRandomRange 内联实参）。
        int amount =
            (static_cast<int>((static_cast<float>(gold) / kGoldDivisor /
                               static_cast<float>(static_cast<int>(
                                   reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelParty))) *
                              weight) *
             (GetRandomRange(-goldTable->m_range, goldTable->m_range) + 100)) / 100;
        Inven_Item inv;
        inv.m_field1 = static_cast<unsigned char>(2);
        inv.m_addInfo = 0;
        inv.set_add_info(amount);
        result.m_dropItems.push_back(inv);
    }
}