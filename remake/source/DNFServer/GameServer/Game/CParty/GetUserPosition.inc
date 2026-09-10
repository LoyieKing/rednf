int CParty::GetUserPosition(CUser* user)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        // ORIG 085b9e57-5d `lea (%ecx,%eax,1)+0x78; mov (%eax)` = m_member[i].m_pUser
        if (m_member[i].m_pUser == user)
            return i;
    }
    return -1;
