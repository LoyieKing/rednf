// ORIG T 0x0855d1c0
// [b28d-solo-CLeagueMatch] DIFF 142/142·真 bug 已修 + N-STRIDE：ORIG 0x0855d245/
// d273 以**第 3 参** team 判 m_teamA/m_teamB（原码误用第 4 参 flag，调用点
// OnWalkOut/PvP_Room 均传 (user,room,team,seat)）；本波再修两处 P0：
// ①`_ReturnTeam(room, room->get_manager_team(), user)` 内联实参（ORIG
// 0x0855d1fc 返回值 %eax 直压 0x8(%esp)，无 managerTeam 具名槽）；
// ②`int idx = 0;` + `for (idx = 0;…)` 双初始化（ORIG 0x0855d232/d239 两次
// movl $0,-0xc）且删 selfRating/oppoRating 的 0.0f 初始化（ORIG 无该清零，
// P171）；gap -5→0，残差 6 处 stride shl-vs-lea（N-STRIDE；p167 7/7 CLEAR），
// 语义等价。
void CLeagueMatch::OnLeaveRoom(CUser* user, PvP_Room* room, int team, int /*flag*/)
{
    if (m_state == 4) {
        _SendMatchStatePacket(4, 2);
        m_state = 0;
        // [b28d-solo-CLeagueMatch] P0：ORIG 0x0855d1fc..0x0855d219 的
        // get_manager_team() 返回值在 %eax 直压 0x8(%esp)（无具名局部槽往返）
        // ⇒ 内联实参，删 managerTeam 局部。
        _ReturnTeam(room, room->get_manager_team(), user);
    } else if (m_state == 5) {
        // [b28d-solo-CLeagueMatch] P191：ORIG 0x0855d232/d239 两次 `movl $0x0,-0xc`
        // = 声明初始化 + for 初始化双形；P171：ORIG 无 selfRating/oppoRating 的
        // float 0.0f 清零（仅 idx 清零），未命中时由 idx<=3 保护 ⇒ 删初始化式。
        int idx = 0;
        float selfRating;
        float oppoRating;
        for (idx = 0; idx < 4; ++idx) {
            // [b28d-solo-CLeagueMatch] 真 bug 修正：ORIG 0x0855d245/d273 以**第 3 参**
            // `cmpl $0x1,0x14(%ebp)` 判队（m_teamA +0x24 / m_teamB +0x94），原码误用
            // 第 4 参 flag（0x18）——调用点 OnWalkOut/PvP_Room 均传 (user,room,team,seat)。
            if (team == 1) {
                if (m_teamA.m_players[idx].m_user == user) {
                    selfRating = m_teamA.m_players[idx].m_rating;
                    oppoRating = m_teamB.m_players[idx].m_rating;
                    break;
                }
            } else if (m_teamB.m_players[idx].m_user == user) {
                selfRating = m_teamB.m_players[idx].m_rating;
                oppoRating = m_teamA.m_players[idx].m_rating;
                break;
            }
        }
        if (idx <= 3) {
            CEloRating::calc(selfRating, oppoRating,
                             static_cast<ENUM_ELO_RATING_SYSTEM>(0),
                             PvPGlobalEnvironmentVariable::school_match_elo_k_);
            user->UpdateRating(selfRating);
        }
    } else {
        m_state = 0;
    }

    if (team == 1)
        m_teamA.del(user);
    else
        m_teamB.del(user);

    pWaitingRoomList_->DelRoom(this);               // IRoomList vtable+0x04
}