// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_AvatarChangeOwner/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "DB_AvatarChangeOwner/makeRequest.inc"
                                       unsigned int c, unsigned char d)
{
    // [b28d-48] P259：ORIG 0x0840055c 的 Acquire 结果直传 CStreamGuard 实参
    // （无 stream 具名局部；`lea -0x14; mov %eax,0x4(%esp)` 直传），具名局部使
    // 4.4 多发 `mov %eax,-0x14` 缓存并使帧 0x30→0x40。
    // 收口：NDIFF 63→47（72/77），帧已对齐 0x30；残差 = guard 槽 -0x18 vs
    // -0x14 与 d+10 的 addb 形（ORIG `addb $0xa,-0x1c(%ebp)` 直接改形参槽
    // vs 4.4 读中转），N82/N18a 封顶。
    // [N1Free91to105 2026-09-11] 按 ORIG 槽集再收：
    //  · ORIG 0x08400564 `mov 0x14(%ebp),%eax; mov %al,-0x1c(%ebp)` 是**被修改形参
    //    d 的入口 spill 槽**（4.1 -O0 对 `d += 10` 的形参先在入口落栈）；
    //    0x084005ea `addb $0xa,-0x1c(%ebp)` = 中途的 `d += 10`（非内联 `(d+10)`）。
    //    改写成具名局部 dd 反而多出 `movzbl/mov %al,-0xd` 两条（已实测回退）。
    //  · ORIG 无 `p` 槽（sig 直接经 -0xc 使用）⇒ 删 `char* p` 具名局部（P0/P183）。
    //  · 写点用**具名成员**（`sig->m_uiId` 等）：ORIG 0x084005f1/0x084005f9/0x08400602
    //    为 `mov 0xc(%ebp),%edx; mov %edx,0x4(%eax)` 直 disp 形；写成
    //    `*(uint*)((char*)sig+4)` 会令 4.4 多发 `lea 0x4(%eax),%edx` 三条（已实测）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb7f), true);
    guard.operator*()->operator<<(0x35);
    guard.operator*()->operator<<(-1);
    SIG_AVATAR_CHANGE_OWNER* sig =
        guard.operator->()->GetInBuffer<SIG_AVATAR_CHANGE_OWNER>();
    d += 10;
    sig->m_uiId = a;
    sig->m_characNo = b;
    sig->m_newCharacNo = c;
    sig->m_slot = d;  // param_4 + '\n'
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}