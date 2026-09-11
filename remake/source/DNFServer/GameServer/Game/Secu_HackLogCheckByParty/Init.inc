void Secu_HackLogCheckByParty::Init(CParty* party)
{
    m_party2 = party;
    Secu_DungeonAverageCheck::SetPartyPtr(party);
}