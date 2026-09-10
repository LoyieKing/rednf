bool ThreadInterface::begin()
{
    int rc = pthread_create(&m_tid, NULL,
                            &ThreadInterface::dispatch_proxy, this);
    if (rc != 0)
    {
        std::cerr << "[ThreadInterface::begin] Can't begin thread." << " "
                  << rc << std::endl;
    }
    m_tidCopy = m_tid;
    return true;
