int ThreadInterface::join()
{
    if (pthread_join(m_tid, 0) != 0)
        return 0;
    return 1;
