void StatisticsToObserveCrash::setAbnormalDownUserCount(int count)
{
    Guard<Mutex> guard(&m_mutex);
    m_abnormalDownUserCount = count;
}