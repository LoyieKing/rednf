// ORIG T 0x088999d8（非 const，nm 实证 _ZN 无 K）：0/1/2 →
// +0x3c/+0x54/+0x6c，其它 → 0。
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
}