void CAssaultPlace::_GetTeamCount(int* counts, ENUM_ASSAULT_STATE state)
{
    for (int i = 0; i < 8; ++i) {
        // b28 BL6: f3 岛形（P164 族）——Empty continue→nop 岛，正向 == 包 ++，
        // 体尾 4.4 保留前向 jmp 统一尾（ORIG 0x82e7288 jne/je/jmp+nop 全等）。
        if (m_assaulters[i].Empty())
            continue;
        if ((int)m_assaulters[i].GetState() == (int)state) {
            ++counts[(int)m_assaulters[i].GetTeam()];
        }
    }
