bool CBattle_Field::get_monster_list(std::list<_mapMonster>& mobList)
{
    if (m_pDungeon == NULL || m_pMap == NULL || m_party == NULL)
        return false;
    if (m_pDungeon->isTournamentDungeon())
    {
        m_tournamentRoundMap.clear();                                    // +0x13c
        bool ok = ConsistTournament(m_pMap, mobList, m_tournamentRoundMap);
        if (!ok)
            return false;
        m_dungeonClearTracer.SetMaxBossCount(m_party->get_member_count());
        SetTournamentCurRound(1);
    }
    else
    {
        mobList = m_pMap->get_monster_list();   // ORIG list operator= 自临时副本
    }
    return true;
