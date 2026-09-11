PacketBuf* PacketPool::Acquire()
{
    // ORIG 0x828fdba：Acquire 失败时 logFormat（App.cpp:0x275）后返回 0；
    // 成功路径调用 packet->clear() 后返回。
    PacketBuf* packet = m_pool->Acquire();
    if (packet == 0)
    {
        LogManager::logFormat(
            1, "App.cpp", __PRETTY_FUNCTION__, 0x275,
            "[PacketPool alloc fail] curthreadid(%08x), poolthreadid(%08x), alloc_cnt(%d), size(%d)",
            pthread_self(), getThreadID(), m_pool->Remain(), m_pool->Size());
        return 0;
    }
    packet->clear();
    return packet;
}