const char* MsgQueueMgr::get_queue_name(QUEUE_IDX queue_idx)
{
    switch (queue_idx)
    {
    case NETWORK_Q:
        return "NETWORK_Q";
    case DISPATCH_Q:
        return "DISPATCH_Q";
    case DB_Q:
        return "DB_Q";
    case TIMER_Q:
        return "TIMER_Q";
    case LOGDB_Q:
        return "LOGDB_Q";
    default:
        return "?????";
    }
