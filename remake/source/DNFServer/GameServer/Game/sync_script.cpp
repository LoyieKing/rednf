// ============================================================================
// df_game_r 还原 —— sync_script::CSyncScript 剩余方法（G5 数据/脚本域）
// 物品相关方法（insert_equiptment_item_to_db / insert_stackable_item_to_db /
// truncate_dnf_item_info_table / insertCeraShopItemIntoDB 等）已由 CItemList.cpp
// 实现；本 TU 补齐 skill/quest/dungeon/town/exp/creature/aicharacter/
// item_master/random_option 等。逐函数对照
// docs/class_func_reports/sync_script__CSyncScript.md 的 C 代码实现。
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

// ---- ORIG 编码转换 / 脚本解析辅助（真实符号） ----
// toMbcs/getCharacterJob 由 SyncScriptDeps.cpp 提供（ORIG 0x08adee98 / 0x088be57e）。
const char* toMbcs(const char* src);
int getCharacterJob(const char* name);

// G_CDataManager() / RandomOptionScript::getPrefix（真实符号，声明与权威头一致；
// 不 include CDataManager.h：其 CItemList.h 链会与 sync_script.h 本地 ST* 镜像
// 冲突——既有跨 TU 布局分歧，见遗留说明。此处为普通 C++ 声明，非 extern C 桥）。
class CDataManager;
CDataManager* G_CDataManager();
class RandomOptionScript
{
public:
    std::string getPrefix(int prefix, int grade, bool flag);  // ORIG 0x08a73956
};

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

// ---- CListTables.cpp / CDungeonList.cpp 引用的全局表（原 GameStubs.cpp 桩） ----
std::map<int, std::string> g_dungeonInfoTable;    // ORIG 0x94f6ea0
std::map<int, std::string> g_monsterInfoTable;    // ORIG 0x94ff260
std::map<int, std::string> g_questInfoTable;      // ORIG 0x9500480
std::map<int, std::string> g_skillInfoTable[11];  // ORIG 0x9500820

// CSyncScript 构造/析构由 CItemList.cpp（ORIG 0x861cbb4 区）提供；本 TU 不重复定义。

bool CSyncScript::GetType(std::string name, int job, int idx, int& out1,
                          int& out2, bool flag)
{
    if (name.empty())
    {
        return false;
    }
    if (name.find_first_of("[", 0) != 0)
    {
        return false;
    }
    name.erase(0, 1);
    name.erase(name.size() - 1, 1);
    std::string key = "stackable ";
    if (flag || name != "creature")
    {
        key += name;
    }
    else
    {
        key = name;
    }
    SITEMTYPE local = m_itemTypeMap[key];
    if (m_vec1.empty() || m_vec1[0] == local.m_field1)
    {
        SWEAPONTYPE wp;
        wp.m_job = job;
        wp.m_idx = idx;
        local = m_weaponMap[wp];
    }
    else if (m_vec1.size() >= 2 && m_vec1[1] == local.m_field1)
    {
        SARMORTYPE ap;
        ap.m_field0 = (unsigned char)idx;
        ap.m_str4 = name;
        local = m_armorMap[ap];
    }
    out1 = local.m_field1;
    out2 = local.m_field4;
    return true;
}

const char* CSyncScript::GetURL(const char* str, int len, int idx)
{
    static std::string strUrl;
    strUrl.clear();
    int start = 0;
    int end = 0;
    int i = 0;
    for (i = 0; i < len; ++i)
    {
        if (str[i] == '/')
        {
            start = i + 1;
        }
        if (str[i] == '.')
        {
            end = i;
        }
    }
    strUrl.insert(0, str + start, end - start);
    char tmp[16] = {0};
    sprintf(tmp, "/%05d.png", idx);
    strUrl += tmp;
    return strUrl.c_str();
}

const char* CSyncScript::GetUserJob(int job)
{
    return GetJobString(job);
}

const char* CSyncScript::GetAttachType(int type)
{
    return GetAttachTypeString(type);
}

const char* CSyncScript::GetAttackElement(EquipmentParameterInfo* param)
{
    return GetAttackElementString(param->m_field140);
}

