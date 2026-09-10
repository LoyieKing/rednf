int CQuickParty::get_party_index(int idx)
{
    if (idx > 3 || idx < 0)
        return -1;
    return m_partyIndex[idx];
