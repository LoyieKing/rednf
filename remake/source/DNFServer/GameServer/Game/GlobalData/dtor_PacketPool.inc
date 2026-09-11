PacketPool::~PacketPool()
{
    // ORIG 0x828fd82：if(m_pool){ delete m_pool; m_pool=0; }。
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
}