void PacketPool::Free(PacketBuf* packet)
{
    // ORIG 0x858e2ae：无空判，直接转调 DynamicPool::Free。
    m_pool->Free(packet);
