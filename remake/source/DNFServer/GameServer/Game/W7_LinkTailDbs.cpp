// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W7_LinkTailDbs/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "W7_LinkTailDbs/makeRequest.inc"
{
    // ORIG 空体（0x843f030..0x843f037，7 字节 ret 函数）
    // ORIG 带 outgoing 帧 sub $0x10（4.1 空转发残留形）；P31 dead local 复现帧深。
    char framePad[0x10];
    (void)framePad;
}

// ----------------------------------------------------------------------------
// DB_UpdateReturnUserFirstLogin::makeRequest(unsigned int, int, int)
// ORIG @ 0x0843e146（报告 docs/class_func_reports/DB_UpdateReturnUserFirstLogin/
// makeRequest.md）：Acquire("DBThread.cpp", 0x92e1) → guard(stream,true) →
//   << 0x21f | << -1 | << (uint)accId | << (int)a3 | << (int)a2
// → MsgQueueMgr::put(QUEUE_IDX(2)=DB_Q, guard)。
// ----------------------------------------------------------------------------
#include "W7_LinkTailDbs/makeRequest_1.inc"
{
    // ORIG Acquire 结果直传构造（无 stream 局部槽，P89/P134）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x92e1), true);
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
#include "W7_LinkTailDbs/makeRequest_2.inc"
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
