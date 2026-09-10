void CIPGMgr::FreeIPGData(SIPGData* data)
{
    Guard<Mutex> guard(&m_allocMutex);
    m_pool.Free(data);
}
