// ORIG T 0x08156bca（initStatistics.md）：Guard 内双 map clear。
void CAdvanceAltarLog::initStatistics()
{
    Guard<Mutex> guard(&m_mutex);
    m_entrances.clear();
    m_layers.clear();
}