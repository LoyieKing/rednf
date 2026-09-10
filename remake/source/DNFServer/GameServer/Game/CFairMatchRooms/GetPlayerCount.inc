int CFairMatch::GetPlayerCount()
{
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        if (m_teams[0].m_players[i].m_flag != 0) ++count;
        if (m_teams[1].m_players[i].m_flag != 0) ++count;
    }
    return count;
