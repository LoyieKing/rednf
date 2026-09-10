// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W6_ItemGenDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "W6_ItemGenDeps/makeRequest.inc"
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
}

// ============================================================================
// WongWork::CItemUpgradeTable —— upgrade_table_t 非虚包装（sizeof == 0x4e4）
// ============================================================================

// ORIG W 0x0854b496 C1/C2：仅尾调 upgrade_table_t::C1(0x089000ca)，由编译器
// 生成的基类构造调用即等价，函数体留空并由基类构造完成初始化。
#include "W6_ItemGenDeps/CItemUpgradeTable.inc"
}

// ORIG W 0x0854b4aa D1/D2：仅尾调 upgrade_table_tD1(0x854b3f6)。
#include "W6_ItemGenDeps/dtor_CItemUpgradeTable.inc"
}

// ORIG W 0x0854b382 _ZNK8WongWork17CItemUpgradeTable19getUpgradeInfoTableEv：
// 仅返回 this（mov 0x8(%ebp),%eax; ret）。
#include "W6_ItemGenDeps/getUpgradeInfoTable.inc"
}

// ORIG W 0x0854b38a _ZN8WongWork17CItemUpgradeTable17loadUpgradeScriptEPKc：
// 尾调 importUpgradeTable(path, this) 并透传返回值（错误行号，0=成功）。
#include "W6_ItemGenDeps/loadUpgradeScript.inc"
}

}  // namespace WongWork
