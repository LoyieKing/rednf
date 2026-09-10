Stream* StreamPool::Acquire(const char* file, int line)
{
    Guard<Mutex> guard(&m_mutex);
    Stream* s = m_pool->Acquire();
    if (!s)
    {
        return 0;
    }
    s->m_file = file;
    s->m_line = line;
    s->m_poolType = 0;
    s->set_big_stream(false);
    s->m_inUse = 1;
    return s;
