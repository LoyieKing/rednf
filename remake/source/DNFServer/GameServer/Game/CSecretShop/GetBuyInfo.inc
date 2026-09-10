BUY_INFO* SHOPPER::GetBuyInfo(int itemIdx)
{
    // ORIG T 0x08ab1344：std::find + ne 比较。
    std::vector<BUY_INFO>::iterator it;
    it = std::find(m_buys.begin(), m_buys.end(), itemIdx);
    if (it != m_buys.end())
        return &*it;
    return 0;
