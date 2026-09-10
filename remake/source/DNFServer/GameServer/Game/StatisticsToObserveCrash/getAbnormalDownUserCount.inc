int StatisticsToObserveCrash::getAbnormalDownUserCount()
{
    Guard<Mutex> guard(&m_mutex);
    return m_abnormalDownUserCount;