void CSyncScript::GetLevelUpSkill(STEquipmentScript* script, int idx)
{
    static std::string level_up_skill;
    level_up_skill.clear();
    std::vector<STEquipmentScript::LevelUpSkill>& vec =
        script->m_levelUpSkills[idx];
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        level_up_skill += "|";
        const char* job = GetUserJob(vec[i].m_job);
        if (std::string(job) == "")
        {
            level_up_skill += "\xb0\xf8\xc5\xeb";   // 공용
        }
        else
        {
            level_up_skill += job;
        }
        char tmp[0x400];
        sprintf(tmp, "| %d| %d", vec[i].m_field4, vec[i].m_field8);
        level_up_skill += tmp;
    }
    level_up_skill.c_str();
}

const char* CSyncScript::GetUsableCharacter(STEquipmentScript* script)
{
    if (script->m_fielda0 == 0)
    {
        for (int i = 0; i < 0xb; ++i)
        {
            if (script->m_usable[i] != 0)
            {
                return GetUserJob(i);
            }
        }
        return "";
    }
    return "\xb8\xf0\xb5\xce";   // 모두
}

const char* CSyncScript::GetUsableCharacter(STStackableScript* script)
{
    if (script->m_fielda0 == 0)
    {
        for (int i = 0; i < 0xb; ++i)
        {
            if (script->m_usable[i] != 0)
            {
                return GetUserJob(i);
            }
        }
        return "";
    }
    return "\xb8\xf0\xb5\xce";   // 모두
}

bool CSyncScript::LoadItemInfoMasterScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    std::string line;
    std::string str;
    while (ScanType(line, true))
    {
        if (line == "[master type]")
        {
            int value = ScanInt((int*)0);
            ScanStr(&str);
            m_map64[(unsigned char)value] = str;
        }
        else if (line == "[type addition]")
        {
            int value = ScanInt((int*)0);
            m_vec1.push_back((unsigned char)value);
        }
        else if (line == "[reverse desc]")
        {
            int value = ScanInt((int*)0);
            m_vec2.push_back((unsigned char)value);
        }
        else if (line == "[material type]")
        {
            int value = ScanInt((int*)0);
            ScanStr(&str);
            m_map7c[(unsigned char)value] = str;
        }
        else if (line == "[weapon type]")
        {
            ScanStr(&str);
            int type = ScanInt((int*)0);
            int subType = ScanInt((int*)0);
            int value = ScanInt((int*)0);
            std::string name;
            ScanStr(&name);
            int job = getCharacterJob(str.c_str());
            SWEAPONTYPE wp;
            wp.m_job = job;
            wp.m_idx = subType;
            m_weaponMap[wp] = SITEMTYPE(false, (unsigned char)type,
                                         (unsigned char)subType,
                                         (unsigned short)value, name);
        }
        else if (line == "[armor type]")
        {
            ScanStr(&str);
            int type = ScanInt((int*)0);
            int subType = ScanInt((int*)0);
            int value = ScanInt((int*)0);
            std::string name;
            ScanStr(&name);
            SARMORTYPE ap;
            ap.m_field0 = (unsigned char)subType;
            ap.m_str4 = str;
            m_armorMap[ap] = SITEMTYPE(false, (unsigned char)type,
                                       (unsigned char)subType,
                                       (unsigned short)value, name);
        }
        else if (line == "[item type]")
        {
            ScanStr(&str);
            int type = ScanInt((int*)0);
            int subType = ScanInt((int*)0);
            int value = ScanInt((int*)0);
            int extra = ScanInt((int*)0);
            std::string name;
            ScanStr(&name);
            m_itemTypeMap[str] = SITEMTYPE(type != 0, (unsigned char)subType,
                                           (unsigned char)value,
                                           (unsigned short)extra, name);

        }
    }
    return true;
}

