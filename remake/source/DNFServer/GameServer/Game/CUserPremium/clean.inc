void stPremiumTable_t::clean()
{
    // ORIG 0x891ae68：清零顺序照抄（循环 applyDayMask=-1 在前，随后
    // kind/checkTerm/overSkillLevel/cerashopDiscountRate/useTargetServer/
    // expireTime/unlimitFatigue/fatigueRate/coin/expRate/advantageItems/
    // goldBonus/overEquipCount/overEquipPairs/returnItemCount/returnItemPairs/
    // hpMax..field118/bonusExp/questItemDropRate/dropRates/targetItemKey/premiumItems）。
    for (int i = 0; i <= 6; ++i)
        m_applyDayMask[i] = -1;
    m_kind = 0;
    m_checkTerm = 0;
    m_overSkillLevel = 0;
    m_cerashopDiscountRate = 0;
    m_useTargetServer = 0;
    m_expireTime = 0;
    m_unlimitFatigue = 0;
    m_fatigueRate = 0;
    m_coin = 0;
    m_expRate = 0;
    memset(m_advantageItems, 0, sizeof(m_advantageItems));
    m_goldBonus = 0;
    m_overEquipCount = 0;
    memset(m_overEquipPairs, 0, sizeof(m_overEquipPairs));
    m_returnItemCount = 0;
    memset(m_returnItemPairs, 0, sizeof(m_returnItemPairs));
    memset(&m_hpMax, 0, 0x54);
    m_bonusExp = 0;
    m_questItemDropRate = 0;
    m_independentDropRates.clear();
    m_targetItemKey = 0;
    m_premiumItems.clear();
