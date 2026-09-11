// ---- SetItem @ 0x8510c1a ----
// [b28d-solo-CCeraShopGoods] AE：m_itemIdx 读 ORIG 0x08510c24 为
// `mov 0x14(%eax),%eax; mov %eax,%edx` 两段物化 ⇒ 按 P254 把
// STCeraShopItem::m_field14 改为 ENUM_CERA_SHOP_ITEM_NO（enum 源字段→int
// 目标保留 eax→edx 中转；int 字段 4.4 恒折单段）。原 N18a 判词作废
// （P248 档③类型旋钮）。sizeof/偏移/mangled 不变。
void CCeraShopGoods::SetItem(const STCeraShopItem& item)
{
    m_itemIdx = item.m_field14;
    m_stackCount = item.m_field1c;
    m_goldPrice = item.m_field2c;
    m_winPoint = item.m_field30;
    m_ceraPrice = item.m_field38;
    m_groupIdx = item.m_fieldc;
    m_featuredIdx = item.m_field10;
    m_sticker = (int)(signed char)item.m_field20;
    m_expiryDate = (int)(short)item.m_field22;
    m_bonusWeight = item.m_field44;
    m_field30 = item.m_field4c;
    m_giftDisallow = item.m_field50;
    m_ceraPointDisallow = item.m_field51;
    m_stackBuyable = item.m_field53;
    m_pItem = G_CDataManager()->find_item(m_itemIdx);
    m_mileagePrice = item.m_field34;
    m_subGroupIdx = (int)(signed char)item.m_field0;
}
