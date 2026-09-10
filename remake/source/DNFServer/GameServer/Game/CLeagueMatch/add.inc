int TeamInfo::add(CUser* user)   // ORIG T 0x0855cb54
{
    // [G9WorldTimerLeague 2026-09-10 复核] TeamInfo 族（add/del/refresh/reset/size，
    // 统一 stride 0x1c）成员 disp 扫描：唯一 OURS-only {0x10:1} 为 m_user 成员
    // 偏移 `lea 0x10(%edx),%eax` 的两步形，ORIG 0x0855cb6e 段以
    // `lea 0x0(,%eax,4),%edx + lea 0x0(,%edx,8),%eax + sub %edx,%eax` 一步到位
    // ⇒ N125（i*4 恒 shl $2，7 索引类型 + 双后端实测全败）+ N4/N107 族，
    // 无语义缺口；p167 CLEAR（4/4）。
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
