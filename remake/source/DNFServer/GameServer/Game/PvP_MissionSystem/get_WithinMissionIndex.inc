// ORIG 0x85e2f00（b28d-70 P167 CLEAR）：循环外构造 `std::pair<int,int> grade(0, 0)`
// （0x85e2f06/0x85e2f0d 双 movl $0 + C1IiiEEOT_OT0_ 0x081627d4），循环内
// `grade = (*it)->GetRankRange()`（aSEOS0_ 0x080ddf1c move 赋值），`grade.first == idx`
// 命中即返回 GetMissionIndex()；无匹配返回 0。ORIG 循环体无判空（P140：0x85e2f57 直接
// operator*→GetRankRange），故不写 `if (!*it)`。init/cond 两处**各调一次**
// `m_kindMissionList[0x1b]` 的 const operator[]（0x85e2f3e/0x85e2fc3）⇒ 不缓存 list
// 引用（P48/P183 重物化）。
// [b28d-70] 判词：DIFF→AE 70/71；P167 CLEAR 12=12（C1IiiE/aSEOS0_ 双符号命中）。
int PvP_MissionSystem::get_WithinMissionIndex(int idx) const
{
    std::pair<int, int> grade(0, 0);
    for (std::list<CMission*>::const_iterator it = m_kindMissionList[0x1b].begin();
         it != m_kindMissionList[0x1b].end(); ++it)
    {
        grade = (*it)->GetRankRange();
        if (grade.first == idx)
            return (*it)->GetMissionIndex();
    }
    return 0;
}