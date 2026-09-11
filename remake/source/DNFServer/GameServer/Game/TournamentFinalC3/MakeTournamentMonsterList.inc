// ---------------------------------------------------------------------------
// CBattle_Field::MakeTournamentMonsterList @ 0x08304594
// 报告要点：round=1..4（cmpl $4/setle）；每轮 vec=roundMap[(short)round]
// （operator[] 083115dc）；peer 初值 -1，内层 i=0..15 找首个
// m_monster[0].m_mobId==0 的下标，偶数 peer=i+1 否则 peer=i-1 后 break；
// 未命中保持 peer=-1 仍无条件 AddTournamentMonster(vec[peer],...)（负下标
// UB —— ORIG 无防护，忠实保留）；round==4 传 true 否则 false。
// 循环后遍历 mobList 重编号 it->m_orderIdx=order++（+0x2c，list 迭代器
// 后置 it++ 推进）。
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
}