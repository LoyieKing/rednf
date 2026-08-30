// ============================================================================
// W7_LinkTailDbs.cpp —— W7G_LinkTail 链接收敛批（DB 尾部符号唯一 owner）
// 覆盖 ORIG 符号：
//   T 0x0843f030 _ZN23DB_UpdateLimitItemUsage11makeRequestEiiiil
//   T 0x0843e146 _ZN29DB_UpdateReturnUserFirstLogin11makeRequestEjii
// 引用方：CUserOnlyOrig.cpp:79/84 局部类声明（签名一致）。
// 包构建约定与 DB_AccountCargoSync.cpp / DB_UpdateGiftCount.cpp 相同：
//   StreamPool::Acquire("DBThread.cpp", <行号>) → CStreamGuard(stream,true)
//   → 写 opcode/-1/参数 → MsgQueueMgr::put(DB_Q, guard)。
// ============================================================================

#include "GlobalData.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"

// ---- 类声明（与 CUserOnlyOrig.cpp:79/84 局部声明签名一致；ORIG nm 实证）----
class DB_UpdateLimitItemUsage
{
public:
    static void makeRequest(int, int, int, int, long);
};

class DB_UpdateReturnUserFirstLogin
{
public:
    static void makeRequest(unsigned int, int, int);
};

// ----------------------------------------------------------------------------
// DB_UpdateLimitItemUsage::makeRequest(int, int, int, int, long)
// ORIG @ 0x0843f030（报告 docs/class_func_reports/DB_UpdateLimitItemUsage/
// makeRequest.md）：函数体仅 push %ebp; mov %esp,%ebp; sub $0x10,%esp;
// leave; ret —— 纯空实现，无任何调用。
// ----------------------------------------------------------------------------
void DB_UpdateLimitItemUsage::makeRequest(int, int, int, int, long)
{
    // ORIG 空体（0x843f030..0x843f037，7 字节 ret 函数）
}

// ----------------------------------------------------------------------------
// DB_UpdateReturnUserFirstLogin::makeRequest(unsigned int, int, int)
// ORIG @ 0x0843e146（报告 docs/class_func_reports/DB_UpdateReturnUserFirstLogin/
// makeRequest.md）：Acquire("DBThread.cpp", 0x92e1) → guard(stream,true) →
//   << 0x21f | << -1 | << (uint)accId | << (int)a3 | << (int)a2
// → MsgQueueMgr::put(QUEUE_IDX(2)=DB_Q, guard)。
// ----------------------------------------------------------------------------
void DB_UpdateReturnUserFirstLogin::makeRequest(unsigned int accId, int a2, int a3)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x92e1);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x21f);  // SIG opcode（dbRequest 头）
    guard.operator*()->operator<<(-1);     // slot/mask 固定 -1
    guard.operator*()->operator<<(accId);  // +0x08 参数1（unsigned int）
    guard.operator*()->operator<<(a3);     // +0x10 参数3 先写
    guard.operator*()->operator<<(a2);     // +0x0c 参数2 后写

    GlobalData::s_msgq_mgr->put(
        static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);   // MsgQueueMgr::DB_Q
}

// ---- 类声明（与 CUserOnlyOrig.cpp:82 局部声明签名一致；ORIG nm 实证）----
class DB_UpdateBlueMarbleEnterCount
{
public:
    static void makeRequest(unsigned int, unsigned char);
};

// ----------------------------------------------------------------------------
// DB_UpdateBlueMarbleEnterCount::makeRequest(unsigned int accId,
//                                            unsigned char count)
// ORIG T 0x0844884c `_ZN29DB_UpdateBlueMarbleEnterCount11makeRequestEjh`
// （objdump 逐指令转写）：Acquire("DBThread.cpp", 0xb585) → guard(stream,true)
//   → << (int)0x30c | << (int)-1 | << (uint)accId | << (uchar)count
// → MsgQueueMgr::put(QUEUE_IDX(2)=DB_Q, guard)。
// ----------------------------------------------------------------------------
void DB_UpdateBlueMarbleEnterCount::makeRequest(unsigned int accId,
                                                unsigned char count)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb585), true);
    guard.operator*()->operator<<(0x30c);   // SIG opcode
    guard.operator*()->operator<<(-1);      // 固定 -1
    guard.operator*()->operator<<(accId);   // 参数1（unsigned int）
    guard.operator*()->operator<<(count);   // 参数2（unsigned char）

    GlobalData::s_msgq_mgr->put(
        static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
}
