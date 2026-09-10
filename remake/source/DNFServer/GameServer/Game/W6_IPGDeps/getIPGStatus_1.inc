void CIPGMgr::getIPGStatus(stIPGStatus_t& status)
{
    status.m_field00 = m_pool.Remain();
    status.m_field04 = m_pool.Size();
    status.m_field08 = m_reqQueue.size();
}
