CUser* CTradeSpace::GetOppositeUser(CUser* user)
{
    if (m_bTrading != 0) {
        if (m_pTraders[0] == user)
            return m_pTraders[1];
        else
            return m_pTraders[0];
    }
    return 0;
}