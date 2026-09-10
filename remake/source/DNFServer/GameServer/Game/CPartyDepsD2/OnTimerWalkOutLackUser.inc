bool CParty::OnTimerWalkOutLackUser()   // CParty.h:657 bool（PSM 落，定义同步）
{
    int victimIdx = m_election.GetElected();
    while (victimIdx >= 0)
    {
        // P184 死块照抄：ORIG +0x1e `cmpl $0; js → nop岛→return1`（4.1 物化
        // 头部冗余检查，4.4 -O0 同发射）。
        if (victimIdx < 0)
            break;
        m_election.CancelCandidate(victimIdx);
        m_election.CancelVoter(victimIdx);
        CUser* victim = m_member[victimIdx].m_pUser;
        if (victim == 0)
        {
            LogManager::logFormat(1, "party.cpp",
                                  __PRETTY_FUNCTION__, 0x35c6,
                                  "pVictim is null");
            return 0;
        }
        SendWalkout(victimIdx, (ENUM_WALKOUT_TYPE)1);
        walkout_member(m_manager, victimIdx);
        G_GameWorld()->out_from_dungeon(victim);
        victimIdx = m_election.GetElected();
    }
    return 1;
