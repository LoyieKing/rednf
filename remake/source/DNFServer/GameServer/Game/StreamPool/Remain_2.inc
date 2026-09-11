int BigStreamPool::Remain()
{
    Guard<Mutex> guard(&m_mutex);
    return m_pool->Remain();
}