// [b28d-solo-CLeagueMatch] DIFF 39/39·N-STRIDE：残差全为 stride 0x1c 的
// shl-vs-lea（3 访问点），语义等价，结构性豁免。
int TeamInfo::del(CUser* user)   // ORIG T 0x0855cc9c
{
    for (int i = 0; i <= 3; ++i) {
        if (m_players[i].m_flag != 0 && m_players[i].m_user == user) {
            m_players[i].m_flag = 0;
            return 1;
        }
    }
    return 0;
}