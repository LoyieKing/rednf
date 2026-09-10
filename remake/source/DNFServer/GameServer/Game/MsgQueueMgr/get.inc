Stream* MsgQueueMgr::get(QUEUE_IDX queue_idx)
{
    Guard<Mutex> guard(&m_mutex[queue_idx]);
    if (m_queue[queue_idx].empty())
    {
        return NULL;
    }

    Stream* stream = m_queue[queue_idx].front();
    m_queue[queue_idx].pop();
    return stream;