bool CSyncScript::insert_skill_index(STSkillScript* script, int job)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    char name[0x100];
    memset(name, 0, sizeof(name));
    char escaped[0x300];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script->m_str4.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into skill_index(job, skill_idx, skill_name) values(%d, %d, '%s')",
                  job, script->m_field0, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_skill_index()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table skill_index");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::trancate_skill_info()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table skill_info; ");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_skill_info(STSkillScript* script, int job, int type)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    std::vector<int> localVec;
    for (unsigned int i = 0; i < script->m_vec44.size(); ++i)
    {
        localVec.push_back(script->m_vec44[i]);
        localVec.push_back(script->m_vec50[i]);
    }
    char buf[0xb000];
    char query[0x5000];
    sprintf(buf, "%d, %d, %d, '%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, '%s', '%s', '%s', '%s', '%s', %d",
            job, script->m_field0, type,
            script->m_str4.c_str(), script->m_str8.c_str(),
            script->m_strc.c_str(), script->m_str10.c_str(),
            script->m_14, script->m_30, script->m_34, script->m_38,
            script->m_3c, script->m_40, script->m_158, script->m_15c,
            script->m_134, script->m_21c, script->m_1a4,
            script->m_str194.c_str(), script->m_1cc, script->m_1d0,
            script->m_1e8, script->m_1d4, script->m_1d5,
            script->m_str160[0].c_str(), script->m_str160[1].c_str(),
            script->m_1d8);
    (void)localVec;
    sprintf(query,
            "inSert into skill_info(job_index, skill_index, module_type, name, name2, basic_explain, skill_explain, purchase_cost, type, skill_class, growtype_maximum_level, second_growtype_maximum_level, skill_fitness_growtype, skill_fitness_second_growtype, consume_item, required_level, required_level_range, pre_required_skill, consume_mp, cool_time, casting_time, icon, command_key_explain, skill_command_advantage, static_data, level_info, start_cool_time) values(%s)",
            buf);
    db->set_query(query);
    return db->exec(true);
}

bool CSyncScript::insert_quest_category(QuestScript* script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    char name[0x100];
    memset(name, 0, sizeof(name));
    char escaped[0x100];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script->m_name.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into quest_category(quest_idx, quest_name) values(%d, '%s')",
                  script->m_index, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_quest_category()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table quest_category");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_charac_quest_category(unsigned int origin,
                                               unsigned int mapped)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into charac_quest_ref(origin_idx, mapped_idx) values(%d, %d)",
                  origin, mapped);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncate_charac_quest_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_quest_ref; ");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_exp_level_to_db(unsigned int exp, int level)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into exp_level_ref(exp, lev) values(%u, %d)", exp, level);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncate_exp_level_ref_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table exp_level_ref");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_item_parts_set(int index, STEquipmentPartSetPiece& piece)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into item_part_set(part_set_index, part_name, part_type, part_grade, part_rarity) values(%d, '%s',%d,%d,%d)",
                  index, piece.m_name.c_str(), piece.m_field4, piece.m_field8,
                  piece.m_fieldc);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::trancate_item_parts_set()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table item_part_set");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_aicharacter_info_to_db(int idx,
                                                const STAICharacterCommonScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (idx == 0 || script.m_str4.length() == 0)
    {
        return true;
    }
    char name[0xf0];
    memset(name, 0, sizeof(name));
    char escaped[0x100];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script.m_str4.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into dnf_aicharacter_info(idx, ai_name_kr) values(%d, '%s')",
                  idx, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_aicharacter_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_aicharacter_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_town_info_to_db(const TownScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (script.m_field0 == 0 || script.m_str4.length() == 0)
    {
        return true;
    }
    char name[0xf0];
    memset(name, 0, sizeof(name));
    char escaped[0x100];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script.m_str4.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into dnf_town_info(idx, town_name_kr) values(%d, '%s')",
                  script.m_field0, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_town_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_town_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_monster_info_to_db(const STMonsterScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (script.m_field0 == 0 || script.m_str4.length() == 0)
    {
        return true;
    }
    char name[0xf0];
    memset(name, 0, sizeof(name));
    char escaped[0x100];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script.m_str4.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into dnf_monster_info(idx, mon_name_kr) values(%d, '%s')",
                  script.m_field0, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_monster_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_monster_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_creature_info_to_db(unsigned int id,
                                             const CreatureScript* script,
                                             const char* param3,
                                             const char* param4)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    char buf[0xb00];
    memset(buf, 0, sizeof(buf));
    const char* skillNames[3];
    for (int i = 0; i < 3; ++i)
    {
        if (i < (int)script->m_skillNames.size())
        {
            skillNames[i] = script->m_skillNames[i].c_str();
        }
        else
        {
            skillNames[i] = "";
        }
    }
    char escaped[11][0x100];
    for (int i = 0; i < 11; ++i)
    {
        memset(escaped[i], 0, 0x100);
    }
    const char* values[11];
    values[0] = script->m_str4.c_str();
    values[1] = script->m_str9c.c_str();
    values[2] = param3;
    values[3] = param4;
    values[4] = "";
    values[5] = skillNames[0];
    values[6] = skillNames[1];
    values[7] = skillNames[2];
    values[8] = "";
    values[9] = script->m_str134.c_str();
    values[10] = "";
    for (int i = 0; i < 11; ++i)
    {
        GetEscapeString(db, values[i], escaped[i]);
    }
    db->set_query(
        "inSert into dnf_creature_info(it_id, creature_id, creature_name, skill_recovery_time, overskill_recovery_time, artifact_slot, learn_overskill_level, skill_info, overskill_info, piercing, skill_name, skill_desc, overskill_name, overskill_desc, skill_level_values, overskill_level_values, evolution_creature_id, evolution_level) values(%u, %d, '%s', %d, %d, '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d)",
        id, script->m_field0, escaped[0], script->m_1c, script->m_20,
        escaped[1], script->m_5c, escaped[2], escaped[3], escaped[4],
        escaped[5], escaped[6], escaped[7], escaped[8], escaped[9],
        escaped[10], script->m_178, script->m_17c);
    return db->exec(true);
}

bool CSyncScript::truncate_creature_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_creature_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_one_item_info_master(unsigned char masterNo,
                                              unsigned short subNo,
                                              const char* name,
                                              const char* explain)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    const char* names[2];
    names[0] = toMbcs(name);
    names[1] = toMbcs(explain);
    char escaped[2][100];
    memset(escaped, 0, sizeof(escaped));
    for (int i = 0; i < 2; ++i)
    {
        GetEscapeString(db, names[i], escaped[i]);
    }
    if (!db->set_query(
            "inSert into dnf_item_info_master(master_no, sub_no, name, master_explain) values(%u, %u, '%s', '%s')",
            (unsigned int)masterNo, (unsigned int)subNo, escaped[0], escaped[1]))
    {
        return false;
    }
    return db->exec(true);
}

