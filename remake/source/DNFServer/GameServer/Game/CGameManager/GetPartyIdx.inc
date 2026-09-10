int CGameManager::GetPartyIdx(CParty* party)
{
    int idx = m_partyPool.GetIndex(party);
    return idx;
