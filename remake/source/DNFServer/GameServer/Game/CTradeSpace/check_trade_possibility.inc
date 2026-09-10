bool CTradeSpace::check_trade_possibility()   // CTradeSpace.h:53 bool（DCoreB 落，定义同步；体仅 return 0/1）
{
    if (!m_bTrading)
        return 0;
    if (m_pTraders[0]->GetTradeSpace() != m_pTraders[1]->GetTradeSpace())
        return 0;
    if (m_pTraders[0]->get_state() != m_pTraders[1]->get_state())
        return 0;
    return 1;
