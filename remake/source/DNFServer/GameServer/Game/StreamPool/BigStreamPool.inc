BigStreamPool::BigStreamPool()
{
    int n = G_CEnvironment()->m_serverEnvir.m_bigPoolSize;
    m_pool = new DynamicPool<BigStream>(n);
    m_pool->SetPoolSize((unsigned short)n, 100);
    m_pool->m_pCheckCallback = 0;
