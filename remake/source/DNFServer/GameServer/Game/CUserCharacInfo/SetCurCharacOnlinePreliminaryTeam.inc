void CUserCharacInfo::SetCurCharacOnlinePreliminaryTeam(
    online_preliminary::COnlinePreliminaryTeam* team)
{
    if (m_selected)
    {
    CUR->m_onlinePreliminaryTeam = team;
    }
}