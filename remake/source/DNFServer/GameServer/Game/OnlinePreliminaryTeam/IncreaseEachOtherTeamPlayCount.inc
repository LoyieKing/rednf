void COnlinePreliminaryTeam::IncreaseEachOtherTeamPlayCount(int teamId)
{
    std::map<int, PlayCountInfoEx>::iterator it = m_playCountMap.find(teamId);
    if (it != m_playCountMap.end())
    {
        it->second.m_flag = 1;
        it->second.m_count += 1;
    }
    else
    {
        if (m_playCountMap.size() > 999)
        {
            // ORIG __PRETTY_FUNCTION__ 行号 0xca（202）
            cMyTrace(__PRETTY_FUNCTION__, 0xca, 0)(
                "Online Preliminary each other team play count map size over! %d",
                static_cast<unsigned int>(m_playCountMap.size()));
            return;
        }
        m_playCountMap.insert(std::make_pair(teamId, PlayCountInfoEx(1, true)));
    }
    m_hasLoseFlag = 1;
