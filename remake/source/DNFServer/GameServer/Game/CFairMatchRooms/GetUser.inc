CUser* CFairMatch::GetUser(unsigned int idx)
{
    if (idx > 3u) return 0;
    if (!m_teams[0].m_players[idx].m_flag) return 0;
    return m_teams[0].m_players[idx].m_user;
