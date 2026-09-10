int CAssaultPlace::_GetTeamCount(ENUM_ASSAULT_TEAM team)
{
    int count = 0;
    for (int i = 0; i < 8; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        if ((int)m_assaulters[i].GetTeam() == (int)team) {
            count = count + 1;
        }
    }
    return count;
