bool CInventory::IsCoinUsable() const
{
    return m_coin != 0 || m_payCoin != 0 || m_eventCoin != 0;
