void CQuickParty::set_party_index(int partyIdx)
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_partyIndex[i] == -1)
        {
            m_partyIndex[i] = partyIdx;
            return;
        }
    }
