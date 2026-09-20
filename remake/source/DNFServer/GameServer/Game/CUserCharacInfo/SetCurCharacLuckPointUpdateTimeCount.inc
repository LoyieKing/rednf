void CUserCharacInfo::SetCurCharacLuckPointUpdateTimeCount(int count)
{
    if (m_selected)
    {
    m_selected->m_fieldE75 = count;
    }
}