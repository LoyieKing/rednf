CAssaultPlace* CAssaultPlaceFactory::AcquireAssaultPlace()   // ORIG 0x82f0f54
{
    return m_pool.Acquire();
