std::map<int, _BuyItem>* AdvanceAltarData::getBuyItemMap(AdvanceAltarShopType::T type)
{
    switch ((int)type)
    {
    case AdvanceAltarShopType::T_0: /* 仅覆盖整型 0 */
        return &m_buyItems1;
    case AdvanceAltarShopType::T_1:
        return &m_buyItems2;
    case AdvanceAltarShopType::T_2:
        return &m_buyItems3;
    default:
        return 0;
    }
