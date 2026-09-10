void SecretShopScript::CopyItem(secretshop::SALE_INFO& sale, stSaleInfo& info)
{
    sale.m_itemIdx = info.m_itemIdx;
    sale.m_rule = info.m_rule;
    sale.m_limit = info.m_limit;
    if (sale.m_rule == (char)0) {
        sale.m_price = GetRandItemPrice(info.m_price);
    } else if (sale.m_rule == (char)1) {
        sale.m_material = info.m_price;
        sale.m_materialCount = info.m_materialCount;
    }