bool CSyncScript::insert_item_info_master_to_db()
{
    if (m_field0 != 1)
    {
        return true;
    }
    for (std::map<SWEAPONTYPE, SITEMTYPE>::iterator it = m_weaponMap.begin();
         it != m_weaponMap.end(); ++it)
    {
        char name[0x100];
        char full[0x100];
        sprintf(name, "%s%s", m_map7c[it->second.m_field2].c_str(),
                it->second.m_str8.c_str());
        sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(), name);
        if (!insert_one_item_info_master(it->second.m_field1,
                                         it->second.m_field4, name, full))
        {
            return false;
        }
    }
    for (std::map<SARMORTYPE, SITEMTYPE>::iterator it = m_armorMap.begin();
         it != m_armorMap.end(); ++it)
    {
        char name[0x100];
        char full[0x100];
        sprintf(name, "%s%s", m_map7c[it->second.m_field2].c_str(),
                it->second.m_str8.c_str());
        sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(), name);
        if (!insert_one_item_info_master(it->second.m_field1,
                                         it->second.m_field4, name, full))
        {
            return false;
        }
    }
    for (std::map<std::string, SITEMTYPE>::iterator it = m_itemTypeMap.begin();
         it != m_itemTypeMap.end(); ++it)
    {
        if (it->second.m_field0 == 1)
        {
            bool matched = false;
            for (std::vector<unsigned char>::iterator vi = m_vec1.begin();
                 vi != m_vec1.end(); ++vi)
            {
                if (it->second.m_field1 == *vi)
                {
                    matched = true;
                    break;
                }
            }
            char full[0x100];
            sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(),
                    it->second.m_str8.c_str());
            if (matched)
            {
                if (!insert_one_item_info_master(it->second.m_field1,
                                                 it->second.m_field4,
                                                 m_map64[it->second.m_field1].c_str(),
                                                 full))
                {
                    return false;
                }
            }
            else
            {
                if (!insert_one_item_info_master(it->second.m_field1,
                                                 it->second.m_field4,
                                                 it->second.m_str8.c_str(),
                                                 full))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool CSyncScript::truncate_item_info_master_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_item_info_master"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_old_equip_info_to_db()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    for (std::map<unsigned int, STEquipmentStatInfo>::iterator it =
             m_oldEquipMap.begin();
         it != m_oldEquipMap.end(); ++it)
    {
        if (!db->set_query(
                "inSert into dnf_old_equip_info(it_id, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all_elements, ref_slow, ref_freeze, ref_poison, ref_stun, ref_curse, ref_blind, ref_lightning, ref_stone, ref_sleep, ref_burn, ref_weapon_break, ref_bleeding, ref_pierce, ref_stuck, ref_confuse, ref_hold, ref_armor_break, ref_all_state) values(%d, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)",
                it->first, it->second.m_hpMax, it->second.m_mpMax,
                it->second.m_phyAtt, it->second.m_phyDef,
                it->second.m_magAtt, it->second.m_magDef,
                it->second.m_equipPhyAtt, it->second.m_equipPhyDef,
                it->second.m_equipMagAtt, it->second.m_equipMagDef,
                it->second.m_refFire, it->second.m_refWater,
                it->second.m_refDark, it->second.m_refLight,
                it->second.m_refAll, it->second.m_refSlow,
                it->second.m_refFreeze, it->second.m_refPoison,
                it->second.m_refStun, it->second.m_refCurse,
                it->second.m_refBlind, it->second.m_refLightning,
                it->second.m_refStone, it->second.m_refSleep,
                it->second.m_refBurn, it->second.m_refWeaponBreak,
                it->second.m_refBleeding, it->second.m_refPierce,
                it->second.m_refStuck, it->second.m_refConfuse,
                it->second.m_refHold, it->second.m_refArmorBreak,
                it->second.m_refAllState))
        {
            return false;
        }
        if (!db->exec(true))
        {
            return false;
        }
    }
    return true;
}

