advancealtar::BuyShopData::getBuyUpgradeDataVec(AdvanceAltarShopType::T type)
{
    switch (type)
    {
    case AdvanceAltarShopType::T_0: return &m_vecT0;
    case AdvanceAltarShopType::T_1: return &m_vecT1;
    case AdvanceAltarShopType::T_2: return &m_vecT2;
    default: return 0;
    }
}