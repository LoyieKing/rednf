// ORIG 反汇编实证：teamNo==1 分支 GetTeamRating 用 this+0x64（m_teams[1]），
// 队员循环读 this+0x24+i*0x10（m_teams[0]）；else 分支相反（0x1c / 0x6c）。
// BLOCKED 局部依赖：CEloRating::calc 本体与 PvPGlobalEnvironmentVariable::
// school_match_elo_k_ 定义尚未由任何 TU 提供（仅 ORIG 符号）。
// b28d-solo-CFairMatchRooms 判词：AE——去 PlayerInfoThird 引用缓存 + memberRating/
// oppositeRating 声明序翻转，逐条全等；仅剩 elo_k 全局重定位伪差。
// call 差集 6=6 CLEAR。
void CFairMatch::_UpdateRating(int teamNo, ENUM_ELO_RATING_SYSTEM system)
{
    // b28d-solo-CFairMatchRooms：ORIG 槽序 memberRating@-0x18（浅）/
    // oppositeRating@-0x1c（深）⇒ 4.4 按首次使用序，声明序须 memberRating 在前。
    float memberRating = 0.0f;
    float oppositeRating = 0.0f;
    CUser* member = 0;
    if (teamNo == 1) {
        oppositeRating = static_cast<float>(m_teams[1].GetTeamRating());
        for (int i = 0; i < 4; ++i) {
            // b28d-solo-CFairMatchRooms：P157/P183 去引用缓存——ORIG 每处字段
            // 访问都重算完整地址（mov i; mov this; add $1; shl $4; add; add $0x14），
            // 无 -0x14 指针槽。
            if (m_teams[0].m_players[i].m_flag != 0 &&
                m_teams[0].m_players[i].m_user != 0) {
                member = m_teams[0].m_players[i].m_user;
                memberRating = m_teams[0].m_players[i].m_rating;
                CEloRating::calc(memberRating, oppositeRating, system,
                                 PvPGlobalEnvironmentVariable::school_match_elo_k_);
                member->UpdateRating(memberRating);
            }
        }
    } else {
        oppositeRating = static_cast<float>(m_teams[0].GetTeamRating());
        for (int i = 0; i < 4; ++i) {
            if (m_teams[1].m_players[i].m_flag != 0 &&
                m_teams[1].m_players[i].m_user != 0) {
                member = m_teams[1].m_players[i].m_user;
                memberRating = m_teams[1].m_players[i].m_rating;
                CEloRating::calc(memberRating, oppositeRating, system,
                                 PvPGlobalEnvironmentVariable::school_match_elo_k_);
                member->UpdateRating(memberRating);
            }
        }
    }
}