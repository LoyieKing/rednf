int StatisticsShusiaService::getDungeonFailure(int dungeonIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_dungeonFailure.find(dungeonIdx);
    if (it != m_dungeonFailure.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}