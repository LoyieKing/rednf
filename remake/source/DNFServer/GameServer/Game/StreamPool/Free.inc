bool StreamPool::Free(Stream* s)
{
    if (!s)
    {
        return false;
    }
    Guard<Mutex> guard(&m_mutex);
    s->m_inUse = 0;
    if (!m_pool->IsValidPtr(s))
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x239,
                              "[StreamPool::Free] invalid ptr : %p", s);
        return false;
    }
    s->reset();
    m_pool->Free(s);
    return true;
}