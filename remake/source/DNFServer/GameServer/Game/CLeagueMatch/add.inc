int TeamInfo::add(CUser* user)   // ORIG T 0x0855cb54
{
    // [b28d-solo-CLeagueMatch] P292/P248 档②：ORIG 0x0855cb83 `movzbl; xor $1; test; je`
    // = bool 字段 + 裸 `if (!m_flag)` 正向包裹（P296）；continue 形反产 xor+jne 岛。
    for (int i = 0; i <= 3; ++i) {
        if (!m_players[i].m_flag) {
            m_players[i].m_flag = 1;
            m_players[i].m_user = user;
            m_players[i].m_accId = user->get_acc_id();
            m_players[i].m_rating = user->GetRating();
            m_players[i].m_schoolNo = user->GetSchoolNo();
            user->GetDailyBadge(m_players[i].m_badges);
            return 1;
        }
    }
    return 0;
