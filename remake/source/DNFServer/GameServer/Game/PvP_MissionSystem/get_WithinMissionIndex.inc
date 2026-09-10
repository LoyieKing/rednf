int PvP_MissionSystem::get_WithinMissionIndex(int idx) const
{
    std::pair<int, int> grade(0, 0);
    for (std::list<CMission*>::const_iterator it = m_kindMissionList[0x1b].begin();
         it != m_kindMissionList[0x1b].end(); ++it)
    {
        grade = (*it)->GetRankRange();
        if (grade.first == idx)
            return (*it)->GetMissionIndex();
    }
    return 0;
