void CUserCharacInfo::SetCurCharacOnlinePreliminaryTeam(
    online_preliminary::COnlinePreliminaryTeam* team)
{
    if (m_selected)
        m_selected->m_onlinePreliminaryTeam = reinterpret_cast<int>(team);
}
