void cUserHistoryLog::CharacDel(int idx)
{
    TimeLog_ tl;
    tl.setTime();
    m_user->LogHistory("Charac-,%d",
        (int)m_user->m_characList[idx].m_field26);
