// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W7_LinkTailDbs/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "W7_LinkTailDbs/makeRequest.inc"
}

// ----------------------------------------------------------------------------
// DB_UpdateReturnUserFirstLogin::makeRequest(unsigned int, int, int)
// ORIG @ 0x0843e146（报告 docs/class_func_reports/DB_UpdateReturnUserFirstLogin/
// makeRequest.md）：Acquire("DBThread.cpp", 0x92e1) → guard(stream,true) →
//   << 0x21f | << -1 | << (uint)accId | << (int)a3 | << (int)a2
// → MsgQueueMgr::put(QUEUE_IDX(2)=DB_Q, guard)。
// ----------------------------------------------------------------------------
#include "W7_LinkTailDbs/makeRequest_1.inc"
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
}
