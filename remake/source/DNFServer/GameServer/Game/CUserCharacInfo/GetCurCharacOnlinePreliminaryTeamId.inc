int CUserCharacInfo::GetCurCharacOnlinePreliminaryTeamId() const
{
    if (m_selected && m_selected->m_fieldE89 != 0)
        return reinterpret_cast<online_preliminary::COnlinePreliminaryTeam*>(m_selected->m_fieldE89)->GetTeamId();
    return 0;
}
