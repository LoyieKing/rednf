// ---- SALE_INFO / BUY_INFO（ORIG W 0x08a832aa / 0x08ab1606 逐字段清零；
//      operator== ORIG W 0x08a832ee / 0x08ab161e，sete 直返 bool，
//      供 std::find 实例（0x08a839dd / 0x08ab16ec）比较）----
SALE_INFO::SALE_INFO()
{
    m_itemIdx = 0;
    m_rule = 0;
    m_price = 0;
    m_limit = 0;
    m_material = 0;
    m_materialCount = 0;
    m_field18 = 0;
}