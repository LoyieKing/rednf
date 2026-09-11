#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_LoadCharac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>
#include <string>

#include "DB_LoadCharac.h"

#include "SigTypes_C.h"       // SIG_LOAD_CHARAC / TAG_CHARAC_DATA / CHARAC_DATA
#include "MySQL.h"
#include "GlobalData.h"        // s_db_mgr / s_stream_pool / s_msgq_mgr
#include "DBMgr.h"             // GetDBHandle
#include "Stream.h"            // Stream::GetOutBuffer 模板
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"         // cMyTrace
#include "CDataManager.h"      // G_CDataManager / get_event_script_mng
#include "CEventScriptMng.h"   // EventClassify::CEventScriptMng（is_eventing/get_db_table_name）
#include "GameWorld.h"         // G_GameWorld()->IsSchoolPvPChannel
#include "SyncScriptDeps.h"    // toTString(const std::string&)

// ORIG T _Z14NumberToStringji @0x0810904b（C++ linkage；定义归既有批次）。
extern char* NumberToString(unsigned int value, int radix);

#include "DB_LoadCharac/DB_LoadCharac.inc"


#include "DB_LoadCharac/dtor_DB_LoadCharac.inc"


#include "DB_LoadCharac/_getGameDB.inc"


#include "DB_LoadCharac/_getCharacInfo.inc"


#include "DB_LoadCharac/_getCharacStat.inc"


#include "DB_LoadCharac/_GetTagCharacInfo.inc"


#include "DB_LoadCharac/_UpdateSchoolCharac.inc"


#include "DB_LoadCharac/_LoadEventBringUpInfo.inc"


#include "DB_LoadCharac/_sendResult.inc"


#include "DB_LoadCharac/dispatch.inc"

