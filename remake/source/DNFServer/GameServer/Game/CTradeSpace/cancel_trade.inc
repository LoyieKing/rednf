// ---- cancel_trade @ 0x0852effa ----
bool CTradeSpace::cancel_trade()
{
    if (m_pTraders[0]->getCurCharacR() == 0 ||
        m_pTraders[1]->getCurCharacR() == 0) {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x788, 5);
        tr("CTradeSpace::cancel_trade()\t0 == m_pTraders[0]->getCurCharacR() || m_pTraders[1]->getCurCharacR()");
        return false;
    }
    return cancel_trade_for_china(0);
}