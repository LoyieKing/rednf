void cUserHistoryLog::DiePvP(ENUM_PVP_HISTORY_REASON reason, unsigned int a, unsigned int b,
                             int c, int d, int e, int f, const char* name)
{
    m_user->LogHistory("PVP,%d,%u,%u,%d,%d,%d,%d,\"%s\"",
                       (int)reason, a, b, c, d, e, f, name);
