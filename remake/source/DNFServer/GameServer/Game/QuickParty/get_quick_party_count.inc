int CQuickParty::get_quick_party_count()
{
    int total = 0;
    for (int i = 0; i < 4; ++i) {
        if (m_partyIndex[i] == -1)
            continue;
        CParty* party = G_CGameManager()->GetParty(m_partyIndex[i]);
        if (party != 0)
        {
            // ORIG：get_member_count 结果落具名槽 -0xc 后再累加
            int memberCount = party->get_member_count();
            total += memberCount;
        }
    }
    return total;
