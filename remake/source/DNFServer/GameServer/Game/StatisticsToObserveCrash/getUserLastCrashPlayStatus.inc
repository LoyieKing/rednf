int StatisticsToObserveCrash::getUserLastCrashPlayStatus(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPlayStatus.find(userIdx);
    if (it != m_crashPlayStatus.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}