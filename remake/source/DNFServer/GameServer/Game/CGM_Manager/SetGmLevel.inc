void CGM_Manager::SetGmLevel(unsigned int accId, int level)
{
    writeGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it != m_gmMap.end())
    {
        m_gmMap[accId].m_level = level;
    }
