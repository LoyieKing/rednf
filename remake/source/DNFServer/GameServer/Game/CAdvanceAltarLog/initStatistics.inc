void CAdvanceAltarLog::initStatistics()
{
    Guard<Mutex> guard(&m_mutex);
    m_entrances.clear();
    m_layers.clear();
