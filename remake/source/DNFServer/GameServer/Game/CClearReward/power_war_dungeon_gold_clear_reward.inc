void CClearReward::power_war_dungeon_gold_clear_reward(
    stGenerateRefData_t const& ref, stGenerateResult_t& result, int level)
{
    // [b28d-solo-CClearReward] verdict=AE。ORIG 0x08538e6e..0x08538ea4：rc 具名指针局部（-0x20）；
    // fA/fB **无槽**（`flds 0xc(%eax,%edx,4)` 直读成员，P0/P183），书写序照 P243
    // （先 flds 左操作数 = m_partyBonus）；rankIdx/sp 亦无槽（内联表达式）。
    unsigned char const* rc =
        reinterpret_cast<unsigned char const*>(&ref) + kRefClearRewardCtx;
    // ctx 视图不落具名指针槽（ORIG 仅 rc 一个指针槽 -0x20）。
    float weight =
        (m_partyBonus[reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelParty - 1] *
         m_dungeonGoldBonus[reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelGold]) *
        (1.0f + G_CDataManager()->m_serverParameter.GetExpBonusWeight(
                    reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_rankBonusIndex));
    stGoldGenTable_t* goldTable = reinterpret_cast<stGoldGenTable_t*>(
        m_common->getGoldGenTable(static_cast<unsigned char>(level)));
    int base = (goldTable->m_base * 0xaf) / 1000;
    // ORIG 0x08538f0b：sum 无槽（表达式内联到 (sum*base)/2），ushort 经视图 disp 访问。
    // ORIG 0x08538f61：piece 亦无槽（FPU 链连续，末尾 fistpl 截断），故一并内联。
    int gold = static_cast<int>(
        static_cast<float>(
            (static_cast<int>(reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field08 +
                              (reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field0e * 2 +
                               reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field0a) * 2) *
             base) / 2) *
        weight);
    static const float kGoldDivisor = 2.0f;   // _DAT_08c9a6d8 = 2.0f
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
