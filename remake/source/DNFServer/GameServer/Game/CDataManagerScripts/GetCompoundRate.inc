stCompoundRate* STExpertJobScript::GetCompoundRate(int level)
{
    if (m_compoundRateVec.empty())
        return 0;
    std::vector<stCompoundRate>::iterator it = m_compoundRateVec.begin();
    std::vector<stCompoundRate>::iterator e = m_compoundRateVec.end();
    for (; it != e; ++it)
    {
        if ((*it).m_level >= level)
            return &(*it);
    }
    --it;  // 全部 < level：取末元素
    return &(*it);
