// ---------------------------------------------------------------------------
// CBattle_Field::MakeRoundInfo @ 0x08303f66
// 报告要点：round 从 1 计到 4（0x083040d7 cmp $0x4 / setle，含 4）。每轮：
// map::begin 后构造空 snapshot vector（0x08303f7a..0x08303f95），内层 while
// （每轮重取 map::end，0x08303fd6）将全部 second（pair+0x04）push_back；
// 迭代器用后置 it++（ORIG 0x083113d8 三参 operator++(int)，含丢弃的临时拷贝）；
// 插入 roundInfoMap（返回值丢弃），~pair×2 后调 MakeNextRoundInfo(round,
// roundMap) 推进对阵（0x0830410c），最后 ~vector；循环外调
// MakeTournamentMonsterList(roundInfoMap, mobList)（0x08304594）。
// roundMap 不预先 clear —— 忠实保留。
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
}