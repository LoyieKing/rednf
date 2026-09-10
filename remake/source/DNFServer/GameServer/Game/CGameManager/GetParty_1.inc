CParty* CGameManager::GetParty(int index)
{
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.find(index);
    if (it == m_partyMap.end())
        return 0;
    return it->second;
