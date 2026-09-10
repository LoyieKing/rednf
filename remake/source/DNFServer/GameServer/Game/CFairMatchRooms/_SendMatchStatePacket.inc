void CFairMatch::_SendMatchStatePacket(int state, int teamSpan)
{
    PacketGuard pkt;
    pkt.put_header(0, 0xaf);
    pkt.put_byte(state);
    pkt.finalize(true);
    for (int t = 0; t < teamSpan; ++t) {
        for (int i = 0; i <= 3; ++i) {
            // ORIG +0x68..0x8b：先取 m_user 落槽（无论 flag），后独立重算地址读
            // m_flag（P48 双独立访问），再判 u!=0。
            CUser* u = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && u != 0)
                u->Send(pkt);
        }
    }
