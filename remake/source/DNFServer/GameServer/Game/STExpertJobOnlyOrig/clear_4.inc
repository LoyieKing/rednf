void STExpertJobScript::stDollControllerResultInfo::clear()
{
    for (int i = 0; i <= 2; ++i)
    {
        m_count[i] = 0;
        m_vec[i].clear();
    }
