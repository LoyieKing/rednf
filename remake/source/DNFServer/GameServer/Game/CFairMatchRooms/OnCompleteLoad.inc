void CFairMatch::OnCompleteLoad(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(0, 0x116);
    pkt.put_short(static_cast<unsigned short>(user->get_unique_id() & 0xffff));
    pkt.finalize(true);
    for (int t = 0; t < 2; ++t) {
        for (int i = 0; i <= 3; ++i) {
            // ORIG 循环体：先独立取 m_user 落槽，再重算地址读 m_flag（P48）。
            CUser* u = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && u != 0)
                u->Send(pkt);
        }
    }
