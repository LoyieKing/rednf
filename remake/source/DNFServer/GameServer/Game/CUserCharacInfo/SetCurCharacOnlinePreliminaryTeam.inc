void CUserCharacInfo::SetCurCharacOnlinePreliminaryTeam(
    online_preliminary::COnlinePreliminaryTeam* team)
{
    if (m_selected)
        m_selected->m_fieldE89 = reinterpret_cast<int>(team);
}
