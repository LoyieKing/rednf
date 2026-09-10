void CBattle_Field::MakeTournamentMonsterList(
    std::map<short, std::vector<TournamentInfo> >& roundInfoMap,
    std::list<_mapMonster>& mobList)
{
    for (int round = 1; round <= 4; ++round)
    {
        const std::vector<TournamentInfo>& vec = roundInfoMap[(short)round];   // ORIG const operator[]
        int peer = -1;
        for (int i = 0; i < 0x10; ++i)
        {
            if (vec[i].m_monster[0].m_mobId == 0)
            {
                if ((i & 1) == 0)
                    peer = i + 1;
                else
                    peer = i - 1;
                break;
            }
        }

        if (round == 4)
            AddTournamentMonster(vec[peer], mobList, true);
        else
            AddTournamentMonster(vec[peer], mobList, false);
    }

    std::list<_mapMonster>::iterator it = mobList.begin();
    int order = 0;
    while (it != mobList.end())
    {
        it->m_orderIdx = order++;
        it++;
    }
