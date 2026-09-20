void CUserCharacInfo::SetCurCharacGhost(bool flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_fieldE66 = flag;
    }
}