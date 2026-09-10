void STSecondGrowTypeInfo::clear()
{
    for (int i = 0; i <= 1; ++i)
    {
        m_addInfo[i].clear();
        m_skills[i].clear();
    }
    memset(&m_fieldbc, 0, 2);
