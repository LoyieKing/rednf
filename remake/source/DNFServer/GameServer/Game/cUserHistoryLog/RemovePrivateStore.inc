// ORIG T 0x8684586（报告全文）："Private Store-,%d,%d"
void cUserHistoryLog::RemovePrivateStore(int zoneA, int zoneB, char const* name)
{
    m_user->LogHistory("Private Store-,%d,%d", zoneA, zoneB);
}