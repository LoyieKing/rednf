void EmblemCompound::clearScript()
{
    m_cost.clear();
    std::map<int, Result*>::iterator it = m_result.begin();
    std::map<int, Result*>::iterator endIt = m_result.end();
    for (; it != endIt; ++it)
        ::operator delete(it->second);
    m_result.clear();
