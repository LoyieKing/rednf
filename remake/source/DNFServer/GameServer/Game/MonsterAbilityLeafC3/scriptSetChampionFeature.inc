// ---------------------------------------------------------------------------
// scriptSetChampionFeature — ORIG 0x88b9f12：对脚本列表逐项 addChampionFeature。
// ---------------------------------------------------------------------------
void scriptSetChampionFeature(std::vector<int>& scriptList, int& count,
                              std::vector<int>& temp)
{
    for (std::vector<int>::size_type i = 0; i < scriptList.size(); ++i)
        addChampionFeature(static_cast<CHAMPION_FEATURES>(scriptList[i]),
                           count, temp);
}