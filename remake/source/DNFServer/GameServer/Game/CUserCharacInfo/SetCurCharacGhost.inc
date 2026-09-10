void CUserCharacInfo::SetCurCharacGhost(bool flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_ghost = flag;
    }
