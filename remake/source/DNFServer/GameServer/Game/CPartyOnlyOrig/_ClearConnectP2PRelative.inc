int CParty::_ClearConnectP2PRelative(int slot)
{
    for (int i = 2; i < 4; i++)
    {
        if (_GetConnectP2PPoint(i) == slot)
        {
            leave_user(m_member[i].m_pUser,
                       static_cast<ENUM_PARTY_INFO_TYPE>(2));
        }
    }
    return 1;
