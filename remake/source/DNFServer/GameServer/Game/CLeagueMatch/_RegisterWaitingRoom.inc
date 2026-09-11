// [b28d-solo-CLeagueMatch] DIFF 132/132·ndiff 24·N-STRIDE+N7（S217 新鲜 .o
// 复验；x87 比较段已产 ORIG 的 `fucompp;fnstsw;sahf/test;sete` 形，S180 判词
// 里的 fucomip/seta 系 stale 链接二进制假象）：foundMin 改
// int（ORIG -0x20 `cmpl $0`）+ 两循环改嵌套正向 if（P296）后条数 132=132；
// S180 再把 sort 双实参改 P235 视角 m_teams[0]/m_teams[1]，复现 ORIG
// 0x0855da16 的 `add $0x8c` / `add $0x1c` 双独立算址；残差仅剩 stride
// shl-vs-lea 与 x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 seta 形），
// 语义等价。
void CLeagueMatch::_RegisterWaitingRoom(PvP_Room* room)   // ORIG T 0x0855d884
{
    // [b28d-solo-CLeagueMatch] ORIG foundMin 为 int（-0x20 `cmpl $0x0,-0x20`）非 bool；
    // 两循环均嵌套正向 if（P296）：m_flag 正向 `test;je`（0x0855d8c7）、
    // get_team==3 正向（0x0855d98a），消 continue 的 jmp+nop 中继岛。
    int foundMin = 0;
    float minRating = 0.0f;
    for (int i = 0; i < 4; ++i) {
        if (m_teamA.m_players[i].m_flag != 0) {
            if (foundMin == 0) {
                minRating = m_teamA.m_players[i].m_rating;
                foundMin = 1;
            }
            if (m_teamA.m_players[i].m_rating < minRating)   // asm fucompp：小于最小值则更新
                minRating = m_teamA.m_players[i].m_rating;
        }
    }
    minRating -= 1.0f;

    for (int seat = 0; seat <= 7; ++seat) {
        if (room->get_team(seat) == 3) {
            CUser* seatUser = reinterpret_cast<CUser*>(room->get_user_seat(seat));
            for (int i = 0; i < 4; ++i) {
                if (m_teamA.m_players[i].m_user == seatUser && seatUser != 0)
                    m_teamA.m_players[i].m_rating = minRating;
            }
        }
    }

    std::sort<PlayerInfo*, bool (*)(PlayerInfo const&, PlayerInfo const&)>(
        m_teams[0].m_players, m_teams[1].m_players, CompareRating);  // ORIG weak sort 实例
        // [b28d-solo-CLeagueMatch] P235：ORIG 0x0855da16 两实参各自独立算址
        // （this+0x1c / this+0x8c）；写 m_teamA.m_players+4 会让 4.4 先算基址再
        // lea 0x70 ⇒ 用等宽数组视角 m_teams[0]/m_teams[1] 复现 ORIG 双独立地址。
    pWaitingRoomList_->AddRoom(this);                               // IRoomList vtable+0x00
}