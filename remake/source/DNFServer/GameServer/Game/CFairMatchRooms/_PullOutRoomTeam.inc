// _PullOutRoomTeam ORIG T 0x85665d86?（README 0x08565d86）报告同名 md
// b28d-solo-CFairMatchRooms 判词：N121——P227 链式 + P225/P295（while+break 复现
// `cmp $2; sete; test; je`）后仅缺 ORIG body 尾 `jmp <紧邻下一指令>`（1 行）：
// 4.4 恒 jump-thread 掉该空岛（N121 epilogue-threading 尾岛不可消）⇒ 封顶。
// fresh ndiff=1（157/156）。call 差集 10=10 CLEAR。
// [b28d-PoolSmall 复验] 再补两形实测均不闭合：①`break`→`goto <体尾空标签>`
// （标签置于 while 之后）⇒ 4.4 仍把 goto 融为 fall-through，`jmp` 不发射
// （l8_cmp 严格口径 ndiff 2，其中 1 行为 rodata 标签伪差）；②UpdateScore 侧
// P212「全头部声明 + 块内仅赋值」把 killCnt 提出块外 ⇒ 深槽号零改善、
// 本函数 ndiff 6→8 反劣（已回滚）。⇒ P223 定案③「4.4 岛数 = 条件数或 0」
// 成立，N121 封顶维持。
// [GW20Run 2026-09-11 复核] fresh 158/157；ORIG-only 1 条 = 0x08565f73 冗余
// `jmp 08565f76`（落点=下一指令；4.1 `break` 产物），4.4 恒 jump-thread 合并；
// p167 10=10 别名、enc_kr_scan 0 ⇒ 维持 N121。
bool CFairMatch::_PullOutRoomTeam(PvP_Room* room, ENUM_SCHOOL_MATCH_TEAM teamIdx)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链（_PullOutRoom 实证同族）。
    CSwitchLog(
        __PRETTY_FUNCTION__,   // [Q2P342ScanB] 原手写串 → prett（ORIG size 0x4f）
        0xfa7, 0, 0)("pvp@log room(%d),%d,%d",
                     room->get_index(), room->get_pvp_battle_mode(),
                     static_cast<int>(teamIdx));
    int lastTeam = 0;
    for (int i = 0; i < 4; ++i) {
        // b28d-solo-CFairMatchRooms：P157/P183 去引用缓存（ORIG 每处字段访问
        // 重算完整地址，无引用槽）。
        if (m_teams[teamIdx].m_players[i].m_flag != 0 &&
            m_teams[teamIdx].m_players[i].m_user != 0) {
            const int seat =
                room->get_user_seat(m_teams[teamIdx].m_players[i].m_user);
            if (seat != -1) {
                lastTeam = room->get_team(seat);
                _PullOutRoom(m_teams[teamIdx].m_players[i].m_user, room, seat);
            }
        }
    }
    if (lastTeam == 1) {
        for (int seat = 0; seat <= 7; ++seat) {
            CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(seat));
            // b28d-solo-CFairMatchRooms：P225/P295 —— ORIG `cmp $2; sete %al;
            // test %al,%al; je`（bool 物化）；`&&` 短路形被 4.4 折成 jne 直跳，
            // 改 while+break 复现物化。
            if (u != 0) {
                while (room->get_team(seat) == 2) {
                    room->OnSetPVPSeatState(u, seat,
                                            static_cast<ENUM_SEAT_STATE>(1),
                                            static_cast<ENUM_WALKOUT_TYPE>(2));
                    // [b28f-SweepCFairMatch 负结论] ORIG 0x08565f73 在 call 后有
                    // `jmp +0x1f0`（落点=循环增量块 ⇒ 语义上像 continue）。改
                    // `continue` 实测反劣：4.4 把 while 条件下沉/回边重排，归一化
                    // 差异 14→38 行且 `jmp +0x1f0` 仍未发射 ⇒ 恢复 `break`；
                    // 该条为 4.1 `break` 冗余 jmp（落点=下一指令），4.4 恒
                    // jump-thread（N121 族），封顶。
                    break;
                }
            }
        }
    }
    return true;
}