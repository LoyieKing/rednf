// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_UpdatePurchaseCashItemBonus/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_UpdatePurchaseCashItemBonus（商城消费奖励购买计数 DB
// 请求，DBThread 派发）。逐函数对照
// docs/class_func_reports/DB_UpdatePurchaseCashItemBonus/{makeRequest,dispatch}.md。
// 语义：makeRequest（0x0843e560）向 DB 队列发一条请求，表头 0x23c + m_id + acc_id；
// dispatch（0x0843e252）读流取 m_id 后对 0x19a 事件表执行
// "upDate %s set purchase_cnt = purchase_cnt + 1 where m_id=%s"（无行则 insert），
// 再 select 回读 purchase_cnt 经 DISPATCH_Q 回发游戏侧。
#include "DB_UpdatePurchaseCashItemBonus.h"

#include <string>

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CDataManager.h"
#include "CEventScriptMng.h"
#include "SigTypes.h"
#include "SkillSlot.h"          // NumberToString
#include "SyncScriptDeps.h"     // toTString
#include "Stream.h"

#include "DB_UpdatePurchaseCashItemBonus/makeRequest.inc"


#include "DB_UpdatePurchaseCashItemBonus/dispatch.inc"

