// b28d-solo-PvpUserTable：ORIG 0x085d4bd0。三处翻正——
//   ① 0x085d4bd7 `mov 0x8(%ebp),%eax` 先于两个常量压栈 ⇒ P68 `(char*)this`；
//   ② 0x085d4caa 段先算第二实参（&m_teamVsTeam[j][i]）再算第一实参
//      （&m_minPoint[i]，`lea 0x4(%edx)`）⇒ std::min 实参序为
//      (m_minPoint[i], m_teamVsTeam[j][i])；
//   ③ 0x085d4c64 空 then 占位岛（`je +0x94(nop)` + body 尾 `jmp +0x95`）⇒
//      P223 正条件 goto 尾空标签形。
int PvpUserTable::Calculate()
{
    memset((char*)this, 0, 0x100);
    memset(m_teamPoint, 0, sizeof(m_teamPoint));
    CheckDan();

    for (int i = 0; i < 8; ++i)
    {
        SetWinTeam(i);
        for (int j = 0; j < 8; ++j)
        {
            if (i == j)
                goto skip_vs;
            GetTeamVsTeamPoint(i, j);
        skip_vs:;
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        m_minPoint[i] = 0.0f;
        for (int j = 0; j < 8; ++j)
        {
            m_minPoint[i] = std::min(m_minPoint[i], m_teamVsTeam[j][i]);
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            m_users[i][j].user = 0;
        }
    }
    return 1;
}