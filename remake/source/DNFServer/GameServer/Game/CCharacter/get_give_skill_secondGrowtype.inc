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
