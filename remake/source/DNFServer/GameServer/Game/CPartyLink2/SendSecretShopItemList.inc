void CParty::SendSecretShopItemList(CUser* user)
{
    int pos = GetUserPosition(user);
    // ORIG 0x085b9ef3 地址链 `pos*0x1c → lea 0x2a0 → add this → lea 0x8` 实证
    // vector 落在 m_secretShopInfo[pos] 起始；取具名数组元素地址可复现
    // `add 0x8(%ebp),%eax`（cast 取址形恒产 mov this,%ecx 中转，CParty.h:761 注释同证）。
    if (pos >= 0 && pos < 4)
    {
        GlobalData::s_secret_shop->SendSecretShopItemList(
            user, *reinterpret_cast<std::vector<secretshop::SALE_INFO>*>(
                      &this->m_secretShop.m_info[pos]));
    }
