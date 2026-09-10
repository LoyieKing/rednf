void CLeagueMatch::OnCompleteLoad(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(0, 0x81);
    pkt.put_short(user->get_unique_id() & 0xffff);
    pkt.finalize(true);
    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 无 TeamInfo&/PlayerInfo& 引用缓存槽
    // （Ghidra 仅 t/i/user 三局部）；以 (&m_teamA)[t] 数组化寻址 + user 具名局部
    // （ORIG -0x10）复现，并消 4.4 的引用槽。
    for (int t = 0; t <= 1; ++t) {
        for (int i = 0; i < 4; ++i) {
            CUser* member = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && member != 0)
                member->Send(pkt);
        }
    }
