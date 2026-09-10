CParty* CQuickParty::get_party_object(int idx)
{
    if (idx < 0 || idx > 3)
        return 0;
    if (m_partyIndex[idx] != -1)
    {
        // ORIG：GetParty 结果落具名槽 -0xc；m_partyIndex[idx] 两次完整下标读（P48/P52）
        CParty* party = G_CGameManager()->GetParty(m_partyIndex[idx]);
        if (party != 0)
            return party;
    }
    return 0;
