#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BattleMonsterDepsC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h      // CBattle_Field + WarField/CMap/CDungeon/CParty/
                                // CMonster/STMonsterScript/GridScript/MazeScript 权威链
#include "CDataManager.h"       // G_CDataManager/find_*/get_hellparty_groupOrder_map/
                                // CAICharacterList（get/getByLevel 桥接声明）
#include "CItemGenerator.h"     // WongWork::stGenerateRefData_t/stGenerateResult_t/CItemGenerator
#include "CItemGeneratorMgr.h"  // WongWork::CItemGeneratorMgr::eGenerateType_t/getGenerator
#include "GlobalData.h"         // GlobalData::s_systemTime_
#include "MonsterIndependentDrop.h"  // CAICharacter 完整镜像（getLevel/getIdx 内联）

using WongWork::stGenerateRefData_t;
using WongWork::stGenerateResult_t;
using WongWork::CItemGenerator;
// ============================ CAICharacterList =============================

#include "BattleMonsterDepsC2/getByLevel.inc"


// ============================ CBattle_Field ================================

#include "BattleMonsterDepsC2/getMap.inc"


#include "BattleMonsterDepsC2/setDungeonClearTrace.inc"


#include "BattleMonsterDepsC2/onStartMap.inc"


#include "BattleMonsterDepsC2/get_monster_list.inc"


#include "BattleMonsterDepsC2/add_monster_list_bossroom_condition.inc"


#include "BattleMonsterDepsC2/add_monsters_current_map.inc"


#include "BattleMonsterDepsC2/add_monster_hell_party.inc"


#include "BattleMonsterDepsC2/add_monster_blood.inc"


#include "BattleMonsterDepsC2/add_monster_APC_AI.inc"


#include "BattleMonsterDepsC2/add_monster_APC_AI_by_rate.inc"


#include "BattleMonsterDepsC2/add_monster_additional_champion.inc"


#include "BattleMonsterDepsC2/add_monster_event.inc"


#include "BattleMonsterDepsC2/set_monster_ability.inc"


#include "BattleMonsterDepsC2/insert_tournamentRoundMonster.inc"


#include "BattleMonsterDepsC2/add_item_curr_map.inc"


// ---- moveMap_*_Log 族（0x0830766e / 0x08307740 / 0x08307812 / 0x083078e4）----

#include "BattleMonsterDepsC2/moveMap_1_Log.inc"


#include "BattleMonsterDepsC2/moveMap_2_Log.inc"


#include "BattleMonsterDepsC2/moveMap_3_Log.inc"


#include "BattleMonsterDepsC2/moveMap_4_Log.inc"


#include "BattleMonsterDepsC2/check_boss_map.inc"


#include "BattleMonsterDepsC2/check_movemap_range.inc"


#include "BattleMonsterDepsC2/get_invade_direction.inc"


#include "BattleMonsterDepsC2/check_visited_map.inc"


#include "BattleMonsterDepsC2/set_visited_map.inc"


#include "BattleMonsterDepsC2/set_visited_map_goto.inc"


#include "BattleMonsterDepsC2/choose_random_bossmap_from_dummymaps.inc"


#include "BattleMonsterDepsC2/get_candidator_map.inc"


#include "BattleMonsterDepsC2/set_map_id.inc"

