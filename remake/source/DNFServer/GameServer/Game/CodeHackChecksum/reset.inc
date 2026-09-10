void CodeHackCheckStorage::reset()                        // W @0x08575e74
{
    while (!m_queue.empty())                              // empty→xor 1 判定
        m_queue.pop();
