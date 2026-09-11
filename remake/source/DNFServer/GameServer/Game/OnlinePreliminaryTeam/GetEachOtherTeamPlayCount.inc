// ORIG T 0x0858844c：find；miss→0；hit→value.count。
int COnlinePreliminaryTeam::GetEachOtherTeamPlayCount(int teamId) const
{
    std::map<int, PlayCountInfoEx>::const_iterator it =
        m_playCountMap.find(teamId);
    // [b28c-16] P222 正向包裹：ORIG 命中路径 fall-through、`mov $0` 落叶尾共享块；
    // 负极 `== end → return 0` 使 4.4 把零值块前置（jmp 落点互换）。
    if (it != m_playCountMap.end())
        return static_cast<int>(it->second.m_count);
    return 0;
}