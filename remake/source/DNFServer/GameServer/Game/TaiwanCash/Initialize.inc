int CIPGMgr::Initialize(unsigned int workerCount, unsigned int param2)
{
    if (m_workers != 0)
    {
        return -1;
    }
    if (workerCount != 0)
    {
        m_workers = new CIPGWorker[workerCount];
        for (unsigned int i = 0; i < workerCount; ++i)
        {
            m_workers[i].SetIPGMgr(this);
            m_workers[i].begin();
        }
    }
    m_param2 = param2;
    m_workerCount = workerCount;

    if (!InitBillingCash(m_billingCash))
    {
        LogManager::logFormat(
            1, "IPGMgr.cpp",
            __PRETTY_FUNCTION__,
            0x98, "Global billing cash initialize failed.");
        return -1;
    }
    return 0;
