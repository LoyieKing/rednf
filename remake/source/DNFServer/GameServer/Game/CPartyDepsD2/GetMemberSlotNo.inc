CUser* CParty::GetMemberSlotNo(unsigned int characNo)
{
    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if ((unsigned int)m_member[i].m_pUser->get_charac_no(-1) == characNo)
            return m_member[i].m_pUser;
    }
    return 0;
