void StatisticsShusiaService::reset()
{
    Guard<Mutex> guard(&m_mutex);
    m_dungeonFailure.clear();
    m_serviceUsage.clear();
}