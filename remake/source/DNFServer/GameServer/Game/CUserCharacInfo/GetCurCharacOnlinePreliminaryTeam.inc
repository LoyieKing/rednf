CUserCharacInfo::GetCurCharacOnlinePreliminaryTeam()
{
    return m_selected ? reinterpret_cast<online_preliminary::COnlinePreliminaryTeam*>(m_selected->m_onlinePreliminaryTeam) : 0;
}
