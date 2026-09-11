// ORIG PacketPool::PacketPool() 0x0828fc76：DynamicPool(10)，容量与步长均为10，
// 检查回调清零；这是 GlobalData::Init 使用的真实默认池，而非空构造。
PacketPool::PacketPool()
{
    m_threadID = 0;
    m_pool = new DynamicPool<PacketBuf>(10);
    m_pool->SetPoolSize(10, 10);
    m_pool->m_pCheckCallback = 0;
}