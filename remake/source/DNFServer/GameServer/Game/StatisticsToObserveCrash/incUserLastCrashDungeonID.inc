void StatisticsToObserveCrash::incUserLastCrashDungeonID(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashDungeonID.find(userIdx);
    if (it == m_crashDungeonID.end())
    {
        m_crashDungeonID.insert(std::map<int, int>::value_type(userIdx, 1));
    }
    else
    {
        (*it).second += 1;
    }
