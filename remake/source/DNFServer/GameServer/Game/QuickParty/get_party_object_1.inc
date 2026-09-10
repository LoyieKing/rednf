CParty* CQuickParty::get_party_object()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_partyIndex[i] == -1)
            continue;
        CParty* party = G_CGameManager()->GetParty(m_partyIndex[i]);
        if (party != 0)
            return party;
    }
    return 0;
