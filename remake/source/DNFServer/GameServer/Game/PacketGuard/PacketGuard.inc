PacketGuard::PacketGuard()
    : InterfacePacketBuf(), m_pool(0), m_external(false)
{
    for (int i = 0; i < 9; ++i)
    {
        // [b24ab] P48：ORIG 无 pool 具名局部、无空判，逐点取 list[i]；线程比较
        // 走 pthread_equal（非 ==）。
        if (pthread_equal(GlobalData::s_packet_pool_list[i]->getThreadID(),
                          pthread_self()) != 0)
        {
            m_pool = GlobalData::s_packet_pool_list[i];
            break;
        }
    }
    if (m_pool)
    {
        AcquirePacketBuf(m_pool->Acquire());
    }
    else
    {
        LogManager::logFormat(1, "packet_buf.cpp", __PRETTY_FUNCTION__, 0x3e5,
                              "[PacketPool Acquire err - pool_type_err] "
                              "thread_id(%08x)",
                              (unsigned int)pthread_self());
    }
