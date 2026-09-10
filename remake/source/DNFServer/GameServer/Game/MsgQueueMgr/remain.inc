size_t MsgQueueMgr::remain(QUEUE_IDX queue_idx)
{
    Guard<Mutex> guard(&m_mutex[queue_idx]);
    return m_queue[queue_idx].size();
