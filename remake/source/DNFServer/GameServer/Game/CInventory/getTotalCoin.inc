unsigned int CInventory::getTotalCoin() const
{
    return m_coin + m_eventCoin + m_payCoin;
}