// _ReturnTeam ORIG T 0x85664dea?（README 0x08564dea）报告同名 md
// 把 room 内指定队成员退回各自新开房；excludedUser 不参与搬运
// b28d-solo-CFairMatchRooms 判词：N7——P227 链式 + P196（seat unsigned 复现
// setbe）后仅剩 CSwitchLog varargs 的 esi↔edi 分配互换（ORIG 第1值→esi/第2→edi，
// 4.4 反之）+ 对应压栈序（4 行 NEAR，113=113）。跨版本 callee-saved 分配不可择
// （P75/N82附）⇒ 封顶。call 差集 11=11 CLEAR。
// [GW20Run 2026-09-11 复核] fresh 116/116；ORIG 0x08564dfa 段 %esi=waiterCount /
// %edi=pvp_battle_mode，我方两 callee-saved 角色互换（CLog 实参序随之）⇒ 跨版本
// 分配差；p167 11=11 别名 ⇒ 维持。
PvP_Room* CFairMatch::_ReturnTeam(PvP_Room* room, int teamNo, CUser* excluded)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链（_PullOutRoom 实证同族）。
    CSwitchLog(
        __PRETTY_FUNCTION__,   // [Q2P342ScanB] 原手写串 → prett（ORIG size 0x44）
        0xd95, 0, 0)("pvp@log room(%d), %d, %d",
                     room->get_index(), room->get_pvp_battle_mode(),
                     room->get_waiter_count());
    CUser* member = 0;
    PvP_Room* newRoom = 0;
    // b28d-solo-CFairMatchRooms：P196 —— ORIG 循环尾测为 setbe（无符号），
    // 源码 seat 须为 unsigned（int 形 4.4 恒 setle）。
    for (unsigned int seat = 0; seat <= 7; ++seat) {
        member = reinterpret_cast<CUser*>(room->get_user_seat(seat));
        if (member == 0) continue;
        const int teamOf = room->get_team(seat);
        if (teamNo == teamOf) {
            if (teamNo == 2) {   // 观战队：直接按 walkoutType=2 清座
                room->OnSetPVPSeatState(
                    member, seat,
                    static_cast<ENUM_SEAT_STATE>(1),
                    static_cast<ENUM_WALKOUT_TYPE>(2));
            }
            continue;   // asm: 命中本队后不搬
        }
        if (excluded != 0 && member == excluded)
            continue;

        _PullOutRoom(member, room, seat);
        if (newRoom == 0) {
            newRoom = _MakeRoom(member);
        } else {
            _PutInRoom(member, newRoom, true);   // asm 第3参恒 1
        }
    }
    m_field04 = 0;
    return newRoom;
}