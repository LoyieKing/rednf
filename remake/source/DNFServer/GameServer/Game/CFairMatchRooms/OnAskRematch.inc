// OnAskRematch ORIG T 0x856664a0 报告 OnAskRematch.md
// param：0=拒绝 1=接受 2=全队同意提交；队决定写入 m_field04(team)
// b28d-solo-CFairMatchRooms 判词：DIFF（ndiff 244→36，322→321 指令；room/myTeam/
// other/trace/pkt 槽全部回正 -0x24/-0x20/-0x1c/-0x34/-0x40）。已收：删具名 bool
// reject 改 P226 语句表达式 `if (({ !(A && B && C); }))`（ORIG 0x08566795/0x0856679c
// 无槽 `mov $1/mov $0` 岛 + test/je；具名 bool 曾多占 -0x19 槽把全帧推深 4）；if
// 链→`switch (decision)`（P251/P286，复现 ORIG `mov 0x10(%ebp),%eax; test; js;
// cmp $1; jle; cmp $2; je; jmp` 形参载入 + 寄存器比较链）；P227 链式。残 = 伪差 +
// 1 处 `m_teams[myTeam].m_field04=decision` 的 `add $0x20` vs `lea 0x20`（N107 族）；
// call 差集 41=41 CLEAR。
// [GW20Run 2026-09-11 复核] fresh 321/321；唯一实质差 = ORIG 0x085666b0
// `add $0x20,%eax` vs 我方 0x…`lea 0x20(%eax),%edx`（N107 反向族，P211 档③已试零效）
// + 尾 nop 豁免；p167 41=41 别名 ⇒ 维持。
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
        // [b28f-SweepCFairMatch 负结论] ORIG 值先载(%ecx)→算址(add $0x20)→store；
        // 4.4 恒算址(lea 0x20(%eax),%edx)→值后载→store。试 RHS enum 异型化
        // （P211 档③）实测零效（归一化差异 69 行不变）⇒ N107 反向族封顶，
        // 已恢复直证形态。
        // [G18 T11 2026-09-11 · P-g03-b 负结论] 改 `*(int*)((char*)&m_teams[myTeam] + 0x20)`
        // 实测反劣：our 321→323，多 `mov this,%eax; add $0x1c,%eax; mov %eax,%ecx`
        // 三段（数组基址常量 0x1c 被 4.4 提前 add，P-g03-b 的 add 只对**末段 K**
        // 生效）⇒ 已回滚（备份 /tmp/lane_diff/g18/backup/OnAskRematch.inc）。
        m_teams[myTeam].m_field04 = decision;     // asm 写 4 字节到 team+4
        // [b28f-SweepCFairMatch 试形] 取址解引用 → 具名 bool 成员直访问
        // （J1 实证：成员直访产 disp 折叠，取址链产 add）。
        if (m_teams[other].m_field00) {
            m_teams[other].SendToTeam(pkt);
            room->setReamtchable(false);
        }
        room->revengeMission(false);
        break;
    case 2:                                   // 全队满编 → 提交重赛
        m_teams[myTeam].m_field00 = true;     // 队头字节置可重赛标记（具名 bool）
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
        } else if (m_teams[other].m_field00) {
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
}