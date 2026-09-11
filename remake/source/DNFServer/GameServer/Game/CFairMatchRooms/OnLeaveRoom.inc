// OnLeaveRoom ORIG T 0x8562fbc 报告 OnLeaveRoom.md
// [b28 BM2 判词·N7] AE-ndiff=6（fresh .o 归一，全二进制口径 16 含标签伪差）：
// 唯一差 = _ReturnTeam 类虚槽调用 call *%ecx(ORIG) vs call *%eax(4.4) 及其
// 实参装载寄存器对调（N7 族，b26 NL28）。其余 181  insn 逐条同。
void CFairMatch::OnLeaveRoom(CUser* user, PvP_Room* room,
                             int teamNo, int flag)
{
    if (m_field04 == 4) {
        _SendMatchStatePacket(4, 2);
        m_field04 = 0;
        _PullOutRoomTeam(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(1));
    } else if (m_field04 == 5) {
        if (room->GetTeamCount(teamNo) == 1)
            _UpdateRating(teamNo, static_cast<ENUM_ELO_RATING_SYSTEM>(0));
        const int myTeam = _FindTeam(user);
        if (myTeam == 2) {
            LogManager::logFormat(
                1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__,
                0xb0b, "error _FindTeam");
            return;
        }
        const ENUM_SCHOOL_MATCH_TEAM other =
            _GetOtherTeam(static_cast<ENUM_SCHOOL_MATCH_TEAM>(myTeam));
        if (other == static_cast<ENUM_SCHOOL_MATCH_TEAM>(2)) {
            LogManager::logFormat(
                1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__,
                0xb12, "error _GetOtherTeam");
            return;
        }
        // ORIG +0x44..：(this+other*0x48)+0x10/disp0xc = m_field00（P1 组件视图等价具名字段）
        if (m_teams[other].m_field00) {
            PacketGuard pkt;
            pkt.put_header(0, 0x13a);
            pkt.finalize(true);
            m_teams[other].SendToTeam(pkt);
            m_teams[other].m_field04 = 1;
        }
    } else {
        m_field04 = 0;
    }

    if (teamNo == 1) {
        m_teams[0].del(user);
        _ChangeTeamLeader(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(0), flag);
    } else {
        m_teams[1].del(user);
        _ChangeTeamLeader(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(1), flag);
    }
    // ORIG 无判空直调（0x08563cc6 同族形态，P148 忠实模拟）。
    pWaitingRoomList_->DelRoom(this);
    _SendMatchStatePacket(5, 1);
}