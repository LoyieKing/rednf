// [b28d-solo-CLeagueMatch] DIFF 100/100·N-STRIDE：同 TeamInfo 族 stride 差异
// （ORIG lea 链 vs 4.4 shl 链，逐访问点 4 条对 4 条），语义等价，结构性豁免。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] 本轮两旋钮：①具名 `CUser* u` 缓存
// m_players[i].m_user（P0 反向）⇒ our 100→81、ORIG-only/OURS-only 43→64 劣（ORIG
// 每访问点重算 m_user，无缓存槽），已回滚；②循环上界 `i <= 3` → `i < 4` ⇒ 4.4 折叠
// 同形（43/43 不变），已回滚。ORIG 0x0855c9c2..0x0855caf7 的 9 处 `lea 4/lea 8/sub`
// 链 = N125 结构性（证据见 OnCompleteLoad 探针网络）【不可翻】。p167 4/4 CLEAR。
void TeamInfo::refresh()    // ORIG T 0x0855c9ae
{
    for (int i = 0; i <= 3; ++i) {
        if (m_players[i].m_flag != 0) {
            m_players[i].m_accId = m_players[i].m_user->get_acc_id();
            m_players[i].m_rating = m_players[i].m_user->GetRating();
            m_players[i].m_schoolNo = m_players[i].m_user->GetSchoolNo();
            m_players[i].m_user->GetDailyBadge(m_players[i].m_badges);
        }
    }
}