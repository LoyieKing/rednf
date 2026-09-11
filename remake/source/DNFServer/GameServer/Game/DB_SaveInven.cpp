// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_SaveInven/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "DB_SaveInven.h"
#include "ARAD_Database.h"          // ARAD::DATABASE::source*Size（各快照长度）

#include "GlobalData.h"             // s_db_mgr
#include "DBMgr.h"                  // DBMgr::GetDBHandle
#include "MySQL.h"                  // MySQL::set_query/exec/blob_to_str
#include "GameTypes.h"              // cMyTrace
#include "AdvanceAltarZip.h"        // compress_zip（唯一实现归 APSystemOnlyOrig.cpp）
#include "SigTypes.h"               // SIG_SAVE_INVENTORY
#include "Stream.h"                 // Stream::GetOutBuffer
#include "CEnvironment.h"           // DB_HANDLE_3 / E_SERVER_GROUP_NONE

#include <string.h>                 // memset

// NumberToString 权威声明在 CInventory.h（0x0810904b u32 版），本 TU 仅前向
// 声明同名全局函数，与权威签名一致（W8LT_DBTask.cpp 同款做法）。
char* NumberToString(unsigned int value, int radix);

#include "DB_SaveInven/DB_SaveInven.inc"


#include "DB_SaveInven/dtor_DB_SaveInven.inc"


#include "DB_SaveInven/SendResult.inc"


#include "DB_SaveInven/dispatch.inc"


#include "DB_SaveInven/_saveCharacInvenExpand.inc"


#include "DB_SaveInven/SaveInven.inc"
