// ORIG T 0x084684ce (size 0x83) —— updateRanking.md
void WongWork::CDeathTowerRanking::updateRanking(unsigned int tableIdx,
                                                 unsigned int key,
                                                 unsigned int rank)
{
    RankTable& rt = *_getRankingTable(tableIdx);
    // ORIG: make_pair(key,rank) 临时(-0x14) → value_type 移动拷贝(-0x1c) →
    // insert(-0x28) 双层临时链（mangled 实证）。
    std::pair<RankTable::iterator, bool> rr =
        rt.insert(RankTable::value_type(std::make_pair(key, rank)));
    if (!rr.second)
        rr.first->second = rank;
}