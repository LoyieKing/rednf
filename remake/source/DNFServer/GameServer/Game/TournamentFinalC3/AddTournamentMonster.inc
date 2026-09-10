void CBattle_Field::AddTournamentMonster(const TournamentInfo& info,
    std::list<_mapMonster>& mobList, bool isUltimate)
{
    for (int i = 0; i <= 1; ++i)
    {
        _mapMonster mob;
        if (info.m_monster[i].m_mobId != 0)
        {
            mob.m_mobId = info.m_monster[i].m_mobId;
            mob.m_flag04 = 1;
            mob.m_field08 = 0;
            mob.m_levelAdd = 1;
            mob.m_field14 = 1;
            mob.m_spawnType = 0;
            if (isUltimate)
                mob.m_roleType = 3;
            else
                mob.m_roleType = 0;
            mob.m_champion = 0;
            mob.m_posX = 0;
            mob.m_orderIdx = 0;
            mob.m_hpPct = 100;
            mob.m_flag30 = 0;
            mobList.push_back(mob);
        }
    }
