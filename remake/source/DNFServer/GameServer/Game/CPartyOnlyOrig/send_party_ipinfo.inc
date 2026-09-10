void CParty::send_party_ipinfo()
{
    PacketGuard packet;
    packet.put_header(0, 0xb);
    packet.put_byte(get_member_count());
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i))
        {
            packet.put_short(static_cast<unsigned short>(
                m_member[i].m_pUser->get_unique_id()));
            packet.put_int(m_member[i].m_pUser->m_network.get_inner_ip());
            packet.put_int(m_member[i].m_pUser->m_network.get_ip());
            packet.put_short(static_cast<unsigned short>(
                m_member[i].m_pUser->m_network.get_port()));
            packet.put_int(m_member[i].m_pUser->get_acc_id());
            packet.put_byte(m_member[i].m_pUser->m_network.get_nat_type());
            packet.put_int(m_member[i].m_pUser->m_network.get_mtu());
            if (reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                    ->getCurCharacR() != 0)
            {
                packet.put_byte(
                    reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                        ->getCurCharacR()->m_stamina);
            }
            else
            {
                packet.put_byte(0);
            }
        }
    }
    packet.finalize(true);
    send_to_party(packet);
