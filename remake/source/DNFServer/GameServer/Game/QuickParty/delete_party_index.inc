bool CQuickParty::delete_party_index(int partyIdx)
{
    for (int i = 0; i < 4; ++i) {
        if (m_partyIndex[i] == partyIdx) {
            m_partyIndex[i] = -1;
            return true;
        }
    }
    return false;
