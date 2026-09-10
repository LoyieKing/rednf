int CParty::set_single_play(CUser* user)
{
    m_member[0].m_pUser = user;                    // +0x78
    m_member[0].m_field8 = user->GetUID();    // +0x80
    m_manager = user;                              // +0x74
    m_singlePlay = 1;                              // +0x69
    m_field68 = 1;                                 // +0x68
    m_autoCreated = 1;                             // +0x13c
    m_state = 1;                                   // +0x6a
    user->SetPartyIndex((short)m_partyIndex);
    ((CPartyTelePort*)GetPartyTelePort())->reset_teleport_data();
    return 1;
