int CClientCrashDownLogStatistic::CarryClientCrashDown()
{
    const time_t now = GlobalData::s_systemTime_.getCurSec();
    if (!m_reasonCount.empty())
    {
        Packet_Reason_Crash_Down_Info packet;
        packet.m_occTime = static_cast<int>(now);
        packet.m_channel = G_CEnvironment()->get_channel_no();
        for (std::map<int, int>::iterator it = m_reasonCount.begin();
             it != m_reasonCount.end(); ++it)
        {
            packet.m_reason = it->first;
            packet.m_cnt = it->second;
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
                ->SendPacket(reinterpret_cast<char*>(&packet), 0x1a);
        }

        stClientCrashDownLog_t record;
        record.m_occTime = static_cast<int>(now);
        record.m_reasonCount = m_reasonCount;
        m_logRecords.push_back(record);
        m_reasonCount.clear();
        Init();
    }
    struct tm tmBuf;
    struct tm* pTm = localtime_r(&now, &tmBuf);
    (void)pTm;
    return 1;
}