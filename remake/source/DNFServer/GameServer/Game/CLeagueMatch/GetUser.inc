CUser* CLeagueMatch::GetUser(unsigned int idx)   // ORIG T 0x0855e5e4
{
    if (idx > 3)
        return 0;
    // b28d-lane6：m_flag bool 化 + 裸 `if(!field)` ⇒ ORIG 0x0855e5e9 的
    // `movzbl 0x18; xor $1; test %al,%al; je`（P248 档②，探针 /tmp/l6p/p6.cpp f2）。
    if (!m_teamA.m_players[idx].m_flag)
        return 0;
    return m_teamA.m_players[idx].m_user;
