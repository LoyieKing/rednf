// [b28d-62] AE：同 incUserLastCrashPacketID。ORIG 0x0860bed4
void StatisticsToObserveCrash::incUserLastCrashPlayStatus(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPlayStatus.find(userIdx);
    if (it == m_crashPlayStatus.end())
    {
        m_crashPlayStatus.insert(std::map<int, int>::value_type(userIdx, 1));
    }
    else
    {
        (*it).second += 1;
    }
}