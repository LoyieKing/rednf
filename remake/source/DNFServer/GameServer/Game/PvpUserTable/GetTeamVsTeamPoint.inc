bool PvpUserTable::GetTeamVsTeamPoint(int team1, int team2)
{
    int count1 = GetTeamUserCount(team1);
    int count2 = GetTeamUserCount(team2);
    if (count1 == 0 || count2 == 0)
    {
        return 0;
    }
    if (!m_users[team1][0].flag)
    {
        return 0;
    }

    m_teamPoint[team1] = 0.0f;
    m_teamPoint[team2] = 0.0f;

    for (int i = 0; i < 8; ++i)
    {
        CUser* u = m_users[team1][i].user;
        GetUserVsTeamPoint(i, team1, team2, u);
        GetUserVsTeamPoint(i, team2, team1, u);
    }

    if (count1 > count2)
    {
        int diff = count1 - count2;
        for (int k = 0; k < diff; ++k)
        {
            GetGhostVsTeamPoint(team1, team2);
        }
    }

    float ratioSum = m_teamPoint[team1] + m_teamPoint[team2];
    if (ratioSum == 0.0f)
    {
        double dbl = ratioSum;
        // [G5 2026-09-10 P227 试验·无效已回滚] ORIG 0x085d4eee 的 `fstpl -0x48`（dbl 具名
        //   double 落槽）在 cMyTrace ctor 之前，理论上为全表达式临时链；改链式形实测
        //   四数不变、槽表逐值不变（我方 -0x10..-0x4c vs ORIG -0xc..-0x48）⇒ 回滚。
        cMyTrace tr(__PRETTY_FUNCTION__, 0x162, 5);
        tr("[FPE][PvpUserTable::GetTeamVsTeamPoint] RatioSum: %f",
           dbl);
        return 0;
    }

    float ratio = m_teamPoint[team2] / ratioSum;
    ratio *= (float)(count1 + count2);
    m_teamVsTeam[team1][team1] += ratio / (float)count1;
    m_teamVsTeam[team1][team2] -= ratio / (float)count2;
    return 1;
