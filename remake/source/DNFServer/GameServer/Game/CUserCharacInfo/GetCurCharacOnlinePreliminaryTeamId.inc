int CUserCharacInfo::GetCurCharacOnlinePreliminaryTeamId() const
{
    if (m_selected)
    {
        if (CUR->m_onlinePreliminaryTeam != 0)
            return CUR->m_onlinePreliminaryTeam->GetTeamId();
    }
    return 0;
