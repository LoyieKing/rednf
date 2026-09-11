// ORIG T 0x081046f0 (0x63)：Guard(allocMutex) → pool.Free。
void CIPGMgr::FreeIPGData(SIPGData* data)
{
    Guard<Mutex> guard(&m_allocMutex);
    m_pool.Free(data);
}
