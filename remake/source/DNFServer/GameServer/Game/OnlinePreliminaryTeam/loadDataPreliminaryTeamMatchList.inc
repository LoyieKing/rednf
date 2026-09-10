int COnlinePreliminaryTeam::loadDataPreliminaryTeamMatchList(char* data)
{
    SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST* d =
        reinterpret_cast<SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST*>(data);
    for (int i = 0; d->m_count > i; ++i)
    {
        m_playCountMap[d->m_entries[i].m_otherTeamId].m_count =
            d->m_entries[i].m_playCount;
    }
    return 1;
