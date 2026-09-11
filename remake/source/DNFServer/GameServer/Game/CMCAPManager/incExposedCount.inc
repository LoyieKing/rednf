// ORIG T 0x0822eea4：max<unsigned>(min<unsigned>(10000, m_point), 0)
// ORIG W 0x08695038：m_field10(+0x10) += 1。
void CMCAPManager::incExposedCount()
{
    m_field10 = m_field10 + 1;
}