int CParty::create_party(CUser* user)
{
    m_member[0].m_pUser = user;                    // +0x78
    m_member[0].m_field8 = user->GetUID();    // +0x80 ← CUser::GetUID
    m_manager = user;                              // +0x74
    m_field68 = 1;                                 // +0x68
    m_state = 1;                                   // +0x6a
    user->SetPartyIndex((short)m_partyIndex);      // +0x64（cwtl 截断）
    user->resetStraightVictories();
    ((CPartyTelePort*)GetPartyTelePort())->reset_teleport_data();
    return 1;
