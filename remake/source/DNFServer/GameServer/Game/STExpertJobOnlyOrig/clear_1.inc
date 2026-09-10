void STExpertJobScript::stMonsterCardBindInfo::clear()
{
    for (int i = 0; i <= 3; ++i)
    {
        m_count[i] = 0;
        m_vec[i].clear();
    }
