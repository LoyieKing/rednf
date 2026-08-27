// ============================================================================
// W6_ItemGenDeps.cpp — Wave6 物品生成表域残余 ORIG 符号（WongWork 批）
//
// 本 TU 独占实现：
//   WongWork::CItemGeneratorMgr::onTimerEvent(int)      T 0x08534322 (0x133)
//   DB_UpdateItemDropStatistics::makeRequest(...)       T 0x0842894c (0x1d9)
//       —— onTimerEvent 唯一被调依赖（原源码无声明/定义，按同类
//          DB_U3.cpp 范式补真实转写，报告
//          docs/class_func_reports/DB_UpdateItemDropStatistics/makeRequest.md）。
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
#include "GlobalData.h"        // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"

#include <cstring>

// ============================================================================
// DB_UpdateItemDropStatistics —— 投递统计落库请求（opcode 0xd2）
//   stream: 0xd2 | -1 | serverId(int) | typeId(int) | p3[0..5] | p4[0..5]
//           | p5[0..5] | p6[0..5] → MsgQueueMgr::put(DB_Q)
// ============================================================================
class DB_UpdateItemDropStatistics
{
public:
    static void makeRequest(int serverId, int typeId,
                            unsigned int* counts0, unsigned int* counts1,
                            unsigned int* counts2, unsigned int* counts3);
};

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
    guard.operator*()->operator<<(serverId);  // ORIG 实参 = *(int*)(G_CEnvironment()+0x1b0)
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

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
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
    if (timer != 0x6b)
        return;

    // ORIG 每次调用独立 call G_CEnvironment()(0x080cc181) 读 +0x1b0
    // （服务器/环境 int，未建模字段，保持偏移读取）；四组统计指针均指向
    // m_head 区（未建模区），按原始偏移传参对齐汇编 add $imm,%eax 序列。
    char* base = reinterpret_cast<char*>(this);

    DB_UpdateItemDropStatistics::makeRequest(
        *reinterpret_cast<int*>(G_CEnvironment() + 0x1b0), 0,
        reinterpret_cast<unsigned int*>(base + 0x18),
        reinterpret_cast<unsigned int*>(base + 0x30),
        reinterpret_cast<unsigned int*>(base + 0x48),
        reinterpret_cast<unsigned int*>(base + 0x60));

    DB_UpdateItemDropStatistics::makeRequest(
        *reinterpret_cast<int*>(G_CEnvironment() + 0x1b0), 1,
        reinterpret_cast<unsigned int*>(base + 0x90),
        reinterpret_cast<unsigned int*>(base + 0xa8),
        reinterpret_cast<unsigned int*>(base + 0xc0),
        reinterpret_cast<unsigned int*>(base + 0xd8));

    DB_UpdateItemDropStatistics::makeRequest(
        *reinterpret_cast<int*>(G_CEnvironment() + 0x1b0), 2,
        reinterpret_cast<unsigned int*>(base + 0x108),
        reinterpret_cast<unsigned int*>(base + 0x120),
        reinterpret_cast<unsigned int*>(base + 0x138),
        reinterpret_cast<unsigned int*>(base + 0x150));

    std::memset(this, 0, 0x3c0);
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
int CItemUpgradeTable::loadUpgradeScript(const char* path)
{
    return importUpgradeTable(path, this);
}

}  // namespace WongWork
