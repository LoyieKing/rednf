CCommand* CCommandMap::get(std::string& name)   // 0x084b402c
{
    std::unordered_map<std::string, CCommand*>::iterator it = m_byName.find(name);
    if (it != m_byName.end())
    {
        return it->second;
    }
    return 0;
