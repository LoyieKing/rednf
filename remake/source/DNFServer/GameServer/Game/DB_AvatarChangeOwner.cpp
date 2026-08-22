// df_game_r 还原 —— DB_AvatarChangeOwner（球员换主 DB 请求，DBThread 派发）。
// 逐函数对照 docs/class_func_reports/DB_AvatarChangeOwner/makeRequest.md。
// 语义：向 DB 队列发一条 SIG_AVATAR_CHANGE_OWNER 请求，表头 0x35 + -1，
// 正文为 (a,b,c) 三个 uint 与 (d+10) 的字节（ORIG 0x0840055c）。
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "DB_AvatarChangeOwner.h"

// ============================================================================
// DB_AvatarChangeOwner::makeRequest @ 0x0840055c
// ============================================================================
void DB_AvatarChangeOwner::makeRequest(unsigned int a, unsigned int b,
                                       unsigned int c, unsigned char d)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb7f);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x35);
    guard.operator*()->operator<<(-1);
    SIG_AVATAR_CHANGE_OWNER* sig =
        guard.operator->()->GetInBuffer<SIG_AVATAR_CHANGE_OWNER>();
    char* p = (char*)sig;
    *(unsigned int*)(p + 0x0) = a;
    *(unsigned int*)(p + 0x4) = b;
    *(unsigned int*)(p + 0x8) = c;
    *(unsigned char*)(p + 0xc) = (unsigned char)(d + 10);  // param_4 + '\n'
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}