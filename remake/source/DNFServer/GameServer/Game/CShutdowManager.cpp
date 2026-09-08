// df_game_r 还原 —— CShutdowManager（G2-4 批次，关服管理人）。
// 定义 SendLastMsgDBQueue（ORIG _ZN15CShutdowManager18SendLastMsgDBQueueEP5CUser）
// 及默认构造（GetInstanceShutdowManager 的 static 单例需要）。
// （注：上游单例 GetInstanceShutdowManager 由 CGameManager.cpp TU 提供。）

#include "CShutdowManager.h"
#include "ThreadInterface.h"

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

// ORIG 0x82a37c8：this 使能位(+0x00) 非零时向 DB 队列投递两条关服消息：
//   queue 2: << 0x95 << -1；queue 4: << 0x96 << -1；各使 this+0x04 计数 +1。
//   （ORIG 全程未使用 user 参数。）
unsigned int CShutdowManager::SendLastMsgDBQueue(CUser* user)
{
    if (!static_cast<bool>(m_flag))
    {
        return 1;
    }
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("App.cpp", 0x324c);
        {
            CStreamGuard guard(stream, true);
            (*guard)->operator<<(0x95);
            (*guard)->operator<<(-1);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        }
        ++m_field4;

        stream = GlobalData::s_stream_pool->Acquire("App.cpp", 0x3257);
        {
            CStreamGuard guard(stream, true);
            (*guard)->operator<<(0x96);
            (*guard)->operator<<(-1);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
        }
        ++m_field4;
    }
    return 1;
}

bool CShutdowManager::WaitUntilExitThread()
{
    std::printf("WaitUntilExitThread (%d)\n", (int)(unsigned char)m_flag);
    register unsigned char v asm("eax") = m_flag;
    v ^= 1;
    if (v)
    {
        return true;
    }
    // ORIG 0x82a396e 逐条转写：按地址序 join 7 个线程全局（s_dispatch_thr →
    // s_udp_thr → s_monitor_tcp_thr → s_db_thr → s_db_thr_for_logDB →
    // s_accept_thr → s_net_thr），每次 join 后 puts 对应描述串。
    // ORIG 直接 call ThreadInterface::join（非虚）；线程全局在 ORIG 以
    // ThreadInterface* 使用，重建声明为派生指针，此处 reinterpret 保持 this 值。
    reinterpret_cast<ThreadInterface*>(GlobalData::s_dispatch_thr)->join();
    std::puts("dispatch thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_udp_thr)->join();
    std::puts("udp thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_monitor_tcp_thr)->join();
    std::puts("monitor tcp thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr)->join();
    std::puts("db thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr_for_logDB)->join();
    std::puts("log db thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_accept_thr)->join();
    std::puts("accept thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_net_thr)->join();
    std::puts("net thread join");
    return true;
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
    m_flag = on;
}

// ORIG 0x082a36bc（_ZN15CShutdowManager15OnAllUserLogoutEv，T，70 条）：
//   !m_flag → return；cMyTrace(PRETTY,0x323b,0)("Send message to monitor
//   for logout all user.")；guard 单例 stream：StreamPool::Acquire(
//   GlobalData::s_stream_pool, 0x323d) → CStreamGuard(s,true) << 0x3f
//   << -1；GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard)；
//   GlobalData::s_server_stop = 1。
void CShutdowManager::OnAllUserLogout()
{
    register unsigned char v2 asm("eax") = m_flag;
    v2 ^= 1;
    if (v2)
    {
        return;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x323b, 0);
    // ORIG +0x03c `movl $"ALL USER LOGOUT COMPLETE\n"`——日志第二点：消息串须与
    // ORIG 逐字节一致（原"Send message to monitor for logout all user."是错误串）。
    trace("ALL USER LOGOUT COMPLETE\n");
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           (const char*)"App.cpp", 0x323d),
                       true);
    guard.operator*()->operator<<(0x3f);      // op*(guard)→op<<(int)
    guard.operator*()->operator<<(-1);        // op*(guard)→op<<(int)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
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
