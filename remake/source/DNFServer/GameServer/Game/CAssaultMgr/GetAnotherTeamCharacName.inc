const char* CAssaultPlace::GetAnotherTeamCharacName(int idx)   // ORIG T 0x82eb3ba
{
    // ORIG 0x82eb3ba：`call Empty; xor $1; test; je ""岛` ⇒ 正向 !Empty 块；
    // 循环内两侧各自 GetTeam（无 myTeam 变量），第一结果落 %ebx（0x82eb3f3
    // `mov %eax,%ebx`），比较 `cmp %eax,%ebx; setne %al`（技巧22/28 组合）。
    if (!m_assaulters[idx].Empty())
    {
        for (int j = 0; j <= 7; ++j)
        {
            if (m_assaulters[j].GetTeam() != m_assaulters[idx].GetTeam())
            {
                CUser* u = m_assaulters[j].GetUser();
                if (u != 0)
                    return u->getCurCharacName();
            }
        }
    }
    return "";
}