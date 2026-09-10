void CFairMatch::OnAskRematch(CUser* user, int decision)
{
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x1058, "error GetPVPRoom");
        return;
    }
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
    CSwitchLog(
        "virtual void fair_pvp::CFairMatch::OnAskRematch(CUser*, int)",
        0x105c, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                      user->get_acc_name(), user->getCurCharacName(),
                      room->get_index(), room->get_pvp_battle_mode(), decision);

    const int myTeam = _FindTeam(user);
    if (myTeam == 2) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x1062, "error _FindTeam");
        return;
    }
    const ENUM_SCHOOL_MATCH_TEAM other =
        _GetOtherTeam(static_cast<ENUM_SCHOOL_MATCH_TEAM>(myTeam));
    if (other == static_cast<ENUM_SCHOOL_MATCH_TEAM>(2)) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x1069, "error _GetOtherTeam");
        return;
    }

    PacketGuard pkt;
    // ORIG 0x085667a5 段：`mov 0x10(%ebp),%eax; test; js; cmp $1; jle;
    // cmp $2; je; jmp` = 形参载入 + 寄存器比较决策链 ⇒ switch（P251/P286；
    // if 链在 4.4 恒折内存直比 cmpl $imm,0x10(%ebp)）。
    switch (decision) {
    case 0:                                   // 拒绝 / 单人接受
    case 1:
        pkt.clear();
        pkt.put_header(0, 0x13a);
        pkt.finalize(true);
        m_teams[myTeam].SendToTeam(pkt);
        m_teams[myTeam].m_field04 = decision;     // asm 写 4 字节到 team+4
        if (FM_TeamByte0(m_teams[other]) != 0) {
            m_teams[other].SendToTeam(pkt);
            room->setReamtchable(false);
        }
        room->revengeMission(false);
        break;
    case 2:                                   // 全队满编 → 提交重赛
        *reinterpret_cast<unsigned char*>(&m_teams[myTeam]) = 1;   // 队头字节置可重赛标记
        // ORIG 0x08566795/0x0856679c：reject 为**无槽**寄存器物化（mov $1/mov $0
        // 岛 + test/je），非具名 bool 局部 ⇒ P226 语句表达式复现（禁具名变量）。
        if (({ !(m_teams[other].m_field04 == 2 &&
                 m_teams[other].size() != 0 &&
                 _CheckRematchable(room) == 1); })) {
            pkt.clear();
            pkt.put_header(0, 0x13a);
            pkt.finalize(true);
            m_teams[myTeam].SendToTeam(pkt);
            m_teams[myTeam].m_field04 = 1;
        } else if (FM_TeamByte0(m_teams[other]) != 0) {
            pkt.clear();
            pkt.put_header(0, 0x13a);
            pkt.finalize(true);
            room->send_to_pvp(pkt);
            room->revengeMission(true);
        }
        break;
    }

    // 全房广播本玩家投票（msg 0x137）
    pkt.clear();
    pkt.put_header(0, 0x137);
    pkt.put_byte(decision);
    pkt.put_short(room->get_user_seat(user));
    pkt.finalize(true);
    room->send_to_pvp(pkt);
