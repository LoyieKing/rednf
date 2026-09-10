// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 sync_script/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— sync_script::CSyncScript 剩余方法（G5 数据/脚本域）
// 物品相关方法（insert_equiptment_item_to_db / insert_stackable_item_to_db /
// truncate_dnf_item_info_table / insertCeraShopItemIntoDB 等）已由 CItemList.cpp
// 实现；本 TU 补齐 skill/quest/dungeon/town/exp/creature/aicharacter/
// item_master/random_option 等。逐函数对照
// docs/class_func_reports/sync_script__CSyncScript.md 的 C 代码实现。
// ---------------------------------------------------------------------------
// [b22判词·N族/索引伪差集 P93·P154 同族] __tcf_0（<global>）：本 TU 文件作用域带
// 非平凡析构全局对象的 GCC 合成 atexit cleanup，无独立可写源码函数体，指令完全由
// 本 TU 全局对象集合与构造序决定。ORIG 二进制内 `__tcf_0` 同名 local 数百枚（每含
// 全局对象 TU 一枚），compare 工具按名取到 0x0828f940（WongWork::CDeathTowerRanking
// 域另一 TU，与本 TU 全局无关）→ 逐条 DIFF 系索引伪差集（§6.3 撞名族），非实现缺口。
// 语义等价，不可源码复现，保留判词。
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
// 保留 ORIG 作业名（EUC-KR 字节，见 GetUserJob 0x861d2dc 常量表）。
const char* GetJobString(int job)
{
    switch (job)
    {
    case 0:
        return "\xb1\xcd\xb0\xcb\xbb\xe7";   // 귀검사
    case 1:
        return "\xb0\xdd\xc5\xf5\xb0\xa1";   // 격투가
    case 2:
        return "\xb0\xc5\xb3\xca";           // 거너
    case 3:
        return "\xb8\xb6\xb9\xfd\xbb\xe7";   // 마법사
    case 4:
        return "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";  // 프리스트
    case 5:
        return "\xbf\xa9\xb0\xc5\xb3\xca";   // 여거너
    case 6:
        return "\xb5\xb5\xc0\xfb";           // 도적
    case 7:
        return "\xb3\xb2\xb0\xdd\xc5\xf5\xb0\xa1";  // 남격투가
    default:
        return "";
    }
}

const char* GetAttachTypeString(int type)
{
    switch (type)
    {
    case 0:
        return "\xc0\xda\xc0\xaf";           // 검사
    case 1:
        return "\xb1\xb3\xc8\xaf";           // 격가
    case 2:
        return "\xb1\xb3\xc8\xaf\xbb\xe8\xc1\xa6";  // 격가프리
    case 3:
        return "\xb9\xd0\xba\xc0";           // 마공
    case 5:
        return "\xb0\xe8\xc1\xa4";           // 계열
    default:
        return "";
    }
}

const char* GetAttackElementString(int element)
{
    switch (element)
    {
    case 0:
        return "\xc8\xad";                   // 화
    case 1:
        return "\xbc\xf6";                   // 수
    case 2:
        return "\xbe\xcf";                   // 암
    case 3:
        return "\xb8\xed";                   // 명
    case 4:
        return "\xb9\xab";                   // 무
    default:
        return "";
    }
}


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
}
#include "sync_script/dtor_CStringMaker.inc"

#include "sync_script/operator.inc"
}

#include "sync_script/operator_1.inc"
}

#include "sync_script/operator_2.inc"
}

#include "sync_script/operator_3.inc"
}

#include "sync_script/operator_4.inc"
}

#include "sync_script/operator_5.inc"
}

#include "sync_script/operator_6.inc"
}

#include "sync_script/c_str.inc"
}

#include "sync_script/string.inc"
}

// CSyncScript 构造/析构由 CItemList.cpp（ORIG 0x861cbb4 区）提供；本 TU 不重复定义。

#include "sync_script/GetType.inc"
}

#include "sync_script/GetURL.inc"
}

#include "sync_script/GetUserJob.inc"
}

#include "sync_script/insert_artifact_info_to_db.inc"
}

#include "sync_script/GetAttachType.inc"
}

#include "sync_script/GetAttackElement.inc"
}

#include "sync_script/GetLevelUpSkill.inc"
}

#include "sync_script/GetUsableCharacter.inc"
}

#include "sync_script/GetUsableCharacter_1.inc"
}

#include "sync_script/LoadItemInfoMasterScript.inc"
}

#include "sync_script/insert_skill_index.inc"
}

#include "sync_script/truncate_skill_index.inc"
}

#include "sync_script/trancate_skill_info.inc"
}

#include "sync_script/insert_skill_info.inc"
#undef SKILL_ESC_SLOT
    db->set_query("inSert into skill_info(job_index, skill_index, module_type, name, name2, basic_explain, skill_explain, purchase_cost, type, skill_class, growtype_maximum_level, second_growtype_maximum_level, skill_fitness_growtype, skill_fitness_second_growtype, consume_item, required_level, required_level_range, pre_required_skill, consume_mp, cool_time, casting_time, icon, command_key_explain, skill_command_advantage, static_data, level_info, start_cool_time) values(%s)",
                  sb.c_str());
    return db->exec(true);
}

#include "sync_script/insert_quest_category.inc"
}

#include "sync_script/truncate_quest_category.inc"
}

#include "sync_script/insert_charac_quest_category.inc"
}

#include "sync_script/truncate_charac_quest_ref.inc"
}

#include "sync_script/insert_exp_level_to_db.inc"
}

#include "sync_script/truncate_exp_level_ref_table.inc"
}

#include "sync_script/insert_item_parts_set.inc"
}

#include "sync_script/trancate_item_parts_set.inc"
}

#include "sync_script/insert_aicharacter_info_to_db.inc"
}

#include "sync_script/truncate_aicharacter_info_table.inc"
}

#include "sync_script/insert_town_info_to_db.inc"
}

#include "sync_script/truncate_town_info_table.inc"
}

#include "sync_script/insert_monster_info_to_db.inc"
}

#include "sync_script/truncate_monster_info_table.inc"
}

#include "sync_script/insert_creature_info_to_db.inc"
}

#include "sync_script/truncate_creature_info_table.inc"
}

#include "sync_script/insert_one_item_info_master.inc"
}

#include "sync_script/insert_item_info_master_to_db.inc"
}

#include "sync_script/truncate_item_info_master_table.inc"
}

#include "sync_script/insert_old_equip_info_to_db.inc"
}

#include "sync_script/truncate_old_equip_info_table.inc"
}

#include "sync_script/insert_mapping_equip_info_to_db.inc"
}

#include "sync_script/truncate_mapping_equip_info_table.inc"
}

#include "sync_script/insert_random_option_ref.inc"
}

#include "sync_script/truncate_random_option_ref.inc"
}

#include "sync_script/insert_charac_action_point_desc.inc"
}

#include "sync_script/truncate_charac_action_point_desc.inc"
}

#include "sync_script/insert_charac_advance_altar_item_desc.inc"
}

#include "sync_script/truncate_charac_advance_altar_item_desc.inc"
}

}  // namespace sync_script
