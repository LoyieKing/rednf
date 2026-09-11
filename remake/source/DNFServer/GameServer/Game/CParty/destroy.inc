void CParty::destroy()
{
    if (m_autoCreated)
    {
        PacketGuard packet;
        packet.put_header(0, 9);
        packet.put_short(1);
        packet.put_short(GetPartyIndex());
        packet.put_byte(3);
        packet.finalize(true);
        m_manager->SendPacket((CUser::eSendTarget)1, packet);
    }
    else
    {
        G_GameWorld()->send_party_info_to_all(this, 3);
    }
    init();
    G_CGameManager()->PutParty(this);
    m_usedCoinCount = 0;
}