// ORIG _ZN15cUserHistoryLog9CharacDelEi @0x08682ae0（与 CharacAdd 同型，"Charac-,"）
void cUserHistoryLog::CharacDel(int idx)
{
    TimeLog_ tl;
    tl.setTime();
    m_user->LogHistory("Charac-,%d",
        (int)m_user->m_characList[idx].m_job);
}