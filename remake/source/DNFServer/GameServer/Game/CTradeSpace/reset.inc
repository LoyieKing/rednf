void CTradeSpace::reset()
{
    lock();
    _SetLock(0);
    m_bTrading = 0;
    if (m_pTraders[0] != 0) {
        m_pTraders[0]->SetTradeSpace(-1);
        m_pTraders[0]->m_historyLog.SetTrader("", "");
    }
    if (m_pTraders[1] != 0) {
        m_pTraders[1]->SetTradeSpace(-1);
        m_pTraders[1]->m_historyLog.SetTrader("", "");
    }
    m_nMoney[0] = 0;
    m_nMoney[1] = 0;
    m_nState[0] = 2;
    m_nState[1] = 2;
    memset(&m_slotsData[0], 0, 0x798);
    memset(&m_slotsData[1], 0, 0x798);
    memset(m_fieldF68, 0, sizeof(m_fieldF68));
    unlock();
