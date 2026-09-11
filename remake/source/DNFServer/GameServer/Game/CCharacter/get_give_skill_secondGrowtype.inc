// ORIG get_give_skill_secondGrowtype @ 0x8348694。
// 注意：secondGrow 越界（<1 或 >2）时返回 1 且不拷贝（ORIG uVar2=1 分支）。
// ORIG 地址 = this + 0x330 + 0xc0*(grow+1) + 12*(secondGrow-1) + 8
//          = &m_secondGrowTypes[grow+1].m_skills[secondGrow-1]（同样跳过 0 号基础槽）。
// [2026-09-10 D06] bool（同族）。
bool CCharacter::get_give_skill_secondGrowtype(int grow, int secondGrow,
                                               std::vector<std::pair<int, int> >& out) const
{
    if (grow < 0 || grow > 5)
        return 0;
    if (secondGrow < 1 || secondGrow > 2)
        return 1;
    std::vector<std::pair<int, int> >::const_iterator it =
        m_secondGrowTypes[grow + 1].m_skills[secondGrow - 1].begin();
    std::vector<std::pair<int, int> >::const_iterator e =
        m_secondGrowTypes[grow + 1].m_skills[secondGrow - 1].end();
    for (; it != e; ++it)
    {
        const std::pair<int, int>& p = *it;
        out.push_back(p);
    }
    return 1;
}