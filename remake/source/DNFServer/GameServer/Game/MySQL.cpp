// ===== [SplitC batch9 · A 手法合并] holder=MySQL.cpp ← T=CSlotBoundChecker.cpp（T 内容追加末尾；原 T 已从 CMake 移除）=====
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MySQL/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN5MySQLC2Ev`（ORIG 0x83f52ff）。
#include <iostream>

#include "LogManager.h"
// df_game_r 还原 —— MySQL（D4 批次）
// 参照 docs/class_func_reports/MySQL.md；源文件 DBMgr.cpp（ORIG 日志文件名）。
#include "MySQL.h"

#include <cstdarg>
#include <stdio.h>
#include <string.h>

#include "CQueryCounter.h"
#include "CUnixTimer.h"
#include "GameBase.h"

// ---- cMyTraceNoop（禁用追踪，ORIG 0x822ad5a/0x822ad60，属日志族 TU）----
class cMyTraceNoop
{
public:
    cMyTraceNoop();
    void operator()(const char* fmt, ...);
};
#include "MySQL/MySQL.inc"


#include "MySQL/SetLastTime.inc"
#include "MySQL/GetTimeInterval.inc"
#include "MySQL/CreateDnFTimer.inc"
#include "MySQL/DestroyDnFTimer.inc"


#include "MySQL/dtor_MySQL.inc"


#include "MySQL/set_compress_option.inc"


#include "MySQL/set_read_default_grp_option.inc"


#include "MySQL/set_charset_name_option.inc"


#include "MySQL/set_reconnect_option.inc"


#include "MySQL/init_db_handle.inc"


#include "MySQL/init.inc"


#include "MySQL/close.inc"


#include "MySQL/open.inc"


#include "MySQL/open_1.inc"


#include "MySQL/set_query.inc"


#include "MySQL/getAffectedRowCount.inc"


#include "MySQL/exec.inc"


#include "MySQL/fetch.inc"


#include "MySQL/ping.inc"


#include "MySQL/blob_to_str.inc"


#include "MySQL/getServerVersion.inc"


#include "MySQL/getClientVersion.inc"


#include "MySQL/select.inc"


#include "MySQL/select_1.inc"


#include "MySQL/insert.inc"


#include "MySQL/update.inc"


#include "MySQL/set_query_1.inc"


#include "MySQL/escape_string.inc"


#include "MySQL/clear_result_set.inc"


#include "MySQL/exec_query.inc"


// ============================================================================
// GetIdentityFromDB（ORIG 0x83fbc66 T）
// 包装 WongWork::DBCommon::GetIdentity（0x83f9ad4）：查询 @@identity（上次
// INSERT 的自增 ID），供 CQueryCounter::insertCounter 取 q_id 使用。
// ORIG：call WongWork::DBCommon::GetIdentity → leave/ret。
// ============================================================================
namespace WongWork {

class DBCommon
{
public:
    static unsigned int GetIdentity(MySQL* db);  // ORIG T 0x083f9ad4
};

}  // namespace WongWork

#include "MySQL/GetIdentityFromDB.inc"



// ===== [merged from CSlotBoundChecker.cpp] =====
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSlotBoundChecker/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CSlotBoundChecker 实现（独立 TU，从 GameStubs.cpp 拆出）。
// ORIG 符号（nm -C 核对）：C1 0x837355a（map C1 + init）、D1 0x837388e（map D1）、
//   init 0x8373674、insert 0x837359c、get_slot_bound 0x80f1096、get_item_type 0x850d1a0。
// 唯一成员 map<SlotBoundKey,SlotBound>（sizeof 0x18）；键/值权威结构在 CDataManager.h。
// g_slotBoundChecker 为全局对象（ORIG 0x8500384 以取地址压栈作 this 实证）。
#include "CInventory.h"

CSlotBoundChecker g_slotBoundChecker;

#include "CSlotBoundChecker/init.inc"


#include "CSlotBoundChecker/insert.inc"


#include "CSlotBoundChecker/get_slot_bound.inc"


#include "CSlotBoundChecker/get_item_type.inc"

