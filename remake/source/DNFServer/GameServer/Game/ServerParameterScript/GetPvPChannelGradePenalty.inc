int ServerParameterScript::GetPvPChannelGradePenalty(int grade) const
{
    if (m_map497c.empty())
    {
        return 0;
    }
    std::map<int, int>::const_iterator it = m_map497c.find(grade);
    if (it != m_map497c.end())
    {
        return it->second;
    }
    // ORIG 0x08693xx 实证：size>2(seta) 时 it **复用** = end() 临时+拷贝赋值，
    // --it 后取 second；否则落尾部 return 0 岛。
    if (m_map497c.size() > 2)
    {
        it = m_map497c.end();
        --it;
        return it->second;
    }
    return 0;
}