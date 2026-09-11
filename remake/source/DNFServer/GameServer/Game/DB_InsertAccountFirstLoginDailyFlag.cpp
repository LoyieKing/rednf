// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_InsertAccountFirstLoginDailyFlag/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_InsertAccountFirstLoginDailyFlag（账号每日首次登录标记 DB 请求）。
// 逐函数对照 docs/class_func_reports/DB_InsertAccountFirstLoginDailyFlag/。
// 语义：makeRequest（0x0843ea64）发表头 0x342 + 4 参数；dispatch（0x0843e8be）
// 先 UPDATE（affected==0 则 INSERT）。
#include "DB_InsertAccountFirstLoginDailyFlag.h"
// ============================================================================
// b28d-52 判词表（本 TU，唯一 DIFF）：
//   dispatch 0x0843e8be —— 已修：P48/P282 两处 fresh 虚调用 get_db_table_name
//     （vptr+0x20，旧版具名引用缓存致第二次虚调用缺失）+ P0 去 dm 局部 +
//     P171 照抄死写入 local_14 + p3 无预清。残差为早返块布局（P16/P222 方向；
//     ORIG `jne <后置失败块>` vs 4.4 `je <就地失败块>`）⇒ N（跨版本分支布局）。
// ============================================================================

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

#include "DB_InsertAccountFirstLoginDailyFlag/makeRequest.inc"


#include "DB_InsertAccountFirstLoginDailyFlag/dispatch.inc"

