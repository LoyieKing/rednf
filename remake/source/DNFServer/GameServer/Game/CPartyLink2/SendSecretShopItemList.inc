// ============================================================================
// CParty::SendSecretShopItemList —— ORIG T 0x085b9ecc（SendSecretShopItemList.md）。
// pos = GetUserPosition(user)；0<=pos<4 时经 GlobalData::s_secret_shop 把
// this+0x2a8+pos*0x1c（= m_secretShopInfo[pos]）处的 SALE_INFO vector 发给 user。
// [M5A_DispLink] ORIG 0x085b9f0b `lea 0x8(%eax),%edx`（单条 lea 取字段地址）vs 旧 cast
//   取址形恒产 `add $0x8,%eax; mov %eax,%edx`（OURS 多 1 条 = 29）。改直接写具名成员
//   链 `m_info[pos].m_retailer.m_sales`（类型即 std::vector<SALE_INFO>，无需 cast）。
// ============================================================================
void CParty::SendSecretShopItemList(CUser* user)
{
    int pos = GetUserPosition(user);
    // ORIG 0x085b9ef3 地址链 `pos*0x1c → lea 0x2a0 → add this → lea 0x8` 实证
    // vector = m_info[pos].m_retailer.m_sales（SECRET_SHOP_INFO +0x00）。
    if (pos >= 0 && pos < 4)
    {
        GlobalData::s_secret_shop->SendSecretShopItemList(
            user, this->m_secretShop.m_info[pos].m_retailer.m_sales);
    }
}