bool MsgQueueMgr::put(QUEUE_IDX queue_idx, CStreamGuard& guard)
{
    // ORIG 0x08570fde：tail-call 转发 Stream* 版并返回其返回值。
    return put(queue_idx, guard.Get());
