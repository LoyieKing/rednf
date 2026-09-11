int StatisticsToObserveCrash::getUserLastCrashDungeonID(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashDungeonID.find(userIdx);
    if (it != m_crashDungeonID.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}