void cUserHistoryLog::StartPvP(ENUM_PVP_HISTORY_REASON reason, const char* n1, const char* n2,
                               const char* n3, const char* n4, const char* n5,
                               const char* n6, const char* n7, const char* n8)
{
    m_user->LogHistory("PVP,%d,\"%s\",\"%s\",\"%s\",\"%s\"", (int)reason, n1, n2, n3, n4);
    m_user->LogHistory("PVP,%d,\"%s\",\"%s\",\"%s\",\"%s\"", (int)reason, n5, n6, n7, n8);
