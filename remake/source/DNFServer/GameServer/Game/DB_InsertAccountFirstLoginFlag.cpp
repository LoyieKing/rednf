// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_InsertAccountFirstLoginFlag/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_InsertAccountFirstLoginFlag（账号首次登录标记 DB 请求，DBThread 派发）。
// 逐函数对照 docs/class_func_reports/DB_InsertAccountFirstLoginFlag/。
// 语义：makeRequest（0x0843e79a）向 DB 队列发一条请求，表头 0x25e + 5 参数；
// dispatch（0x0843e636）读流并据此向事件表执行 "insert into ...(m_id,server_id,
// charac_no, occ_date) values(...)"。
#include "DB_InsertAccountFirstLoginFlag.h"

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CDataManager.h"
#include "CEventScriptMng.h"
#include "SyncScriptDeps.h"         // toTString(const std::string&)
#include "CInventory.h"
#include "Stream.h"

#include <cstdio>

// ============================================================================
// DB_InsertAccountFirstLoginFlag::makeRequest @ 0x0843e79a
// mangled _ZN30DB_InsertAccountFirstLoginFlag11makeRequestEijiji
// ============================================================================
#include "DB_InsertAccountFirstLoginFlag/makeRequest.inc"
}

// ============================================================================
// DB_InsertAccountFirstLoginFlag::dispatch @ 0x0843e636
// mangled _ZN30DB_InsertAccountFirstLoginFlag8dispatchEiiP6Stream
// ============================================================================
// 逐条对照 ORIG 0x0843e636..0x0843e799：槽图 db(-0x24)/m_id(-0x28)/characNo(-0x2c)/
// serverID(-0x30)/eventType(-0x34 未初始化)/sm(-0x20)/ent(-0x1c)；读取序
// m_id,serverID,characNo,eventType（lea 序 0x0843e674/e686/e698/e6aa）；条件形
// `if(A||B){...}else{return 0}`（ORIG 0x0843e6db je / 0x0843e6e5 jne 双落点）；
// set_query 中间值全部表达式内联（ORIG edi/esi/ebx 寄存器直传，无栈槽往返 ⇒ P0）；
// 尾形 `if(!exec)return 0; return 1;`（P101，ORIG 0x0843e77f xor/test/je）。
#include "DB_InsertAccountFirstLoginFlag/dispatch.inc"
}
