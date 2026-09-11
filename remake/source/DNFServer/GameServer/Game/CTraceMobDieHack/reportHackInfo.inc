void CTraceMobDieHack::reportHackInfo()
{
    if (GlobalData::s_villageMonsterMgr->OnEvent())
    {
        reset();
        return;
    }
    for (int i = 0; i <= 3; ++i)
    {
        if (m_records[i].m_user == 0 || m_records[i].m_count <= 1)
        {
            continue;
        }
        int avg = m_records[i].m_damage / m_records[i].m_count;
        if (m_dungeonDiff != 5 && !m_bInspectUser)
        {
            int limit = 10;
            switch (m_dungeonDiff)
            {
            case 0:
                limit = 5;
                break;
            case 1:
                limit = 6;
                break;
            case 2:
                limit = 7;
                break;
            case 3:
                limit = 8;
                break;
            default:
                break;
            }
            if (avg <= limit)
            {
                unsigned int extra = (unsigned int)(limit - avg);
                m_records[i].m_user->getHackAnalyzer()->addServerHackCnt(
                    m_records[i].m_user, (WongWork::ENUM_HACKTYPE)0x19e,
                    extra + 1, 0, 0);
            }
        }
        if (m_bInspectUser)
        {
            if (avg < 10)
            {
                m_records[i].m_user->getHackAnalyzer()->addServerHackCnt(
                    m_records[i].m_user, (WongWork::ENUM_HACKTYPE)0x194, 1, 0, 0);
            }
        }
        else
        {
            if (avg < 0x14)
            {
                m_records[i].m_user->getHackAnalyzer()->addServerHackCnt(
                    m_records[i].m_user, (WongWork::ENUM_HACKTYPE)0x193, 1, 0, 0);
            }
        }
    }
    reset();
}