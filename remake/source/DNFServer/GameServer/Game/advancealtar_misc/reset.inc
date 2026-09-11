// ============================================================================
// BuyShopData::reset（ORIG T 0x088a1090：+0=0 + 三 vector clear）
// ============================================================================
void BuyShopData::reset()
{
    m_nField0 = 0;
    m_vecT0.clear();
    m_vecT1.clear();
    m_vecT2.clear();
}