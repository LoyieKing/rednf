#ifndef GAME_SYNC_SCRIPT_H_
#define GAME_SYNC_SCRIPT_H_

// ============================================================================
// df_game_r 还原 —— sync_script::CSyncScript（G5 数据/脚本域）
// 物品相关方法由 CItemList.cpp 实现（其私有声明与本头布局一致）；
// 本头供 sync_script.cpp 补齐 skill/quest/dungeon/town 等剩余方法。
// 依赖的 ST* 外部类型：EquipmentParameterInfo/STEquipmentScript 使用权威头
// （EquipmentParameterInfo.h/STEquipmentScript.h，经 CItemAmplifier.h 链已
// 进入本 TU，本地镜像会重定义）；其余无权威头的类型仍以最小镜像声明。
// ============================================================================

#include <map>
#include <string>
#include <vector>

#include "Quest.h"
#include "STQuestScript.h"

class MySQL;
class STSkillScript;

// ---- 权威类型（唯一声明点收敛：本地镜像已删除，改引权威头）----
#include "EquipmentParameterInfo.h"
#include "STEquipmentScript.h"
#include "STStackableScript.h"   // STStackableScript/STStackablePackageData 权威
#include "CreatureScript.h"      // CreatureScript 权威（importCreatureScript.md 布局）


class STAICharacterCommonScript
{
public:
    int m_field0;        // +0x00
    std::string m_str4;  // +0x04
};


class STEquipmentPartSetPiece
{
public:
    // ORIG C1 0x0891afd0：string 默认构造 + 三个 int 置 -1。
    STEquipmentPartSetPiece() : m_field4(-1), m_field8(-1), m_fieldc(-1) {}

    std::string m_name;  // +0x00
    int m_field4;        // +0x04（getSameStringIndex(characterEquipmentTypeTags) 结果）
    int m_field8;        // +0x08（ScanInt）
    int m_fieldc;        // +0x0c（ScanInt）
};

class STEquipmentStatInfo
{
public:
    int m_hpMax;            // +0x00
    int m_mpMax;            // +0x04
    int m_phyAtt;           // +0x08
    int m_phyDef;           // +0x0c
    int m_magAtt;           // +0x10
    int m_magDef;           // +0x14
    int m_equipPhyAtt;      // +0x18
    int m_equipPhyDef;      // +0x1c
    int m_equipMagAtt;      // +0x20
    int m_equipMagDef;      // +0x24
    int m_refFire;          // +0x28
    int m_refWater;         // +0x2c
    int m_refDark;          // +0x30
    int m_refLight;         // +0x34
    int m_refAll;           // +0x38
    int m_refSlow;          // +0x3c
    int m_refFreeze;        // +0x40
    int m_refPoison;        // +0x44
    int m_refStun;          // +0x48
    int m_refCurse;         // +0x4c
    int m_refBlind;         // +0x50
    int m_refLightning;     // +0x54
    int m_refStone;         // +0x58
    int m_refSleep;         // +0x5c
    int m_refBurn;          // +0x60
    int m_refWeaponBreak;   // +0x64
    int m_refBleeding;      // +0x68
    int m_refPierce;        // +0x6c
    int m_refStuck;         // +0x70
    int m_refConfuse;       // +0x74
    int m_refHold;          // +0x78
    int m_refArmorBreak;    // +0x7c
    int m_refAllState;      // +0x80
};

class STMonsterScript
{
public:
    int m_index;         // +0x00（真实布局权威定义见 STMonsterScript.h，
                         //  此处仅 DB 入库所需最小字段视图，名称与权威一致）
    std::string m_name;  // +0x004 [name]
};

namespace sync_script
{

class CSyncScript
{
public:
    CSyncScript();
    ~CSyncScript();

    // ---- 物品相关（CItemList.cpp 实现，此处声明） ----
    bool truncate_dnf_item_info_table();
    bool insert_equiptment_item_to_db(STEquipmentScript* script,
                                      unsigned int idx, int mode, int key);
    bool insert_stackable_item_to_db(STStackableScript* script);
    bool truncateCeraShopItemTable();
    bool insertCeraShopItemIntoDB(unsigned int itemIdx, unsigned int itemType,
                                  int value, int count, short c,
                                  const char* name, const char* iconName);
    bool truncate_artifact_info_table();
    bool insert_artifact_info_to_db(STEquipmentScript* script);

    // ---- G5 剩余方法（本 TU 实现） ----
    bool GetType(std::string name, int job, int idx, int& out1, int& out2,
                 bool flag);
    const char* GetURL(const char* str, int len, int idx);
    const char* GetUserJob(int job);
    const char* GetAttachType(int type);
    const char* GetAttackElement(EquipmentParameterInfo* param);
    void GetLevelUpSkill(STEquipmentScript* script,
                         EquipmentParameterInfo::ENUM_SKILL_UP_VECTOR_INDEX vectorIndex);
    const char* GetUsableCharacter(STEquipmentScript* script);
    const char* GetUsableCharacter(STStackableScript* script);
    bool LoadItemInfoMasterScript(const char* path);

