// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMonster/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r Game/ CMonster 实现。
#include <utility>
#include <vector>

#include "STMonsterScript.h"
#include "CMonster.h"


#include "CMonster/get_rand_int.inc"

#include "CMonster/_drop_item_index.inc"


#include "CMonster/CMonster.inc"


#include "CMonster/dtor_CMonster.inc"


#include "CMonster/get_index.inc"


#include "CMonster/getMonsterScript.inc"


#include "CMonster/set_monster.inc"


#include "CMonster/IsHellMonster.inc"


#include "CMonster/get_specialItem_list_size.inc"


#include "CMonster/deathTowerSpecifyItemDrop.inc"


#include "CMonster/drop_item_index.inc"


#include "CMonster/drop_item_index_1.inc"

