void CAutoMarketConditionsControl::SendNotiSetDurability()
{
    SendNotiSetDurability(static_cast<int>(m_durCurrent * 100.0));
