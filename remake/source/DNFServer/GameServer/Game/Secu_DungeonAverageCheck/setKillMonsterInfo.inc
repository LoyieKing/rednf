void Secu_DungeonAverageCheck::setKillMonsterInfo(const MSG_MONSTER_DIE* info,
                                                  map_monster* monster)
{
    // ORIG 0x82794d6。ROUND 为控制字截断（round toward zero），用 (int) 强转等价。
    if (m_party == 0)
        return;
    if (monster->isNamedMonster() || monster->m_roleType != 0)
        return;

    unsigned int monsterId = (unsigned int)monster->m_mobIndex;
    std::vector<SecKillMonsterInfo>::iterator it = findKillMonsterInfo(monsterId);
    if (it != m_killInfos.end())
    {
        if (4 < (unsigned int)it->m_count)
        {
            float avg1 = (float)(unsigned int)it->m_sumValue1 /
                         (float)(unsigned int)it->m_count;
            float avg2 = (float)(unsigned int)it->m_sumValue2 /
                         (float)(unsigned int)it->m_count;

            float adj1;
            if ((int)avg1 < 10)
                adj1 = 1.0f;
            else
                adj1 = 0.1f * avg1;

            unsigned int upper1 = (unsigned int)(int)(0.5f + avg1 + adj1);
            unsigned int lower1 = (unsigned int)(int)(avg1 - adj1);
            float adj2 = 0.05f * avg2;
            unsigned int upper2 = (unsigned int)(int)(0.5f + avg2 + adj2);
            unsigned int lower2 = (unsigned int)(int)(avg2 - adj2);

            if (m_party->m_mapHitCount > 0 &&
                lower1 <= info->m_killValue1 && info->m_killValue1 <= upper1)
            {
                it->m_hackCntE6 += 1;
                for (int i = 0; i < 4; ++i)
                {
                    if (!m_party->checkValidUser(i))
                        continue;
                    CUser* user = m_party->get_user(i);
                    CUser* userArg = m_party->get_user(i);
                    WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
                    analyzer->addServerHackCnt(userArg, (WongWork::ENUM_HACKTYPE)0xe6, 1, 0, 0);
                }
            }
            if (m_party->m_mapHitCount > 0 &&
                lower2 <= info->m_killValue2 && info->m_killValue2 <= upper2)
            {
                it->m_hackCntE7 += 1;
                for (int i = 0; i < 4; ++i)
                {
                    if (!m_party->checkValidUser(i))
                        continue;
                    CUser* user = m_party->get_user(i);
                    CUser* userArg = m_party->get_user(i);
                    WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
                    analyzer->addServerHackCnt(userArg, (WongWork::ENUM_HACKTYPE)0xe7, 1, 0, 0);
                }
            }
        }
        it->m_count += 1;
        it->m_sumValue1 += (int)info->m_killValue1;
        it->m_sumValue2 += (int)info->m_killValue2;
    }
    else
    {
        m_killInfos.push_back(SecKillMonsterInfo(
            monsterId, 1,
            (unsigned int)info->m_killValue1,
            (unsigned int)info->m_killValue2));
    }
