CCommand* CCommandMap::get(int id)              // 0x084b3fc8
{
    std::map<int, CCommand*>::iterator it = m_byId.find(id);
    if (it != m_byId.end())
    {
        return it->second;
    }
    return 0;
}