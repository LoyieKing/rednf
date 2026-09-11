// ============================================================================
// 队伍
// ============================================================================
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
                              // [K9GameRestA-S322 · §6.5.1 编码+文案] ORIG 0x08294e10：
                              // 实串 @0x8c16b70 = EUC-KR "이미 존재하는 번 할당 요청: %d"
                              // （发明英文文案已换回 ORIG 字节转义）。
                              "\xc0\xcc\xb9\xcc \xc1\xb8\xc0\xe7\xc7\xcf\xb4\xc2 \xb9\xe6 \xc7\xd2\xb4\xe7 \xbf\xe4\xc3\xbb: %d",
                              party->GetPartyIndex());
    }
    return party;
}