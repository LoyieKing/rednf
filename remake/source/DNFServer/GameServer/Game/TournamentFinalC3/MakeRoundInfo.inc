void CBattle_Field::MakeRoundInfo(std::map<short, TournamentInfo>& roundMap,
    std::map<short, std::vector<TournamentInfo> >& roundInfoMap,
    std::list<_mapMonster>& mobList)
{
    for (int round = 1; round <= 4; ++round)
    {
        std::map<short, TournamentInfo>::iterator it = roundMap.begin();
        std::vector<TournamentInfo> snapshot;
        while (it != roundMap.end())
        {
            snapshot.push_back(it->second);
            it++;
        }

        roundInfoMap.insert(std::make_pair(round, snapshot));
        MakeNextRoundInfo(round, roundMap);
    }

    MakeTournamentMonsterList(roundInfoMap, mobList);
