// _FindTeam ORIG T 0x856663a8 报告 _FindTeam.md
int CFairMatch::_FindTeam(CUser* user)
{
    for (int t = 0; t < 2; ++t) {
        for (int i = 0; i < 4; ++i) {
            if (m_teams[t].m_players[i].m_flag != 0 &&
                m_teams[t].m_players[i].m_user != 0 &&
                m_teams[t].m_players[i].m_user == user)
                return t;
        }
    }
    return 2;
}