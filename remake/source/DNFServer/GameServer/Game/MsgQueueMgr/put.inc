bool MsgQueueMgr::put(QUEUE_IDX queue_idx, Stream* stream)
{
    if (stream == NULL)
    {
        return false;
    }

    char* pBuf = stream->get_buffer();   // ORIG 存入未使用局部 -0xc(%ebp)
    stream->m_inUse = false;

    Guard<Mutex> guard(&m_mutex[queue_idx]);
    m_queue[queue_idx].push(stream);
    return true;
