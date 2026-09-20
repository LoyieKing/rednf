void CUserCharacInfo::IncCurCharacLuckPointUpdateTimeCount()
{
    if (m_selected)
        m_selected->m_fieldE75 += 1;
}