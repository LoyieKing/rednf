// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InterSelectCreateDnfEventInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— InterSelectCreateDnfEventInfo（台湾内部事件分发器）。
// 依据报告：
//   makeRequest  docs/class_func_reports/InterSelectCreateDnfEventInfo/makeRequest.md（0x8160ef6）
//   dispatch_sig docs/class_func_reports/InterSelectCreateDnfEventInfo/dispatch_sig.md（0x8160d5e）
//   Reward       docs/class_func_reports/InterSelectCreateDnfEventInfo__Reward/（0x8160c4a / 0x8160c6e）
//   C1/D1/D0     objdump 0x816201a/0x816203e/0x816206e（无报告，nm 定址转写）
// 依赖：TaiwanInterfaceInterDispatcher（基类）实现由 OrigTaiwanDisp 落；
// CEventCreateDnf / CEventManager / MsgQueueMgr / StreamPool 已有权威实现。
#include "InterSelectCreateDnfEventInfo.h"

#include "CInventory.h"      // PacketGuard 完整定义（含 InterfacePacketBuf 基础方法）
#include "CUser.h"
#include "CStreamGuard.h"
#include "TaiwanInternal.h"  // Taiwan::internal_stream
#include "GlobalData.h"      // s_stream_pool / s_msgq_mgr / s_event_manager
#include "MsgQueueMgr.h"
#include "CGuildServerProxy.h"  // CEventManager（GetRepeatEvent）
#include "CEventCreateDnf.h"
#include "GameWorld.h"       // ProcessAllUsers 模板 + G_GameWorld()
#include "LogManager.h"

#include "InterSelectCreateDnfEventInfo/Reward.inc"


#include "InterSelectCreateDnfEventInfo/operator.inc"


#include "InterSelectCreateDnfEventInfo/InterSelectCreateDnfEventInfo.inc"


#include "InterSelectCreateDnfEventInfo/dtor_InterSelectCreateDnfEventInfo.inc"


// ORIG 0x816206e：call D1 + operator delete。
// （编译器生成 D0，无需手写。）

#include "InterSelectCreateDnfEventInfo/makeRequest.inc"


#include "InterSelectCreateDnfEventInfo/dispatch_sig.inc"


#include "InterSelectCreateDnfEventInfo/GetState.inc"

