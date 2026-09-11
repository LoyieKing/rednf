void StatisticsPvP::reset()
{
    Guard<Mutex> guard(&m_mutex);
    m_map.clear();
}