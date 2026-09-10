bool CTradeSpace::is_empty()   // CTradeSpace.h:67 bool（DCoreB 落，定义同步）
{
    return m_bTrading ^ 1;
