void CAssaultPlace::_GetTeamCount(int* counts)
{
    // ORIG T 0x82e7232：continue 形（call Empty 后 test al;jne，无 xor）；
    // GetTeam 结果 %eax 直通索引（技巧28）+ 后缀 ++（技巧4 单装载）。
    for (int i = 0; i <= 7; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        // N96 清绑：GetTeam 结果作下标表达式直传（P0 表达式直读）。
        counts[m_assaulters[i].GetTeam()]++;
    }
}