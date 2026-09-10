void CMCAPManager::reset()
{
    m_point = 1;
    m_rate = 0x3e8;
    m_field08 = static_cast<int>(m_point);
    m_field0c = 1;
    m_field10 = 0;
    m_saveCleanPadPoint = false;
