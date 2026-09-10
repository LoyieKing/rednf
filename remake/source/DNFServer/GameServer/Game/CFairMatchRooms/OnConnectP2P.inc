void CFairMatch::OnConnectP2P(CUser* user, PvP_Room* room,
                              char* addr, int port)
{
    (void)user;
    if (m_field04 != 4) {
        _SendMatchStatePacket(4, 2);
        m_field04 = 0;
        _ReturnTeam(room, room->get_manager_team(), 0);
        return;
    }

    int i;
    for (i = 0; (i < 8) && (i < port); ++i) {
        if (addr[i] == '\0') {
            // 未全部连上：发失败态并直接走 +0x40 兜底虚槽（OnEndFight）
            _SendMatchStatePacket(4, 2);
            OnEndFight(room);
            return;
        }
    }

    ++m_fieldAC;
    if (m_fieldAC == static_cast<int>((unsigned char)GetPlayCountPerOneTeam() * 2)) {
        PacketGuard pkt;
        pkt.put_header(0, 0x117);
        pkt.finalize(true);
        for (int t = 0; t < 2; ++t) {
            for (int p = 0; p < 4; ++p) {
                CUser* u = m_teams[t].m_players[p].m_user;
                if (m_teams[t].m_players[p].m_flag != 0 && u != 0)
                    u->Send(pkt);
            }
        }
        pkt.clear();
        room->battle_reset();
        for (int seat = 0; seat <= 7; ++seat) {
            if (room->get_user_seat(seat) != 0) {
                room->m_relayMgr.OnReady(seat, true);
            }
        }
        room->start_pvp();
        room->make_state_info(reinterpret_cast<char*>(&pkt));
        G_GameWorld()->send_all(pkt);
    }
