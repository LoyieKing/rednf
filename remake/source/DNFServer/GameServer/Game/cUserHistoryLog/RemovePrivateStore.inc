void cUserHistoryLog::RemovePrivateStore(int zoneA, int zoneB, char const* name)
{
    m_user->LogHistory("Private Store-,%d,%d", zoneA, zoneB);
