// ORIG 符号: _ZN11CGM_Manager10SetGmLevelEji @ 0x0829932a
// writeGuard(m_lock)；find 命中后经 operator[] 写 +0x04（m_level）。无返回值。
void CGM_Manager::SetGmLevel(unsigned int accId, int level)
{
    writeGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it != m_gmMap.end())
    {
        m_gmMap[accId].m_level = level;
    }
}