CParty* CGameManager::GetParty()
{
    CParty* party = m_partyPool.Acquire();
    if (party == 0)
        return 0;
    party->SetIDX(m_partyPool.GetIndex(party));
    int partyIndex = party->GetPartyIndex();
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.find(partyIndex);
    if (it == m_partyMap.end())
    {
        m_partyMap[party->GetPartyIndex()] = party;
        return party;
    }
    else
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0xa5f,
                              "party index(%d) already alloc", party->GetPartyIndex());
    }
    return party;
