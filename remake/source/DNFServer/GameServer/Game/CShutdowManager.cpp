// df_game_r 还原 —— CShutdowManager（G2-4 批次，关服管理人）。
// 定义 SendLastMsgDBQueue（ORIG _ZN15CShutdowManager18SendLastMsgDBQueueEP5CUser）
// 及默认构造（GetInstanceShutdowManager 的 static 单例需要）。
// （注：上游单例 GetInstanceShutdowManager 由 CGameManager.cpp TU 提供。）

#include "CShutdowManager.h"

#include <cstdio>

#include "CUser.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

CShutdowManager::CShutdowManager()
{
}

// ORIG 0x82a37c8：user 标志(+0x00)==1 时向 DB 队列投递两条关服消息：
//   queue 2: << 0x95 << -1；queue 4: << 0x96 << -1；各使 user+0x04 计数 +1。
unsigned int CShutdowManager::SendLastMsgDBQueue(CUser* user)
{
    if (*reinterpret_cast<int*>(user) == 1)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("App.cpp", 0x324c);
        {
            CStreamGuard guard(stream, true);
            (*guard)->operator<<(0x95);
            (*guard)->operator<<(-1);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        }
        ++*reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(user) + 4);

        stream = GlobalData::s_stream_pool->Acquire("App.cpp", 0x3257);
        {
            CStreamGuard guard(stream, true);
            (*guard)->operator<<(0x96);
            (*guard)->operator<<(-1);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
        }
        ++*reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(user) + 4);
    }
    return 1;
}

// ORIG _ZN15CShutdowManager19WaitUntilExitThreadEv（0x82a396e，0xec 字节）：
// printf("WaitUntilExitThread (%d)\n", (int)m_pad[0])；m_pad[0]!=0 直接返回 true；
// 否则 join 8 个线程全局（0x940bdb0..0x940bdc8：db/udp/monitor/tcp/db/logdb/
// accept/net thread），每次 join 后 puts 对应 "xxx thread join"。
// 【阻塞】ThreadInterface 与 8 个线程全局尚未在重建中建模，join 序列待线程域
// 还原后补齐；当前仅实现标志检查路径。
bool CShutdowManager::WaitUntilExitThread()
{
    std::printf("WaitUntilExitThread (%d)\n", (int)(unsigned char)m_pad[0]);
    if ((unsigned char)m_pad[0] != 0)
        return true;
    return true;  // join 序列阻塞（见上）
}

CShutdowManager* GetInstanceShutdowManager()
{
    static CShutdowManager s;
    return &s;
}