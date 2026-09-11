// [b28d-solo-CLeagueMatch] DIFF 72/72·N-STRIDE：三守卫已按 P296 改嵌套正向 if，
// 与 ORIG 0x0855e1a3/a9/af 同落 +0xcb（消 3 个 jmp+nop 岛）；残差全为 stride 的
// shl-vs-lea，语义等价。
float CLeagueMatch::GetGap(IMatch* other)     // ORIG T 0x0855e12a
{
    float gap = 0.0f;
    for (unsigned int i = 0; i < (unsigned int)nPlayerCountPerOneTeam_; ++i) {
        CUser* mine = m_teamA.m_players[i].m_user;
        CUser* theirs = other->GetUser(i);
        // [b28d-solo-CLeagueMatch] P296：ORIG 三守卫各 `je` 直跳循环尾（无 continue
        // 的 jmp+nop 中继岛，0x0855e1a3/a9/af 同落 +0xcb）⇒ 嵌套正向 if 复现。
        if (m_teamA.m_players[i].m_flag != 0) {
            if (mine != 0) {
                if (theirs != 0) {
                    float a = mine->GetRating() * g_RankingIncr[i];
                    float b = theirs->GetRating() * g_RankingIncr[i];
                    gap += __builtin_fabsf(a - b);
                }
            }
        }
    }
    return gap;
}