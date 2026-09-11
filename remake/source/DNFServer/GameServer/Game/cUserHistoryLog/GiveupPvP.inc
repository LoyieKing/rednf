// GiveupPvP(reason)：ORIG 0x868646c —— CUser::LogHistory(m_user, "PVP,%d", reason)。
void cUserHistoryLog::GiveupPvP(ENUM_PVP_HISTORY_REASON reason)
{
    m_user->LogHistory("PVP,%d", (int)reason);
}