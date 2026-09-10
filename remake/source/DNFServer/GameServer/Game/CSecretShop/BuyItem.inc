void SHOPPER::BuyItem(int itemIdx, int count)
{
    // ORIG T 0x08ab13e8：内联 std::find；命中累加，未命中 ctor+push_back。
    std::vector<BUY_INFO>::iterator it;
    it = std::find(m_buys.begin(), m_buys.end(), itemIdx);
    if (it != m_buys.end()) {
        (*it).m_count += count;   // ORIG 用 operator*（de）
    } else {
        BUY_INFO b;
        b.m_itemIdx = itemIdx;
        b.m_count = count;
        m_buys.push_back(b);
    }
