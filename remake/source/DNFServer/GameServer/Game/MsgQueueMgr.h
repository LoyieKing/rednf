#ifndef GAME_MSGQUEUEMGR_H_
#define GAME_MSGQUEUEMGR_H_

#include <deque>
#include <queue>

#include "GameBase.h"

// df_game_r MsgQueueMgr —— 6 条互斥保护的消息队列（ORIG 0x8570e40）。
// 布局：Mutex[6] @ +0x00（0x90 字节），queue[6] @ +0x90（0xf0 字节），总 0x180。
// ctor/dtor 反汇编均为 5→-1 共 6 次循环；get_queue_name 仅命名 0..4，
// 第 6 槽（idx=5）无队列名（default "?????"）。
class MsgQueueMgr
{
public:
    enum QUEUE_IDX
    {
        NETWORK_Q = 0,
        DISPATCH_Q = 1,
        DB_Q = 2,
        TIMER_Q = 3,
        LOGDB_Q = 4
    };

    MsgQueueMgr();
    ~MsgQueueMgr();

    bool put(QUEUE_IDX queue_idx, Stream* stream);
    void put(QUEUE_IDX queue_idx, CStreamGuard& guard);
    Stream* get(QUEUE_IDX queue_idx);
    bool empty(QUEUE_IDX queue_idx);
    size_t remain(QUEUE_IDX queue_idx);
    static const char* get_queue_name(QUEUE_IDX queue_idx);

private:
    Mutex m_mutex[6];                                       // +0x00
    std::queue<Stream*, std::deque<Stream*> > m_queue[6];   // +0x90
};

#endif  // GAME_MSGQUEUEMGR_H_