bool CSyncScript::truncate_old_equip_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_old_equip_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_mapping_equip_info_to_db(std::map<int, int>& mapping)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    for (std::map<int, int>::iterator it = mapping.begin();
         it != mapping.end(); ++it)
    {
        if (it->second != 0)
        {
            db->set_query("inSert into equip_mapping_info(equip_idx, mapping_idx) values(%d, %d)",
                          it->second, it->first);
            if (!db->exec(true))
            {
                return false;
            }
        }
    }
    return true;
}

bool CSyncScript::truncate_mapping_equip_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table equip_mapping_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_random_option_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    bool ok = true;
    for (int prefix = 0; prefix <= 0x64 && ok; ++prefix)
    {
        for (unsigned char grade = 0; grade < 0x65; ++grade)
        {
            std::string name;
            name = ((RandomOptionScript*)((char*)G_CDataManager() + 0x4e10))->getPrefix(prefix, grade, false);
            if (name.length() == 0)
            {
                continue;
            }
            char converted[0x100];
            memset(converted, 0, sizeof(converted));
            char escaped[0x100];
            memset(escaped, 0, sizeof(escaped));
            CodePage::script2Database((char*)name.c_str(), converted);
            db->escape_string(escaped, converted);
            db->set_query("inSert into random_option_ref(random_option_index, random_option_value, random_option_name) values(%d,%d, '%s')",
                          prefix, (unsigned int)grade, escaped);
            if (!db->exec(true))
            {
                ok = false;
                break;
            }
        }
    }
    return ok;
}

bool CSyncScript::truncate_random_option_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table random_option_ref"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_charac_action_point_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return APSystem::CSyncScript::InsertDescTable();
}

bool CSyncScript::truncate_charac_action_point_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return APSystem::CSyncScript::TruncateDescTable();
}

bool CSyncScript::insert_charac_advance_altar_item_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return advancealtar::SyncScript::insertItemDescTable();
}

bool CSyncScript::truncate_charac_advance_altar_item_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return advancealtar::SyncScript::truncateItemDescTable();
}

}  // namespace sync_script
