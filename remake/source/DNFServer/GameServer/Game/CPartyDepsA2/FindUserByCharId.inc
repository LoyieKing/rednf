int CParty::FindUserByCharId(unsigned int characNo)
{
    for (int local_10 = 0; local_10 <= 3; ++local_10)
    {
        if (!_checkValidUser(local_10))
            continue;
        if (static_cast<unsigned int>(
                m_member[local_10].m_pUser->get_charac_no(-1)) == characNo)
        {
            return reinterpret_cast<int>(m_member[local_10].m_pUser);
        }
    }
    return 0;
