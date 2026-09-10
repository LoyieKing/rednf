QuickParty::CQuickParty* CGameManager::GetQuickParty(int index)
{
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(index);
    if (it == m_quickPartyMap.end())
        return 0;
    return it->second;
