int CParty::change_manager(CUser* user, int slot)
{
    if (slot < 0 || 3 < slot)
        return 0x15;
    if (m_member[slot].m_pUser == 0)
        return 0x15;
    if (user != m_manager)
        return 8;
    if (m_member[slot].m_pUser == m_manager)
        return 0x12;
    m_manager = m_member[slot].m_pUser;
    set_host();
    send_host_info();
    G_GameWorld()->send_party_info_to_all(this, 2);
    return 0;
