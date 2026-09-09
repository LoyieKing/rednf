// ============================================================================
// W6_ItemGenDeps.cpp — Wave6 物品生成表域残余 ORIG 符号（WongWork 批）
//
// 本 TU 独占实现：
//   WongWork::CItemGeneratorMgr::onTimerEvent(int)      T 0x08534322 (0x133)
//   DB_UpdateItemDropStatistics::makeRequest(...)       T 0x0842894c (0x1d9)
//       —— onTimerEvent 唯一被调依赖；类定义唯一声明点在 W8LT_DBTask.h
//          （§4.4/P104 ODR 修复，本 TU 旧局部无基类声明已删），本 TU 仅提供
//          makeRequest 定义。报告
//          docs/class_func_reports/DB_UpdateItemDropStatistics/makeRequest.md。
//   WongWork::CItemUpgradeTable::{
//         CItemUpgradeTable C1/C2                       W 0x0854b496
//         ~CItemUpgradeTable D1/D2                      W 0x0854b4aa
//         getUpgradeInfoTable() const                   W 0x0854b382 }
//
// 另：WongWork::CItemGenRateTable::_itemTypeVerify 与
//     CSecialIMonsterDropItemGenRateTable::_itemTypeVerify 已由 DropLayoutDeps.cpp
//     落地（T 0x08534906 / 0x085352a6），本 TU 不重复定义。
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
void DB_UpdateItemDropStatistics::makeRequest(int serverId, int typeId,
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
void CItemGeneratorMgr::onTimerEvent(int timer)
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
CItemUpgradeTable::CItemUpgradeTable()
    : upgrade_table_t()
{
}

// ORIG W 0x0854b4aa D1/D2：仅尾调 upgrade_table_tD1(0x854b3f6)。
CItemUpgradeTable::~CItemUpgradeTable()
{
}

// ORIG W 0x0854b382 _ZNK8WongWork17CItemUpgradeTable19getUpgradeInfoTableEv：
// 仅返回 this（mov 0x8(%ebp),%eax; ret）。
const upgrade_table_t* CItemUpgradeTable::getUpgradeInfoTable() const
{
    return this;
}

// ORIG W 0x0854b38a _ZN8WongWork17CItemUpgradeTable17loadUpgradeScriptEPKc：
// 尾调 importUpgradeTable(path, this) 并透传返回值（错误行号，0=成功）。
bool CItemUpgradeTable::loadUpgradeScript(const char* path)
{
    return importUpgradeTable(path, this);
}

}  // namespace WongWork
