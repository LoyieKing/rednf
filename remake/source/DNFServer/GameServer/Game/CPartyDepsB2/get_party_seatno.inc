// ============================================================================
// CParty::get_party_seatno —— ORIG 0x085a277a
// ============================================================================
int CParty::get_party_seatno(CUser* user)
{
    if (user == 0)
        return 0;

    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (!_checkValidUser(i))
            continue;
        if (m_member[i].m_pUser == user)
            return i;
    }
    return 0;
}