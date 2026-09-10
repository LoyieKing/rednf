Arad_GiftItem_Set::Arad_GiftItem_Set()
    : m_map(), m_kindMap()
{
    // ORIG 形态：make_pair 直接收字符串字面量 + ST_ITEM_KIND 枚举（pair 转换构造
    // 生成 _ZSt9make_pairIRA19_KcN17Arad_GiftItem_Set12ST_ITEM_KINDEE… 与 ORIG 一致）。
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM", ST_ITEM_KIND_1));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM2", ST_ITEM_KIND_2));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM3", ST_ITEM_KIND_3));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM4", ST_ITEM_KIND_4));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM5", ST_ITEM_KIND_5));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM6", ST_ITEM_KIND_6));
