StreamPool::StreamPool()
{
    int n = G_CEnvironment()->m_serverEnvir.m_defaultPoolSize;
    m_pool = new DynamicPool<Stream>(n);
    m_pool->SetPoolSize((unsigned short)n, 1000);
    m_pool->m_pCheckCallback = 0;
}