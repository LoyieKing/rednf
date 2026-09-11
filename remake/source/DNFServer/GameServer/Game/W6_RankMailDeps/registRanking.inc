// ORIG T 0x084683da (size 0xf3) —— registRanking.md
// [b28d-55] DIFF(N) — 修法：value_type(make_pair(key,rank)) 复现 ORIG
//   make_pair<unsigned&,unsigned&> + pair<const unsigned,unsigned>::C1IjjE
//   （P178/P86），P167 CLEAR(10/10)。残余 = 寄存器/槽位差（N82 族）。
//   ORIG 0x084683da。
void WongWork::CDeathTowerRanking::registRanking(unsigned int tableIdx,
                                                 unsigned int key,
                                                 unsigned int rank)
{
    // asm +0x18..+0x81: ranking 子表 insert(key→rank)，重复键改写。
    RankTable& rt = *_getRankingTable(tableIdx);
    std::pair<RankTable::iterator, bool> rr =
        rt.insert(RankTable::value_type(std::make_pair(key, rank)));
    if (!rr.second)
        rr.first->second = rank;
    // asm +0x93..+0xf2: best 子表 find(key)；命中则其记录存故名次(+0x94)=rank。
    BestMap& bt = *_getBestRecordTable(tableIdx);
    BestMap::iterator bi = bt.find(key);
    if (bi != bt.end())
        bi->second.m_field94 = static_cast<int>(rank);
}