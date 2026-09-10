SmallStreamPool::~SmallStreamPool()
{
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
