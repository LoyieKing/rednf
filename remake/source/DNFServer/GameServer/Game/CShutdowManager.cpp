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

// ORIG W 0x082a6fa8：ctor 即 Reset()（清零 +0x00/+0x04/+0x08）。
CShutdowManager::CShutdowManager()
{
    Reset();
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
    std::printf("WaitUntilExitThread (%d)\n", (int)(unsigned char)m_flag);
    if ((unsigned char)m_flag != 0)
        return true;
    return true;  // join 序列阻塞（见上）
}

CShutdowManager* GetInstanceShutdowManager()
{
    static CShutdowManager s;
    return &s;
}
// ORIG 0x082a6f88（_ZN15CShutdowManager5ResetEv，W）：
//   this+0 = 0；this+4 = 0；this+8 = 0。
void CShutdowManager::Reset()
{
    m_flag = 0;
    m_field4 = 0;
    m_field8 = 0;
}

// ORIG 0x082a6f70（_ZN15CShutdowManager14EnableShutdownEb，W）：
//   *(bool*)this = on。
void CShutdowManager::EnableShutdown(bool on)
{
    m_flag = static_cast<char>(on);
}

// ORIG 0x082a36bc（_ZN15CShutdowManager15OnAllUserLogoutEv，T，70 条）：
//   !m_flag → return；cMyTrace(PRETTY,0x323b,0)("Send message to monitor
//   for logout all user.")；guard 单例 stream：StreamPool::Acquire(
//   GlobalData::s_stream_pool, 0x323d) → CStreamGuard(s,true) << 0x3f
//   << -1；GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard)；
//   GlobalData::s_server_stop = 1。
void CShutdowManager::OnAllUserLogout()
{
    if (!static_cast<bool>(m_flag))
    {
        return;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x323b, 0);
    trace("Send message to monitor for logout all user.");
    {
        Stream* s =
            GlobalData::s_stream_pool->Acquire((const char*)"App.cpp", 0x323d);
        CStreamGuard guard(s, true);
        guard << 0x3f;
        guard << -1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    GlobalData::s_server_stop = true;
}

// ORIG 0x084ed1ea（_ZN15CShutdowManager29IncrementLastMsgResponseCountEv，W）：
//   ++*(int*)(this+0x08)。
void CShutdowManager::IncrementLastMsgResponseCount()
{
    ++m_field8;
}

// ORIG 0x084ed1fe（_ZN15CShutdowManager15GetLastMsgCountEv，W）：
//   return *(unsigned int*)(this+0x04)。
unsigned int CShutdowManager::GetLastMsgCount()
{
    return static_cast<unsigned int>(m_field4);
}

// ORIG 0x084ed20a（_ZN15CShutdowManager23GetLastMsgResponseCountEv，W）：
//   return *(unsigned int*)(this+0x08)。
unsigned int CShutdowManager::GetLastMsgResponseCount()
{
    return static_cast<unsigned int>(m_field8);
}
