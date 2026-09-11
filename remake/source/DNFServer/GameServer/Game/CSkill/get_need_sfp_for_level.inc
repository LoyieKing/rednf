int CSkill::get_need_sfp_for_level(int level, int targetLevel) const
{
    std::list<int>::const_iterator it;
    int count = (int)m_needSFP.size();
    if (count < targetLevel)
    {
        return -1;
    }
    if (level < 0)
    {
        return -1;
    }
    int total = 0;
    it = m_needSFP.begin();
    for (int i = 1; i < targetLevel; ++i)
    {
        if (i > level && i < targetLevel)
        {
            total += *it;
        }
        it++;
    }
    total += *it;
    return total;
}