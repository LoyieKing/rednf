void StatisticsToObserveCrash::incAbnormalDownUserCount()
{
    Guard<Mutex> guard(&m_mutex);
    m_abnormalDownUserCount += 1;
}