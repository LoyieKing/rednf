// ---- std::pair 排序比较器（ORIG 085df55f T） ----
bool funDeathMatchKillCntCompare(const std::pair<int, int>& a,
                                 const std::pair<int, int>& b)
{
    return a.second > b.second;
}