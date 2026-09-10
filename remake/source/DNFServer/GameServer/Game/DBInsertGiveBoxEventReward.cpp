// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DBInsertGiveBoxEventReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DBInsertGiveBoxEventReward::makeRequest（礼盒事件奖励请求）。
// ORIG 0x0815d2c8（mangled _ZN26DBInsertGiveBoxEventReward11makeRequestEP5CUser，
// 0x126 字节）：Acquire("DBThread.cpp",0x369) → CStreamGuard(stream,true) →
// Taiwan::internal_stream(guard, 0x10, user->GetUID()) → GetInBuffer<MSG>() 写
// m_accId/m_characNo → DB_Q put；缓冲为空走 cMyTrace。
#include "DBInsertGiveBoxEventReward.h"

#include "CUser.h"
#include "MoreTypes.h"       // DBInsertGiveBoxEventReward::MSG
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "TaiwanInternal.h"
#include "GameTypes.h"       // cMyTrace

// b28c-L20: AE — ORIG 0x0815d2c8 orig=77 our=77 gap=0 ndiff=0。两处修正：
//   ①Acquire 文件名串必须 "localtaiwan/Dispatcher/Taiwan_DbThread.cpp"（原 "DBThread.cpp" 为真 bug）；
//   ②cMyTrace 用 __PRETTY_FUNCTION__（P139）+ 全表达式临时链（P221/P227）：具名对象
//   使 guard(-0x24)/trace(-0x1c) 槽号互换（ndiff 16），临时链后槽深与 ORIG 一致。
#include "DBInsertGiveBoxEventReward/makeRequest.inc"
}
