int CGM_Manager::GetGmLevel(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    return m_gmMap[accId].m_level;
