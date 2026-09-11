// ORIG T 0x82e71c4 _GetTeamCountAliveUser(int*)
void CAssaultPlace::_GetTeamCountAliveUser(int* counts)
{
    for (int i = 0; i <= 7; ++i) {
        CUser* u = m_assaulters[i].GetUser();
        if (u == 0)
            continue;
        if (u->IsCurCharacGhost())
            continue;
        counts[m_assaulters[i].GetTeam()]++;
    }
}