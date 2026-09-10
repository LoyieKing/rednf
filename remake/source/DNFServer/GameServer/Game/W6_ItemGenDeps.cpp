// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_ItemGenDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CItemGeneratorMgr.h"
#include "CItemGenerator.h"
#include "StreamDeps.h"        // G_CEnvironment（ORIG wrapper 0x080cc181）
#include "CEnvironment.h"      // Server_Envir 完整布局（m_serverEnvir +0x198 / m_channelNo +0x18）
#include "GlobalData.h"        // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "W8LT_DBTask.h"       // 权威声明点 DB_UpdateItemDropStatistics（§4.4/P104 ODR 修复）

#include <cstring>

// ============================================================================
// DB_UpdateItemDropStatistics —— 投递统计落库请求（opcode 0xd2）
//   stream: 0xd2 | -1 | serverId(int) | typeId(int) | p3[0..5] | p4[0..5]
//           | p5[0..5] | p6[0..5] → MsgQueueMgr::put(DB_Q)
//   类定义唯一声明点在 W8LT_DBTask.h（含 DBDispatcher 基类 + virtual dispatch，
//   与本 TU 旧局部最小声明曾构成 ODR 冲突）；本 TU 仅提供 makeRequest 定义。
// ============================================================================

// ORIG T 0x0842894c _ZN27DB_UpdateItemDropStatistics11makeRequestEiiPjS0_S0_S0_
#include "W6_ItemGenDeps/makeRequest.inc"
                                              unsigned int* counts0,
                                              unsigned int* counts1,
                                              unsigned int* counts2,
                                              unsigned int* counts3)
{
    // ORIG 直接把 Acquire 返回值传入守卫构造（无中间局部，栈帧一致）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6a14), true);
    guard.operator*()->operator<<(0xd2);      // SIG_UPDATE_ITEM_DROP_STATISTICS
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(serverId);  // ORIG 实参 = env+0x1b0
                                              //   = m_serverEnvir.m_channelNo
    guard.operator*()->operator<<(typeId);

    // 四组计数各写 6 个 uint（asm: i<=5 循环 cmpl $0x5 + setle）
    for (int i = 0; i <= 5; ++i)
        guard.operator*()->operator<<(counts0[i]);
    for (int i = 0; i <= 5; ++i)
        guard.operator*()->operator<<(counts1[i]);
    for (int i = 0; i <= 5; ++i)
        guard.operator*()->operator<<(counts2[i]);
    for (int i = 0; i <= 5; ++i)
        guard.operator*()->operator<<(counts3[i]);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ============================================================================
// WongWork::CItemGeneratorMgr::onTimerEvent(int)
//   ORIG T 0x08534322：报告 WongWork__CItemGeneratorMgr/onTimerEvent.md
//   逐段转写。timer==0x6b 时依次对三组统计区发 makeRequest 后 memset 清零；
//   其余直接返回。
// ============================================================================
namespace WongWork
{
#include "W6_ItemGenDeps/onTimerEvent.inc"
{
    // ORIG 控制流即 `if (param_1 == 0x6b) { … } return;`（正向判定，函数尾
    // 直落 epilogue，无额外 jmp）。
    if (timer == 0x6b)
    {
        // ORIG（0x08534322）每次调用独立 call G_CEnvironment() 后
        // `mov 0x1b0(%eax),%eax` —— env+0x1b0 = Server_Envir(+0x198)::m_channelNo(+0x18)，
        // 故首参具名为 m_serverEnvir.m_channelNo（原写法 `G_CEnvironment() + 0x1b0`
        // 是 CEnvironment* 上的指针算标度，非字节偏移，属真实缺陷，本次一并修正）。
        // 四组统计指针即本对象计数表 m_dropStat[g][1..4]（每行 6 uint，
        // 行距 0x18、genType 距 0x78；ORIG `add $imm,%eax` 序列同源）。
        DB_UpdateItemDropStatistics::makeRequest(
            G_CEnvironment()->m_serverEnvir.m_channelNo, 0,
            m_dropStat[0][1], m_dropStat[0][2], m_dropStat[0][3], m_dropStat[0][4]);

        DB_UpdateItemDropStatistics::makeRequest(
            G_CEnvironment()->m_serverEnvir.m_channelNo, 1,
            m_dropStat[1][1], m_dropStat[1][2], m_dropStat[1][3], m_dropStat[1][4]);

        DB_UpdateItemDropStatistics::makeRequest(
            G_CEnvironment()->m_serverEnvir.m_channelNo, 2,
            m_dropStat[2][1], m_dropStat[2][2], m_dropStat[2][3], m_dropStat[2][4]);

        // 头部计数区整体清零（ORIG 0x08534432 `mov 0x8(%ebp),%eax` 先行物化
        // arg1，再压 0x3c0/0 ⇒ 实参必须写成员数组 m_head（非 `this`），
        // 否则 GCC 把 arg1 计算延到最后一条 store，指令序与 ORIG 不同）。
        std::memset(m_head, 0, 0x3c0);
    }
}

// ============================================================================
// WongWork::CItemUpgradeTable —— upgrade_table_t 非虚包装（sizeof == 0x4e4）
// ============================================================================

// ORIG W 0x0854b496 C1/C2：仅尾调 upgrade_table_t::C1(0x089000ca)，由编译器
// 生成的基类构造调用即等价，函数体留空并由基类构造完成初始化。
#include "W6_ItemGenDeps/CItemUpgradeTable.inc"
    : upgrade_table_t()
{
}

// ORIG W 0x0854b4aa D1/D2：仅尾调 upgrade_table_tD1(0x854b3f6)。
#include "W6_ItemGenDeps/dtor_CItemUpgradeTable.inc"
{
}

// ORIG W 0x0854b382 _ZNK8WongWork17CItemUpgradeTable19getUpgradeInfoTableEv：
// 仅返回 this（mov 0x8(%ebp),%eax; ret）。
#include "W6_ItemGenDeps/getUpgradeInfoTable.inc"
{
    return this;
}

// ORIG W 0x0854b38a _ZN8WongWork17CItemUpgradeTable17loadUpgradeScriptEPKc：
// 尾调 importUpgradeTable(path, this) 并透传返回值（错误行号，0=成功）。
#include "W6_ItemGenDeps/loadUpgradeScript.inc"
{
    return importUpgradeTable(path, this);
}

}  // namespace WongWork
