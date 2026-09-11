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

#include "W6_ItemGenDeps/makeRequest.inc"


// ============================================================================
// WongWork::CItemGeneratorMgr::onTimerEvent(int)
//   ORIG T 0x08534322：报告 WongWork__CItemGeneratorMgr/onTimerEvent.md
//   逐段转写。timer==0x6b 时依次对三组统计区发 makeRequest 后 memset 清零；
//   其余直接返回。
// ============================================================================
namespace WongWork
{
#include "W6_ItemGenDeps/onTimerEvent.inc"


// ============================================================================
// WongWork::CItemUpgradeTable —— upgrade_table_t 非虚包装（sizeof == 0x4e4）
// ============================================================================

#include "W6_ItemGenDeps/CItemUpgradeTable.inc"


#include "W6_ItemGenDeps/dtor_CItemUpgradeTable.inc"


#include "W6_ItemGenDeps/getUpgradeInfoTable.inc"


#include "W6_ItemGenDeps/loadUpgradeScript.inc"


}  // namespace WongWork
