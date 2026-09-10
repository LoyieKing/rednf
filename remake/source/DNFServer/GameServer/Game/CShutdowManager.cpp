// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CShutdowManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "CShutdowManager/CShutdowManager.inc"
{
    Reset();
}

// ORIG 0x82a37c8：this 使能位(+0x00) 非零时向 DB 队列投递两条关服消息：
//   queue 2: << 0x95 << -1；queue 4: << 0x96 << -1；各使 this+0x04 计数 +1。
//   （ORIG 全程未使用 user 参数。）
// [b28d-27] DIFF N82：ORIG 0x082a37c8；p167 CLEAR（20/20），残差为帧/槽与
// 分支块排布，语义闭合。
#include "CShutdowManager/SendLastMsgDBQueue.inc"
{
    if (!static_cast<bool>(m_flag))
    {
        return 1;
    }
    {
        {
            // [P259 直证] ORIG 无 stream 具名局部（`call Acquire; mov %eax,0x4(%esp)`
            // eax 直传构造实参）⇒ 内联 Acquire 进 CStreamGuard 实参（两处）。
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("App.cpp", 0x324c), true);
            (*guard)->operator<<(0x95);
            (*guard)->operator<<(-1);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        }
        ++m_field4;

        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("App.cpp", 0x3257), true);
            (*guard)->operator<<(0x96);
            (*guard)->operator<<(-1);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
        }
        ++m_field4;
    }
    return 1;
}

#include "CShutdowManager/WaitUntilExitThread.inc"
{
    std::printf("WaitUntilExitThread (%d)\n", (int)(unsigned char)m_flag);
    // [b28d-27] P0/N96 清绑：ORIG 0x082a396e `movzbl (%eax),%eax; xor $1; test %al;
    // je <joins>` ⇒ 字段已 bool，裸 `if (!m_flag) { joins; return true; }` 即复现
    // （P248 档②；P214② 两个 return true 共享尾块）。
    // [ORIG 0x082a396e 直证] 形为 `movzbl; xor $1; test %al; je <joins>;
    // mov $0x1,%eax; jmp <end>; <joins>: …` ⇒ 原码是**否定守卫** `if (!m_flag)`
    // 且 else 臂（return true）独立成岛 ⇒ 写显式 else（if-return 双臂不共享尾块）。
    if (!m_flag)
    {
        // [b28d-27 N6S5ZeroA 修正] ORIG 0x082a396e `xor $0x1; test %al,%al;
        //   je 0x082a39a4(+0x36)`——je 的**落点就是 join 段**，且 je 紧后的
        //   fall-through 是 `mov $0x1,%eax; jmp 0x082a3a58`（第一处 return）。
        //   ⇒ 原码是**早退**形 `if (!m_flag) return true;` 后接 join 段 + 末尾
        //   return true；即 m_flag 为真(已使能关服)时才 join 线程。
        //   旧写法 `if(!m_flag){joins;return true;}else{return true;}` 令两臂
        //   对调：je 落在 else 的 return 上、fall-through 是 join ⇒ 语义相反
        //   （m_flag==0 才 join）——真语义 bug，此处按 ORIG 形改回。
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
#include "CShutdowManager/Reset.inc"
{
    m_flag = 0;
    m_field4 = 0;
    m_field8 = 0;
}

// ORIG 0x082a6f70（_ZN15CShutdowManager14EnableShutdownEb，W）：
//   *(bool*)this = on。
#include "CShutdowManager/EnableShutdown.inc"
{
    m_flag = on;
}

// ORIG 0x082a36bc（_ZN15CShutdowManager15OnAllUserLogoutEv，T，70 条）：
//   !m_flag → return；cMyTrace(PRETTY,0x323b,0)("Send message to monitor
//   for logout all user.")；guard 单例 stream：StreamPool::Acquire(
//   GlobalData::s_stream_pool, 0x323d) → CStreamGuard(s,true) << 0x3f
//   << -1；GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard)；
//   GlobalData::s_server_stop = 1。
#include "CShutdowManager/OnAllUserLogout.inc"
{
    // [b28d-27] P0/N96 清绑：ORIG 0x082a36bc `movzbl (%eax),%eax; xor $1; test %al;
    // jne <尾>` ⇒ 裸 `if (!m_flag) return;`（P248 档②）。
    if (!m_flag)
        return;
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
#include "CShutdowManager/IncrementLastMsgResponseCount.inc"
{
    ++m_field8;
}

// ORIG 0x084ed1fe（_ZN15CShutdowManager15GetLastMsgCountEv，W）：
//   return *(unsigned int*)(this+0x04)。
#include "CShutdowManager/GetLastMsgCount.inc"
{
    return static_cast<unsigned int>(m_field4);
}

// ORIG 0x084ed20a（_ZN15CShutdowManager23GetLastMsgResponseCountEv，W）：
//   return *(unsigned int*)(this+0x08)。
#include "CShutdowManager/GetLastMsgResponseCount.inc"
{
    return static_cast<unsigned int>(m_field8);
}
