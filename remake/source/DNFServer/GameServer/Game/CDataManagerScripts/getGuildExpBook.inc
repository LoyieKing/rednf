int GuildParameterScript::getGuildExpBook(int key)
{
    std::map<int, int>::iterator it = m_map10.find(key);
    if (it != m_map10.end()) return it->second;
    return 0;
