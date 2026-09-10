// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_UpdatePurchaseCashItemBonus/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ============================================================================
// DB_UpdatePurchaseCashItemBonus::makeRequest @ 0x0843e560
// mangled _ZN30DB_UpdatePurchaseCashItemBonus11makeRequestEij
// ============================================================================
#include "DB_UpdatePurchaseCashItemBonus/makeRequest.inc"
}

// ============================================================================
// DB_UpdatePurchaseCashItemBonus::dispatch @ 0x0843e252
// mangled _ZN30DB_UpdatePurchaseCashItemBonus8dispatchEiiP6Stream
// ============================================================================
// 逐条对照 ORIG 0x0843e252..0x0843e55e：槽图 db(-0x10)/mId(-0x14)/tableStr(-0x18)/
// purchaseCnt(-0x1c)/sig(-0xc)/guard(-0x24)。三处 set_query 的 toTString 均为 fresh
// 调用（ORIG 0x0843e2da/e34f/e3b6 三次 `lea -0x18; call toTString`，禁缓存）；
// Acquire 返回值直传 guard ctor（P259，ORIG 0x0843e47e→e495 无 stream 槽）；
// GetInBuffer 经 operator-> 链（P103，ORIG 0x0843e4d5 `call ptEv`）。
#include "DB_UpdatePurchaseCashItemBonus/dispatch.inc"
}
