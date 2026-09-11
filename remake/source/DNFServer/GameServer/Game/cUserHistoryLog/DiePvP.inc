// ORIG _ZN15cUserHistoryLog6DiePvPE23ENUM_PVP_HISTORY_REASONjjiiiiPKc @0x08686490
void cUserHistoryLog::DiePvP(ENUM_PVP_HISTORY_REASON reason, unsigned int a, unsigned int b,
                             int c, int d, int e, int f, const char* name)
{
    m_user->LogHistory("PVP,%d,%u,%u,%d,%d,%d,%d,\"%s\"",
                       (int)reason, a, b, c, d, e, f, name);
}