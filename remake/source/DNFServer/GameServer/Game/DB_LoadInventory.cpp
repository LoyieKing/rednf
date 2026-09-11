// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_LoadInventory/<Func>.inc（不参与编译、内部不含 #include）。
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
#include <stdio.h>
#include <vector>

#include "DB_LoadInventory.h"

#include "MySQL.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"          // cMyTrace
#include "LogManager.h"         // logFormat
#include "AdvanceAltarZip.h"    // uncompress_zip 权威声明
#include "Inven_Item.h"
#include "GamePODTypes.h"      // SIG_CREATURE_ITEM_DETAIL_INFO/AvatarDetailInfo 权威
                              // 轻量叶子头（ORIG 本 TU 无 ios_base static-init）
#include "CItemAmplifier.h"     // stAmplifyOption_t::SetLock/GetLock
#include "W8LT_DBTask.h"        // DB_SwapCreatureItem 权威声明（makeRequest）

#include "DB_LoadInventory/get_compressed_blob_data.inc"
                              int recordSize);

extern char* NumberToString(unsigned int value, int radix);  // C++ linkage

#include "DB_LoadInventory/DB_LoadInventory.inc"


#include "DB_LoadInventory/dtor_DB_LoadInventory.inc"


#include "DB_LoadInventory/dispatch.inc"


#include "DB_LoadInventory/SendResult.inc"


#include "DB_LoadInventory/SendResult_forchina.inc"


#include "DB_LoadInventory/GetUserInven.inc"


#include "DB_LoadInventory/GetUserAvatar.inc"


#include "DB_LoadInventory/_GetUserAvatar.inc"


#include "DB_LoadInventory/GetUserCreature.inc"


#include "DB_LoadInventory/_getCharacInvenExpand.inc"


#include "DB_LoadInventory/_GetTagCharacInven.inc"


#include "DB_LoadInventory/_GetEquipMaxItemLevel.inc"


#include "DB_LoadInventory/GetItemLock.inc"
