void Secu_DungeonAverageCheck::CheckHackTypeEndDungeon()
{
    if (m_party == 0)
        return;
    for (std::vector<SecKillMonsterInfo>::iterator it = m_killInfos.begin();
         it != m_killInfos.end(); ++it)
    {
        // ORIG 0x08279a4f/0x08279ac1：limit 表达式两次独立求值（P48，无具名缓存），
        // 每次 std::max<int>(limit,0) 独立（P167 两处调用点）。
        const int overTime = it->m_hackCntE6
            - std::max((int)(0.5 + 0.7 * (double)(unsigned int)it->m_count) - 4, 0);
        const int overExp = it->m_hackCntE7
            - std::max((int)(0.5 + 0.7 * (double)(unsigned int)it->m_count) - 4, 0);
        if (overTime > 0 || overExp > 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (m_party->checkValidUser(i))
                {
                    CUser* user = m_party->get_user(i);
                    const int code =
                        user->get_charac_job() * 100 +
                        (int)user->getCurCharFirstGrowType() * 10 +
                        (int)user->getCurCharSecondGrowType();
                    if (overTime > 0)
                    {
                        // ORIG 0x08279bce：SetETC 写在实参位（P52 链式 receiver +
                        // R2L 实参），结果不落具名局部（直接 %esi）。
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xe8,
                            (unsigned int)overTime,
                            (unsigned int)user->SetETC(0xe8, code), 0);
                    }
                    if (overExp > 0)
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xe9,
                            (unsigned int)overExp,
                            (unsigned int)user->SetETC(0xe9, code), 0);
                    }
                }
            }
        }
    }
