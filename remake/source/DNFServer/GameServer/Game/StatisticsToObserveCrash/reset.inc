void StatisticsToObserveCrash::reset()
{
    Guard<Mutex> guard(&m_mutex);
    m_abnormalDownUserCount = 0;
    m_crashPacketID.clear();
    m_crashDungeonID.clear();
    m_crashPlayStatus.clear();
}