int CDeathTowerPlayDataStatistic::SendDeathTowerPlayInfo(
    int idx, char deathTowerType,
    const stDeathTowerRecordMemberInfo_t* members,
    unsigned int clearStage, unsigned int partyCount)
{
    Packet_DeathTower_Statistic_Playdata_Job jobPkt;
    jobPkt.m_deathTowerType = deathTowerType;
    jobPkt.m_level = (unsigned short)members[idx].m_level;
    jobPkt.m_characJob = (unsigned int)members[idx].m_job;
    jobPkt.m_characGrow = (char)members[idx].m_grow;
    jobPkt.m_field = (int)clearStage;
    if (jobPkt.m_level == 0)
    {
        LogManager::logFormat(1, "Statistics.cpp",
            __PRETTY_FUNCTION__,
            0x5fe,
            "CDeathTowerPlayDataStatistic ERROR!! deathTower_type (%d) level (%d) job_ (%d) grow_type_ (%d) clearStage (%d)",
            (int)(char)jobPkt.m_deathTowerType, (int)(short)jobPkt.m_level,
            (int)jobPkt.m_characJob, (int)(char)jobPkt.m_characGrow,
            (int)jobPkt.m_field);
    }
    else
    {
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
            ->SendPacket(reinterpret_cast<char*>(&jobPkt), 0x16);
    }

    Packet_DeathTower_Statistic_Playdata_Party partyPkt;
    partyPkt.m_deathTowerType = deathTowerType;
    partyPkt.m_partyCount = (char)partyCount;
    partyPkt.m_field = (int)clearStage;
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&partyPkt), 0x10);
    return 1;
}