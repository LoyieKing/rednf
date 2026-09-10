// df_game_r MsgQueueMgr 实现（ORIG 0x8570e40-0x8571257，AE 口径还原）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11MsgQueueMgrC2Ev`（ORIG 0x8571298）；ctor 已是首强符号。
#include <iostream>

#include "MsgQueueMgr.h"

MsgQueueMgr::MsgQueueMgr()
{
}

MsgQueueMgr::~MsgQueueMgr()
{
}

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
}

bool MsgQueueMgr::put(QUEUE_IDX queue_idx, CStreamGuard& guard)
{
    // ORIG 0x08570fde：tail-call 转发 Stream* 版并返回其返回值。
    return put(queue_idx, guard.Get());
}

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
}

bool MsgQueueMgr::empty(QUEUE_IDX queue_idx)
{
    Guard<Mutex> guard(&m_mutex[queue_idx]);
    return m_queue[queue_idx].empty();
}

size_t MsgQueueMgr::remain(QUEUE_IDX queue_idx)
{
    Guard<Mutex> guard(&m_mutex[queue_idx]);
    return m_queue[queue_idx].size();
}

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
}
