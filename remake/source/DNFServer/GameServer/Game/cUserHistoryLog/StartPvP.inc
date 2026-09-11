// ORIG _ZN15cUserHistoryLog8StartPvPE23ENUM_PVP_HISTORY_REASONPKcS2_S2_S2_S2_S2_S2_S2_
//      @0x086863b6：两条 PVP 行（前 4 名/后 4 名角色名）。
void cUserHistoryLog::StartPvP(ENUM_PVP_HISTORY_REASON reason, const char* n1, const char* n2,
                               const char* n3, const char* n4, const char* n5,
                               const char* n6, const char* n7, const char* n8)
{
    m_user->LogHistory("PVP,%d,\"%s\",\"%s\",\"%s\",\"%s\"", (int)reason, n1, n2, n3, n4);
    m_user->LogHistory("PVP,%d,\"%s\",\"%s\",\"%s\",\"%s\"", (int)reason, n5, n6, n7, n8);
}