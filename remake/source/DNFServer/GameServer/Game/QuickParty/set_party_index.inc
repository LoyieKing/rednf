// ---- set_party_index @0x0826978c：写入首个空槽（-1），全满则丢弃 ----
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
}