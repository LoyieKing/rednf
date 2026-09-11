int StatisticsToObserveCrash::getUserLastCrashPacketID(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPacketID.find(userIdx);
    if (it != m_crashPacketID.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}