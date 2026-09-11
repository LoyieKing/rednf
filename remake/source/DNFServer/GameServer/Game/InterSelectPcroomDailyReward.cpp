// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InterSelectPcroomDailyReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— InterSelectPcroomDailyReward（台湾内部 PC 房每日奖励分发）。
// 报告：
//   makeRequest  docs/class_func_reports/InterSelectPcroomDailyReward/makeRequest.md（0x816179c）
//   dispatch_sig docs/class_func_reports/InterSelectPcroomDailyReward/dispatch_sig.md（0x81616ce）
//   SendPacket  docs/class_func_reports/InterSelectPcroomDailyReward/SendPacket.md（0x81618a6）
// 依赖：TaiwanInterfaceInterDispatcher（基类）实现由 OrigTaiwanDisp 落；
// CStreamGuard/StreamPool/MsgQueueMgr/Taiwan::internal_stream 已有权威实现。
#include "InterSelectPcroomDailyReward.h"

#include "CInventory.h"   // PacketGuard 完整定义（含 InterfacePacketBuf 基础方法）
#include "CStreamGuard.h"
#include "CUser.h"
#include "GlobalData.h"   // s_stream_pool / s_msgq_mgr
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "TaiwanInternal.h"  // Taiwan::internal_stream

#include "InterSelectPcroomDailyReward/SendPacket.inc"


#include "InterSelectPcroomDailyReward/makeRequest.inc"


#include "InterSelectPcroomDailyReward/dispatch_sig.inc"

