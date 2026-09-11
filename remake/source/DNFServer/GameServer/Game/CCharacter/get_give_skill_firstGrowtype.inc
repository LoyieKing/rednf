// ORIG get_give_skill_firstGrowtype @ 0x83485d8。
// ORIG 地址 = this + 0x240 + 12*(grow+1)（set_data 填满 [0..6]，本查询跳过 0 号基础槽）。
// [2026-09-10 D06] bool（同 get_give_skill 判据；尾 `mov $0x1,%eax` 与 bool 兼容）。
bool CCharacter::get_give_skill_firstGrowtype(int grow,
                                              std::vector<std::pair<int, int> >& out) const
{
    if (grow < 0 || grow > 5)
        return 0;
    std::vector<std::pair<int, int> >::const_iterator it = m_giveSkillList[grow + 1].begin();
    std::vector<std::pair<int, int> >::const_iterator e = m_giveSkillList[grow + 1].end();
    for (; it != e; ++it)
    {
        const std::pair<int, int>& p = *it;
        out.push_back(p);
    }
    return 1;
}