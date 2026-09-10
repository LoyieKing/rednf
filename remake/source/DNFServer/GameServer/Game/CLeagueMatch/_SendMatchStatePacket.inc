void CLeagueMatch::_SendMatchStatePacket(int state, int teams)   // ORIG T 0x0855e664
{
    PacketGuard pkt;
    pkt.put_header(0, 0xaf);
    pkt.put_byte(state);
    pkt.finalize(true);
    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 无 TeamInfo& 引用槽（t/i/user 三局部，
    // user 具名缓存 -0xc，寻址 this+i*0x1c+t*0x70+0x24）⇒ 数组化访问复现。
    for (int t = 0; t < teams; ++t) {
        for (int i = 0; i <= 3; ++i) {
            CUser* member = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && member != 0)
                member->Send(pkt);
        }
    }
