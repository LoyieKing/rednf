SIPGData* CIPGMgr::AllocIPGData()
{
    Guard<Mutex> guard(&m_allocMutex);
    SIPGData* data = m_pool.Acquire();
    if (data != 0)
    {
        memset(data, 0, sizeof(SIPGData));   // ORIG movl $0x2dc（整块清零，非字段访存）
    }
    return data;
