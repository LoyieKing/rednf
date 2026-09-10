int CParty::change_manager_for_quick_party()
{
    for (int i = 0; i <= 3; i++)
    {
        if (m_member[i].m_pUser == m_manager)
        {
            m_manager->SetPartyIndex(-1);
            m_member[i].m_pUser = 0;
            m_member[i].m_field8 = 0;
            break;
        }
    }
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            m_manager = m_member[i].m_pUser;
            set_host();
            send_host_info();
            return 1;
        }
    }
    LogManager::logFormat(
        1, "party.cpp", __PRETTY_FUNCTION__, 0x45d,
        "there is no manager(party empty)");
    return 0;
