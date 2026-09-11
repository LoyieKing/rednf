// ---------------------------------------------------------------------------
// CBattle_Field::InsertTournamentInfo @ 0x08303e02
// 报告要点：并行迭代 infos/indexes —— 循环条件先判 infoIt!=infos.end() 再判
// idxIt!=indexes.end()（0x08303f0c..0x08303f5b），任一到尾即退出；循环体把
// *idxIt 截断为 short 写入元素首字段 m_roundOrder(+0x00)（0x08303e50 mov %ax,
// (%ebx)），随后以该字段自身作键 make_pair(m_roundOrder, *infoIt)（ORIG 两实参
// 均指向同一元素：operator->/operator* 结果一致）经 pair<short,TournamentInfo>
// → pair<const short,TournamentInfo> 转换构造后 map::insert（结果对 -0x48，
// 丢弃），最后 infos.erase/infoIt、indexes.erase/idxIt 双向推进 —— 迭代器仅靠
// erase 返回值前进（0x08303eb0..0x08303ef4），无 ++it、无空表防护 —— 忠实保留。
void CBattle_Field::InsertTournamentInfo(std::vector<int>& indexes,
    std::vector<TournamentInfo>& infos,
    std::map<short, TournamentInfo>& roundMap)
{
    std::vector<TournamentInfo>::iterator infoIt = infos.begin();
    std::vector<int>::iterator idxIt = indexes.begin();

    while (infoIt != infos.end() && idxIt != indexes.end())
    {
        infoIt->m_roundOrder = (short)*idxIt;
        roundMap.insert(std::make_pair(infoIt->m_roundOrder, *infoIt));

        infoIt = infos.erase(infoIt);
        idxIt = indexes.erase(idxIt);
    }
}