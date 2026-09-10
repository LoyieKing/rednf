void online_preliminary::COnlinePreliminaryTeamMgr::ResetDailyMidnight()
{
    for (std::map<int, COnlinePreliminaryTeam*>::iterator it = m_teamMap.begin();
         it != m_teamMap.end(); ++it)
    {
        it->second->ResetDailyMidnight();
    }
