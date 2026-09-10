bool BigStreamPool::Free(Stream* s)
{
    BigStream* p = (BigStream*)s;
    if (!p)
    {
        return false;
    }
    Guard<Mutex> guard(&m_mutex);
    if (!m_pool->IsValidPtr(p))
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x2fc,
                              "[BigStreamPool::Free] invalid ptr : %p", p);
        return false;
    }
    p->reset();
    m_pool->Free(p);
    return true;
