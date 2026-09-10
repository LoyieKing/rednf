void CFairMatch::_ChangeTeamLeader(PvP_Room* room,
                                   ENUM_SCHOOL_MATCH_TEAM teamIdx,
                                   int oldLeaderSeat)
{
    // b28d-solo-CFairMatchRooms：Ghidra C 实证 ORIG 每次访问均重算
    // `*(int*)(this + (teamIdx+0x2c)*4 + 8)`（数组索引形，无引用局部槽；
    // 三目引用形会生 cmpl/jne/add 分支 + -0x14 引用槽）。循环内亦直接
    // 逐字段重算地址（P157/P183 去引用缓存）。
    CUser* newLeader = 0;
    if (m_leaderSeat[teamIdx] == oldLeaderSeat) {
        for (int i = 0; i < 4; ++i) {
            if (m_teams[teamIdx].m_players[i].m_flag != 0) {
                newLeader = m_teams[teamIdx].m_players[i].m_user;
                break;
            }
        }
    }
    if (newLeader != 0) {
        m_leaderSeat[teamIdx] = room->get_user_seat(newLeader);
        PacketGuard pkt;
        pkt.put_header(0, 0x13d);
        pkt.put_short(m_leaderSeat[teamIdx]);
        pkt.finalize(true);
        m_teams[teamIdx].SendToTeam(pkt);
    }
