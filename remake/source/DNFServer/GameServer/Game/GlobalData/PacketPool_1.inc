PacketPool::PacketPool(int poolSize, int allocSize)
{
    // ORIG 0x828fcfa：置 m_threadID=0；new DynamicPool(poolSize)；
    // SetPoolSize(poolSize, allocSize)；m_pCheckCallback=0。
    m_threadID = 0;
    m_pool = new DynamicPool<PacketBuf>(poolSize);
    m_pool->SetPoolSize(poolSize, allocSize);
    m_pool->m_pCheckCallback = 0;
}