    bool insert_skill_index(STSkillScript* script, int job);
    bool truncate_skill_index();
    bool trancate_skill_info();
    bool insert_skill_info(STSkillScript* script, int job, int type);
    bool insert_quest_category(QuestScript* script);
    bool truncate_quest_category();
    bool insert_charac_quest_category(unsigned int origin, unsigned int mapped);
    bool truncate_charac_quest_ref();
    bool insert_exp_level_to_db(unsigned int exp, int level);
    bool truncate_exp_level_ref_table();
    bool insert_item_parts_set(int index, STEquipmentPartSetPiece& piece);
    bool trancate_item_parts_set();
    bool insert_aicharacter_info_to_db(int idx,
                                       const STAICharacterCommonScript& script);
    bool truncate_aicharacter_info_table();
    bool insert_town_info_to_db(const TownScript& script);
    bool truncate_town_info_table();
    bool insert_monster_info_to_db(const STMonsterScript& script);
    bool truncate_monster_info_table();
    bool insert_creature_info_to_db(unsigned int id,
                                    const CreatureScript* script,
                                    const char* param3, const char* param4);
    bool truncate_creature_info_table();
    bool insert_one_item_info_master(unsigned char masterNo,
                                     unsigned short subNo,
                                     const char* name, const char* explain);
    bool insert_item_info_master_to_db();
    bool truncate_item_info_master_table();
    bool insert_old_equip_info_to_db();
    bool truncate_old_equip_info_table();
    bool insert_mapping_equip_info_to_db(std::map<int, int>& mapping);
    bool truncate_mapping_equip_info_table();
    bool insert_random_option_ref();
    bool truncate_random_option_ref();
    bool insert_charac_action_point_desc();
    bool truncate_charac_action_point_desc();
    bool insert_charac_advance_altar_item_desc();
    bool truncate_charac_advance_altar_item_desc();

    void SetEnable(bool flag) { m_field0 = flag ? 1 : 0; }
    bool IsEnable() { return m_field0 == 1; }   // ORIG _ZN11sync_script11CSyncScript8IsEnableEv（非 const）

    // ---- 成员布局（与 CItemList.cpp 私有声明一致） ----
    struct SITEMTYPE
    {
        SITEMTYPE() : m_field0(0), m_field1(0), m_field2(0), m_field4(0) {}
        SITEMTYPE(bool usable, unsigned char f1, unsigned char f2,
                  unsigned short f4, const std::string& name)
            : m_field0(usable ? 1 : 0), m_field1(f1), m_field2(f2),
              m_field4(f4), m_str8(name)
        {
        }
        char m_field0;        // +0x00
        unsigned char m_field1;  // +0x01
        unsigned char m_field2;  // +0x02
        unsigned short m_field4; // +0x04
        std::string m_str8;      // +0x08
    };

    struct SWEAPONTYPE
    {
        int m_job;    // +0x00
        int m_idx;    // +0x04
        bool operator<(const SWEAPONTYPE& o) const
        {
            if (m_job < o.m_job)
            {
                return true;
            }
            if (m_job == o.m_job && m_idx < o.m_idx)
            {
                return true;
            }
            return false;
        }
    };

    struct SARMORTYPE
    {
        unsigned char m_field0;  // +0x00
        std::string m_str4;      // +0x04
        bool operator<(const SARMORTYPE& o) const
        {
            if (m_field0 < o.m_field0)
            {
                return true;
            }
            if (m_field0 == o.m_field0 && m_str4.compare(o.m_str4) < 0)
            {
                return true;
            }
            return false;
        }
    };


    bool m_field0;                            // +0x00
    std::vector<unsigned char> m_vec1;        // +0x04
    std::vector<unsigned char> m_vec2;        // +0x10
    std::map<std::string, SITEMTYPE> m_itemTypeMap;  // +0x1c
    std::map<SWEAPONTYPE, SITEMTYPE> m_weaponMap;    // +0x34
    std::map<SARMORTYPE, SITEMTYPE> m_armorMap;      // +0x4c
    std::map<unsigned char, std::string> m_map64;    // +0x64
    std::map<unsigned char, std::string> m_map7c;    // +0x7c
    std::map<unsigned int, STEquipmentStatInfo> m_oldEquipMap;  // +0x94
};

CSyncScript* GetInstanceSyncScript();

}  // namespace sync_script

// ---- 外部真实符号（统一声明点：SyncScriptDeps.h；G_CDataManager/RandomOptionScript
//      由 sync_script.cpp 按需声明，避免把 CItemList.h 链（STEquipmentScript.h 等）
//      拉入本头与本地镜像冲突） ----
#include "SyncScriptDeps.h"

#endif  // GAME_SYNC_SCRIPT_H_
