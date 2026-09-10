void CParty::ConnectP2P(CUser* user, char* ip, int port)
{
    if (get_state() != 1)
    {
        throw static_cast<ENUM_ERROR>(0x13);
    }

    bool allConnected = true;
    for (int i = 0; i < 4; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_member[i].m_pUser == user)
        {
            if (m_member[i].m_fieldc == 1)
            {
                throw static_cast<ENUM_ERROR>(0x12);
            }
            m_member[i].m_fieldc = 1;
            memcpy(m_member[i].m_fielde, ip, std::min(port, 4));
        }
        if (m_member[i].m_fieldc != 1)
        {
            allConnected = false;
        }
    }
    if (allConnected)
    {
        ClearBadP2PUser();
    }
