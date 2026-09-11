SmallStreamPool::SmallStreamPool()
{
    int n = G_CEnvironment()->m_serverEnvir.m_smallPoolSize;
    m_pool = new DynamicPool<SmallStream>(n);
    m_pool->SetPoolSize((unsigned short)n, 1000);
    m_pool->m_pCheckCallback = 0;
}