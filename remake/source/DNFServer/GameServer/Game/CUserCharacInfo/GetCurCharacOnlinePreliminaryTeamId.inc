int CUserCharacInfo::GetCurCharacOnlinePreliminaryTeamId() const
{
    if (m_selected && m_selected->m_onlinePreliminaryTeam != 0)
        return reinterpret_cast<online_preliminary::COnlinePreliminaryTeam*>(m_selected->m_onlinePreliminaryTeam)->GetTeamId();
    return 0;
}
