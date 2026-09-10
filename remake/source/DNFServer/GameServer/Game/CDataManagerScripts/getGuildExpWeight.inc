int GuildParameterScript::getGuildExpWeight(int level)
{
    std::map<int, int>::iterator it = m_map44.find(level);
    if (it != m_map44.end()) return it->second;
    return 0;
