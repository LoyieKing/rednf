// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 sync_script/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "sync_script.h"

#include <stdio.h>
#include <string.h>

#include "DBMgr.h"
#include "GlobalData.h"
#include "MySQL.h"
#include "STSkillScript.h"
#include "DNFLexWrapper.h"
#include "DNFFunctionLib.h"
#include "CStreamBuffer.h"   // 权威 CStreamBuffer（ORIG 0x086269be 域，全局 W 符号）

// ---- ORIG 编码转换 / 脚本解析辅助（真实符号） ----
// toMbcs/getCharacterJob 声明唯一出处：SyncScriptDeps.h（sync_script.h 尾部已 include）。

#include "CDataManager.h"   // G_CDataManager()->m_randomOption 具名访问（CDataManager.h:3081，ORIG +0x4e10）

// ---- ORIG 全局数据（唯一定义点在 InitScriptProducers_3.cpp，ORIG B 0x095000a0）----
// insert_old_equip_info_to_db 的 ORIG 0x08624d5d 直接 `movl $&g_equipmentStatInfoMap`
// 遍历该全局（成员 m_oldEquipMap 全仓无填充点 ⇒ 旧实现恒空，属数据源真 bug）。
// 定义方以 C 链接发射未修饰符号名（C++ 全局变量名本就不入 mangling），
// 此处按权威类型作 C++ 声明引用（非 extern "C" 桥，§4.5 合规）。
extern std::map<unsigned int, STEquipmentStatInfo> g_equipmentStatInfoMap;

namespace
{
// [AnonLane round8 2026-09-11] 原 3 个 TU-local helper（`_ZN12_GLOBAL__N_1{12GetJobString,
// 19GetAttachTypeString,22GetAttackElementString}Ei`，t 段各 0x56/0x41/0x3c 字节）已删：
// ORIG 全库**无同名符号**（nm -S --defined-only df_game_r 对 jobstring/attachtype/
// attackelement 零命中），且本 TU 内（本 .cpp + 全部 sync_script/*.inc）**零调用点**
// ⇒ P154「无引用发明符号直接删」。三表内容（EUC-KR 作业/属性/元素名）与 ORIG
// GetUserJob 0x0861d2dc 的常量表同源；如需复用请引用具名权威表，勿再造 TU-local 包装。


}

// ===================== CSyncScript 实现 =====================

namespace sync_script
{

// [B01 铁律1] 原此处 4 个 `g_*InfoTable` 为**发明符号**（ORIG nm 无此名）+ 与前缀
// `_ZN11sync_script18g_dungeonInfoTableE` 形 GI 键名一并删除。ORIG 真名与定义点：
//   g_stDungeonScriptFileList        B 0x94f6ea0 (0x18)  → InitScriptProducers_4.cpp:156
//   g_stMonsterScriptFileList        B 0x94ff260 (0x18)  → InitScriptProducers_4.cpp:499
//   g_questScriptFileList            B 0x9500480 (0x18)  → InitScriptProducers_4.cpp:157
//   g_staSkillScriptFileList[11]     B 0x9500820 (0x108) → InitScriptProducers_4.cpp:185
// 全仓消费点（CListTables.cpp:56-59/460/778/1082/1710、GameMain.cpp:1826-1828、
// STDungeonScript.cpp:23、STMonsterScript.cpp:46、STQuestScript.cpp:63、STSkillScript.cpp:34）
// 早已按真名引用，本 4 名 0 引用（841 .o 的 `nm -u` 扫描零命中）⇒ 删定义零链接影响。

// ---- CStringMaker 实现（ORIG 各符号均为 W 内联；唯一定义发射点） ----

#include "sync_script/CStringMaker.inc"
#include "sync_script/CStringMaker_1.inc"

#include "sync_script/dtor_CStringMaker.inc"

#include "sync_script/operator.inc"


#include "sync_script/operator_1.inc"


#include "sync_script/operator_2.inc"


#include "sync_script/operator_3.inc"


#include "sync_script/operator_4.inc"


#include "sync_script/operator_5.inc"


#include "sync_script/operator_6.inc"


#include "sync_script/c_str.inc"


#include "sync_script/string.inc"


// CSyncScript 构造/析构由 CItemList.cpp（ORIG 0x861cbb4 区）提供；本 TU 不重复定义。

#include "sync_script/GetType.inc"


#include "sync_script/GetURL.inc"


#include "sync_script/GetUserJob.inc"


#include "sync_script/insert_artifact_info_to_db.inc"


#include "sync_script/GetAttachType.inc"


#include "sync_script/GetAttackElement.inc"


#include "sync_script/GetLevelUpSkill.inc"


#include "sync_script/GetUsableCharacter.inc"


#include "sync_script/GetUsableCharacter_1.inc"


#include "sync_script/LoadItemInfoMasterScript.inc"


#include "sync_script/insert_skill_index.inc"


#include "sync_script/truncate_skill_index.inc"


#include "sync_script/trancate_skill_info.inc"


#include "sync_script/insert_skill_info.inc"


#include "sync_script/insert_quest_category.inc"


#include "sync_script/truncate_quest_category.inc"


#include "sync_script/insert_charac_quest_category.inc"


#include "sync_script/truncate_charac_quest_ref.inc"


#include "sync_script/insert_exp_level_to_db.inc"


#include "sync_script/truncate_exp_level_ref_table.inc"


#include "sync_script/insert_item_parts_set.inc"


#include "sync_script/trancate_item_parts_set.inc"


#include "sync_script/insert_aicharacter_info_to_db.inc"


#include "sync_script/truncate_aicharacter_info_table.inc"


#include "sync_script/insert_town_info_to_db.inc"


#include "sync_script/truncate_town_info_table.inc"


#include "sync_script/insert_monster_info_to_db.inc"


#include "sync_script/truncate_monster_info_table.inc"


#include "sync_script/insert_creature_info_to_db.inc"


#include "sync_script/truncate_creature_info_table.inc"


#include "sync_script/insert_one_item_info_master.inc"


#include "sync_script/insert_item_info_master_to_db.inc"


#include "sync_script/truncate_item_info_master_table.inc"


#include "sync_script/insert_old_equip_info_to_db.inc"


#include "sync_script/truncate_old_equip_info_table.inc"


#include "sync_script/insert_mapping_equip_info_to_db.inc"


#include "sync_script/truncate_mapping_equip_info_table.inc"


#include "sync_script/insert_random_option_ref.inc"


#include "sync_script/truncate_random_option_ref.inc"


#include "sync_script/insert_charac_action_point_desc.inc"


#include "sync_script/truncate_charac_action_point_desc.inc"


#include "sync_script/insert_charac_advance_altar_item_desc.inc"


#include "sync_script/truncate_charac_advance_altar_item_desc.inc"


}  // namespace sync_script
