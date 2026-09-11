int ServerParameterScript::GetCleanChattingCount(int level)
{
    if (m_vec4954.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it = m_vec4954.begin();
    std::vector<std::pair<int, int> >::iterator e = m_vec4954.end();
    for (; it != e; ++it)
    {
        if ((*it).first >= level)
        {
            return (*it).second;
        }
    }
    --it;
    return (*it).second;
}