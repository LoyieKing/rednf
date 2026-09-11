// ---- is_empty_quick_party @0x08269884：所有槽均为 -1 即为空 ----
bool CQuickParty::is_empty_quick_party()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_partyIndex[i] != -1)
            return false;
    }
    return true;
}