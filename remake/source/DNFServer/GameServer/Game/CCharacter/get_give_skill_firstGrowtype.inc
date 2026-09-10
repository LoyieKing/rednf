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
