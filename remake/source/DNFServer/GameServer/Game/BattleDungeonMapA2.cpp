#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BattleDungeonMapA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CWorldMap.h"
#include "GlobalData.h"
#include "DataManagerInitDepends.h"  // TOD_UserState/TOD_Layer（MakePacket 0x1c 当日层链）
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "PacketGuard.h"

// ============================ CBattle_Field ============================

#include "BattleDungeonMapA2/drop_item.inc"


#include "BattleDungeonMapA2/get_field_item.inc"


#include "BattleDungeonMapA2/is_last_field_item.inc"


#include "BattleDungeonMapA2/check_random_appear_hell_dungeon.inc"


#include "BattleDungeonMapA2/SelectDungeon.inc"


#include "BattleDungeonMapA2/MakePacket.inc"

// ============================== CDungeon ==============================

#include "BattleDungeonMapA2/InsertMap.inc"


#include "BattleDungeonMapA2/insertStart.inc"


#include "BattleDungeonMapA2/IsOpenScheduleDungeon.inc"

