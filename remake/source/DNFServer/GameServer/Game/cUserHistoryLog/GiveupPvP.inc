void cUserHistoryLog::GiveupPvP(ENUM_PVP_HISTORY_REASON reason)
{
    m_user->LogHistory("PVP,%d", (int)reason);
