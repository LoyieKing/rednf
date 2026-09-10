void cUserHistoryLog::SetTrader(const char* accName, const char* characName)
{
    strncpy(m_traderCharac, accName, 0x1e);
    strncpy(m_traderAccount, characName, 0x1e);
