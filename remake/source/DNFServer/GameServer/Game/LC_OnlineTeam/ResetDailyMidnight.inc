// ORIG 0x08588c5c：遍历 m_teamMap，逐 team 调 COnlinePreliminaryTeam::ResetDailyMidnight。
void online_preliminary::COnlinePreliminaryTeamMgr::ResetDailyMidnight()
{
    for (std::map<int, COnlinePreliminaryTeam*>::iterator it = m_teamMap.begin();
         it != m_teamMap.end(); ++it)
    {
        it->second->ResetDailyMidnight();
    }
}