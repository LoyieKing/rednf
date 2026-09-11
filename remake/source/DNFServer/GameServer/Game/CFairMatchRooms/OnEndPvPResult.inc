// OnEndPvPResult ORIG T 0x8566694c 报告 OnEndPvPResult.md
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 链式 + P0 内联
// `_ReturnTeam(room, _GetOppositeTeam(teamOf), 0)`（去 oppo 具名槽使全帧槽号回正）
// + P221 链式 trace；call 差集 29=29。fresh ndiff=0（267=267）。
void CFairMatch::OnEndPvPResult(PvP_Room* room, CUser* user)
{
    const int seat = room->get_user_seat(user);
    if (seat == -1) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x10b3, "error get_user_seat");
        return;
    }
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
    CSwitchLog(
        "virtual void fair_pvp::CFairMatch::OnEndPvPResult(PvP_Room*, CUser*)",
        0x10b8, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                      user->get_acc_name(), user->getCurCharacName(),
                      room->get_index(), room->get_pvp_battle_mode(),
                      room->get_waiter_count());
    const int teamOf = room->get_team(seat);
    if (teamOf == 0xfe) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x10be, "error get_team");
        return;
    }
    const int myTeam = _FindTeam(user);
    if (myTeam == 2) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x10c5, "error _FindTeam");
        return;
    }
    if (m_teams[myTeam].m_field04 == 2)
        return;                                   // 双方均已表决

    const ENUM_SCHOOL_MATCH_TEAM other =
        _GetOtherTeam(static_cast<ENUM_SCHOOL_MATCH_TEAM>(myTeam));
    if (other == static_cast<ENUM_SCHOOL_MATCH_TEAM>(2)) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x10d2, "error _GetOtherTeam");
        return;
    }

    room->set_state(1);   // ORIG PvP_Room::set_state(room,1)
    for (int i = 0; i < 4; ++i) {
        // b28d-solo-CFairMatchRooms：P157/P183 去引用缓存（ORIG 每处字段访问
        // 重算地址）。
        if (m_teams[myTeam].m_players[i].m_flag != 0 &&
            m_teams[myTeam].m_players[i].m_user != 0)
            m_teams[myTeam].m_players[i].m_user->set_state(ch_state6);
    }
    if (m_teams[myTeam].m_field04 == 1) {
        // 已请求返回：把对方队伍整体搬回自建房并广播离场包
        // P0：ORIG 把 _GetOppositeTeam(teamOf) 内联为 _ReturnTeam 第 2 实参
        // （无 oppo 具名槽）。
        PvP_Room* kept = _ReturnTeam(room, _GetOppositeTeam(teamOf), 0);
        if (kept != 0) {
            PacketGuard pkt;
            pkt.put_header(0, 0x13b);
            pkt.finalize(true);
            kept->send_to_pvp(pkt);
        }
    } else if (m_teams[myTeam].m_field04 == 0) {
        _PullOutRoomTeam(room,
                         static_cast<ENUM_SCHOOL_MATCH_TEAM>(myTeam));
    }
    room->set_state(2);
    if (teamOf == 1) {
        std::swap(m_teams[0], m_teams[1]);        // ORIG std::swap<TeamInfoThird>
    }
}