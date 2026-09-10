CIPGMgr::CIPGMgr(IIPGHelperInterface* helper)
    : m_workers(0),
      m_resQueue(),
      m_pool(1000),
      m_helper(helper),
      m_allocMutex(),
      m_reqMutex()
{
