#include "LogManager.h"
#include "GameRand.h"
// df_game_r CItemList 还原（2026-08-16 / 2026-08-17 第九批 I1 收尾）。
// 成员布局以 ORIG 构造/析构为基准；函数形态以 class_func_reports/CItemList.md
// 与 class_func_reports/sync_script__CSyncScript.md 逐条对照
// （AE 口径：调用目标/全局地址归一，指令形态逐条一致）。

#include "CItemList.h"
#include "STEquipmentScript.h"   // EquipmentAniInfoScript / STEquipmentAniScript（insert ani 段真实类型）

#include "CUserCharacInfo.h"   // get_charac_level / get_charac_job（Arad_GiftItem_Set::level_check 依赖）

#include <algorithm>
#include <new>        // placement new
#include <stdarg.h>
#include <utility>
#include <ctime>      // time()

#include "PvPTypes.h" // CMTRand
#include "DNFLexWrapper.h"  // loadRDARScriptFile/ScanType/ScanStr/ScanInt（LoadScript）
#include "CUser.h"    // Arad_GiftItem_Set::level_check 解引用 CUser（原经 PvPTypes→CUser.h
                      // 传递获得；该边已断开以修复 CUser.h 重入环）

#include "GameTypes.h"
#include "GlobalData.h"
#include "CDataManager.h"
#include "CItemAmplifier.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "DNFFunctionLib.h"
#include "CFieldLengthChecker.h"   // 权威 AddField（ORIG W，替代本地 FieldChecker 桩）
#include "EquipmentParameterInfo.h"  // ItemAuraInfo/LevelUpSkill/EquipmentSkillDataUp
                                    // 及 GetAttackElement 实参类型

// ============================================================================
// sync_script 域辅助类型（TU 私有；ORIG 对应符号属后续批次，调用目标在 AE
// 口径下归一，这里只保证尺寸/调用形态一致）。
// ============================================================================
namespace
{
// ORIG CStreamBuffer：0xa000 字节（栈上对象），operator<< 返回流引用。
class MyStream
{
public:
    MyStream() {}
    MyStream& operator<<(const char* s);
    MyStream& operator<<(int v);
    MyStream& operator<<(unsigned int v);
    MyStream& operator<<(float v);
    char* c_str() { return m_buf; }

    char m_buf[0xa000];   // +0x00
};

MyStream& MyStream::operator<<(const char* s)
{
    strcat(m_buf, s);
    return *this;
}

MyStream& MyStream::operator<<(int v)
{
    char tmp[32];
    sprintf(tmp, "%d", v);
    strcat(m_buf, tmp);
    return *this;
}

MyStream& MyStream::operator<<(unsigned int v)
{
    char tmp[32];
    sprintf(tmp, "%u", v);
    strcat(m_buf, tmp);
    return *this;
}

MyStream& MyStream::operator<<(float v)
{
    char tmp[32];
    sprintf(tmp, "%f", v);
    strcat(m_buf, tmp);
    return *this;
}


// ORIG sync_script::CStringMaker 语义的 TU 局部镜像（匿名命名空间，符号不导出）：
// for_each 累加器，int/float 项以 ',' 分隔，结构项以 ';' 分隔（与权威实现一致）。
class StringMaker
{
public:
    StringMaker() {}
    void operator()(int v)
    {
        appendInt("%d", v);
    }
    void operator()(ItemAuraInfo v)
    {
        appendStruct("%d,%d,%d,%d", v.m_field0, (int)v.m_field4, v.m_float8,
                     v.m_fieldc);
    }
    void operator()(LevelUpSkill v)
    {
        int job = (v.m_job == 0xb) ? 0x63 : v.m_job;
        appendStruct("%d,%d,%d", job, v.m_field4, v.m_field8);
    }
    void operator()(EquipmentSkillDataUp v)
    {
        appendStruct("%d,%d,%d,%d,%d,%d,%d", v.m_job, v.m_v4, v.m_v8,
                     v.m_percent, v.m_v10, v.m_key, v.m_module);
    }
    const char* c_str() const
    {
        return m_str.c_str();
    }
private:
    void appendInt(const char* fmt, int v)
    {
        char num[0x100] = {0};
        sprintf(num, fmt, v);
        if (!m_str.empty())
        {
            m_str += ",";
        }
        m_str += num;
    }
    void appendStruct(const char* fmt, ...)
    {
        char num[0x100] = {0};
        va_list ap;
        va_start(ap, fmt);
        vsprintf(num, fmt, ap);
        va_end(ap);
        if (!m_str.empty())
        {
            m_str += ";";
        }
        m_str += num;
    }
    std::string m_str;
};
}  // namespace（TU 私有辅助）
// ORIG T 0x0861ca1b：socket 类型 → 字符。1→'1' 2→'2' 4→'3' 8→'4'
// 0x10→'5' 0xffef→'6' 0xffff→'7'，其余 0（决策树见 ORIG 反汇编）。
int GetSocketTypeCharacter(unsigned short type)
{
    switch (type)
    {
    case 1:
        return '1';
    case 2:
        return '2';
    case 4:
        return '3';
    case 8:
        return '4';
    case 0x10:
        return '5';
    case 0xffef:
        return '6';
    case 0xffff:
        return '7';
    default:
        return 0;
    }
}


// ORIG STStackableScript 字段镜像（insert_stackable_item_to_db 访问位）。
struct StackScriptFull
{
    int m_itNo;                     // +0x00
    int m_class;                     // +0x04
    int m_revertType;                     // +0x08
    int m_createRatio;                     // +0x0c
    int m_rarity;                    // +0x10
    int m_price;                    // +0x14
    int m_cash;                    // +0x18
    int m_medal;                    // +0x1c
    char m_pad20[0x14];               // +0x20
    int m_weight;                    // +0x34
    char m_pad38[0x24];               // +0x38
    int m_level;                    // +0x5c
    char m_pad60[8];                  // +0x60
    int m_coolTime;                    // +0x68
    std::string m_url;              // +0x6c
    int m_urlLen;                    // +0x70
    char m_pad74[0xac - 0x74];        // +0x74
    std::string m_name;              // +0xac
    std::string m_engName;              // +0xb0
    std::string m_explain;              // +0xb4
    std::string m_detailExplain;              // +0xb8
    std::string m_flavorText;              // +0xbc
    int m_fieldc0;                    // +0xc0
    int m_iconMarkNumber;                    // +0xc4
    char m_padc8[0x4bc - 0xc8];
    std::map<int, void*> m_map4bc;    // +0x4bc（ORIG map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>）
};

// EquipmentParameterInfo 字段镜像（TU 私有）：真名取自 dnf_item_info 列名。
struct EquipParamInfo
{
    // dnf_item_info 列名即字段真名（insert_stackable_item_to_db 值序实证）。
    char m_pad00[4];                                   // +0x000
    int m_hpMax;                                       // +0x004 hp_max
    float m_hpMaxRate;                                 // +0x008 hp_max_rate
    int m_mpMax;                                       // +0x00c mp_max
    float m_mpMaxRate;                                 // +0x010 mp_max_rate
    int m_phyAtt;                                      // +0x014 phy_att
    int m_phyDef;                                      // +0x018 phy_def
    int m_magAtt;                                      // +0x01c mag_att
    char m_pad20[4];
    int m_magDef;                                      // +0x024 mag_def
    char m_pad28[0x54 - 0x28];
    int m_equipPhyAtt;                                 // +0x054 equip_phy_att
    char m_pad58[4];
    int m_equipPhyDef;                                 // +0x05c equip_phy_def
    char m_pad60[4];
    int m_equipMagAtt;                                 // +0x064 equip_mag_att
    char m_pad68[4];
    int m_equipMagDef;                                 // +0x06c equip_mag_def
    char m_pad70[0x74 - 0x70];
    int m_physAbsDmg;                                  // +0x074 physical_absolute_damage
    int m_physDmgReduce;                               // +0x078 physical_damage_reduce
    float m_physAbsDef;                                // +0x07c physical_absolute_defense
    int m_magAbsDmg;                                   // +0x080 magical_absolute_damage
    int m_magDmgReduce;                                // +0x084 magical_damage_reduce
    float m_magAbsDef;                                 // +0x088 magical_absolute_defense
    char m_pad8c[0xb0 - 0x8c];
    int m_refFire;                                     // +0x0b0 ref_fire
    int m_refWater;                                    // +0x0b4 ref_water
    int m_refDark;                                     // +0x0b8 ref_dark
    int m_refLight;                                    // +0x0bc ref_light
    int m_refAll;                                      // +0x0c0 ref_all
    int m_refSlow;                                     // +0x0c4 ref_slow
    int m_refFreeze;                                   // +0x0c8 ref_freeze
    int m_refPoison;                                   // +0x0cc ref_poison
    int m_refStun;                                     // +0x0d0 ref_stun
    int m_refCus;                                      // +0x0d4 ref_cus
    int m_refBlind;                                    // +0x0d8 ref_blind
    int m_refLite;                                     // +0x0dc ref_lite
    int m_refSton;                                     // +0x0e0 ref_ston
    int m_refSleep;                                    // +0x0e4 ref_sleep
    int m_refDeekement;                                // +0x0e8 ref_deekement
    int m_refDeadlystrike;                             // +0x0ec ref_deadlystrike
    int m_refBleeding;                                 // +0x0f0 ref_bleeding
    int m_refPierce;                                   // +0x0f4 ref_pierce
    int m_refStuck;                                    // +0x0f8 ref_stuck
    int m_refConfuse;                                  // +0x0fc ref_confuse
    int m_refHold;                                     // +0x100 ref_hold
    int m_armorBreakRes;                               // +0x104 armor_break_resistance
    int m_refAllStat;                                  // +0x108 ref_all_stat
    int m_invenMax;                                    // +0x10c inven_max
    int m_hpRegenRate;                                 // +0x110 hp_regenrate
    int m_mpRegenRate;                                 // +0x114 mp_regenrate
    char m_pad118[4];                                  // +0x118（本 TU 未读）
    int m_movSpeed;                                    // +0x11c mov_speed
    char m_pad120[8];                                  // +0x120（本 TU 未读）
    int m_attSpeed;                                    // +0x128 att_speed
    int m_quest;                                       // +0x12c quest
    int m_hitRecovery;                                 // +0x130 hit_recovery
    int m_jump;                                        // +0x134 jump
    float m_criticalHitRate;                           // +0x138 criticalhit_rate
    float m_magicalCriticalHit;                        // +0x13c magical_critical_hit
    char m_pad140[4];                                  // +0x140（本 TU 未读）
    int m_attActiveStatus;                             // +0x144 att_active_status
    float m_attActiveStatusRatio;                      // +0x148 att_active_status_ratio
    int m_attActiveStatusPow;                          // +0x14c att_active_status_pow
    char m_pad150[0x188 - 0x150];
    std::vector< ::ItemAuraInfo> m_vec188;             // +0x188 item_aura
    char m_pad194[0x1b8 - 0x194];
    std::vector< ::EquipmentSkillDataUp> m_vec1b8;     // +0x1b8 skill_data_up
    char m_pad1c4[0x220 - 0x1c4];
    int m_attBackforce;                                // +0x220 att_backforce
    int m_attUpforce;                                  // +0x224 att_upforce
    float m_stuckRate;                                 // +0x228 stuck_rate
    unsigned char m_attDefenseIgnore;                  // +0x22c att_defenseIgnore
    char m_pad22d[0x23c - 0x22d];
    unsigned char m_attHpDrain;                        // +0x23c att_hp_drain
    unsigned char m_attMpDrain;                        // +0x23d att_mp_drain
    char m_pad23e[0x24c - 0x23e];
    std::vector< ::LevelUpSkill> m_levelUpSkills;      // +0x24c skill_levelup
    char m_pad258[0x264 - 0x258];
    std::vector< ::STAvatarAbilityVariation> m_vec264; // +0x264 avatar_select_ability
    char m_pad270[0x274 - 0x270];
    float m_roomListMoveSpeedRate;                     // +0x274 room_list_move_speed_rate
    char m_pad278[0x284 - 0x278];
    int m_rigidity;                                    // +0x284 rigidity
    int m_fireAttack;                                  // +0x288 fire_attack
    int m_waterAttack;                                 // +0x28c water_attack
    int m_darkAttack;                                  // +0x290 dark_attack
    int m_lightAttack;                                 // +0x294 light_attack
    int m_allAttack;                                   // +0x298 all_attack
    char m_pad29c[0x560 - 0x29c];                      // 尾部未访问区段占位
};

struct stAvatarTypeSelectFull
{
    int m_field0; int m_field4; int m_field8; int m_fieldc;
    int m_field10; unsigned short m_ushort14[8];
};
// ORIG STEquipmentAniScript 局部镜像；本 TU 仅 insert_equiptment_item_to_db 读取
// 动画帧号（+0x0c/+0x10/+0x30/+0x34，格式化为 "%02d%02d"）。注意 setEquipmentAniScriptLay
// （ORIG 0x89c1cb9）对同一区段（+0xc..+0x40）写入 std::string；两函数对该区的解读不同
// 【推断】——此处镜像以 insert 函数读法具名，保持既有 int 读行为可审计。
// ORIG STEquipmentScript 字段镜像（insert_equiptment_item_to_db 访问位）。
struct EquipScriptFull
{
    int m_field0; int m_field4; int m_field8; int m_fieldc;
    int m_field10; int m_field14; int m_field18; int m_field1c;
    int m_field20; int m_field24; int m_field28; int m_field2c;
    int m_field30; int m_field34;
    char m_aniFlags[0xb]; char m_pad43[0x19];
    int m_field5c; char m_pad60[8]; int m_field68;
    char m_pad6c[0x118];
    std::vector<int> m_vec198;
    char m_pad1a4[0x18]; int m_field1b4; int m_field1b8;
    int m_field1bc; int m_field1c4; int m_field1cc;
    int m_field1d4; int m_field1d8; int m_field1dc;
    std::string m_strb4; std::string m_strbc;
    int m_field184;
    char m_pad1e0[4]; int m_field1e4; char m_pad1e8[0x2c];
    int m_field214; char m_pad218[4]; int m_field21c;
    char m_pad220[4]; int m_field224;
    char m_pad228[4]; int m_field22c;
    char m_pad230[0x40]; int m_field270; int m_field274;
    int m_field278; int m_field27c; int m_field280; int m_field284;
    int m_field288; int m_field28c; int m_field290; int m_field294;
    int m_field298; int m_field29c; int m_field2a0; int m_field2a4;
    int m_field2a8; int m_field2ac; int m_field2b0; int m_field2b4;
    int m_field2b8; int m_field2bc; int m_field2c0; int m_field2c4;
    int m_field2c8; int m_field2cc; int m_field2d0;
    int m_field2d4; char m_pad2d8[4]; int m_field2dc;
    int m_field2f0; int m_field2f4; int m_field2f8; float m_field2fc;
    int m_field304; float m_field308; int m_field30c;
    char m_pad310[0xd0]; int m_field3e0; int m_field3e4;
    float m_field3e8; unsigned char m_field3ec;
    char m_pad3ed[0xf]; unsigned char m_field3fc; unsigned char m_field3fd;
    float m_field434; int m_field440; int m_field444;
    int m_field448; int m_field44c; int m_field450; int m_field454; int m_field458;
    int m_field798; int m_field70; int m_fieldc4;
    int m_field234; int m_field238; float m_field23c; int m_field240; int m_field244; float m_field248;
    EquipParamInfo m_equipParam;
    char m_pad640[0x18]; std::vector<std::string> m_vec658;
    std::string m_str664; std::string m_str668; std::vector<int> m_vec66c;
    char m_pad690[0x10]; int m_field6a0;
    std::string m_str6a4; std::string m_str6a8; std::string m_str6ac;
    std::string m_str6b0; std::string m_str6b4; std::string m_str6b8;
    char m_pad6bc[4]; std::vector<stAvatarTypeSelectFull> m_vec6c0;
    std::vector<unsigned short> m_vec6cc; char m_pad6d8[0x90];
    std::vector<ENUM_EQUIPMENTTYPE> m_vec768;
    std::map<int, EquipParamInfo> m_map678;
};

 
// ===================== 外部依赖（其它 TU 提供，仅声明） =====================

namespace sync_script
{
// 布局依据 ORIG 构造（0x861cc6c）/ 析构（0x861cd7a）：
//   +0x00 bool m_field0
//   +0x04 vector<unsigned char>（12 字节）
//   +0x10 vector<unsigned char>
//   +0x1c map<string, SITEMTYPE>
//   +0x34 map<SWEAPONTYPE, SITEMTYPE>
//   +0x4c map<SARMORTYPE, SITEMTYPE>
//   +0x64 map<unsigned char, string>
//   +0x7c map<unsigned char, string>
struct SITEMTYPE
{
    char m_field0;         // +0x00
    char m_pad1[3];
    unsigned short m_field4;  // +0x04（ORIG GetType 读 *(ushort*)(p+4)）
};

struct SWEAPONTYPE
{
    int m_field0;
    int m_field4;
    bool operator<(const SWEAPONTYPE& o) const
    {
        return m_field0 < o.m_field0 ||
               (m_field0 == o.m_field0 && m_field4 < o.m_field4);
    }
};

struct SARMORTYPE
{
    unsigned char m_field0;
    std::string m_str4;
    bool operator<(const SARMORTYPE& o) const
    {
        return m_field0 < o.m_field0 ||
               (m_field0 == o.m_field0 && m_str4 < o.m_str4);
    }
};

class CSyncScript
{
public:
    CSyncScript();
    ~CSyncScript();

    bool truncate_dnf_item_info_table();
    bool insert_equiptment_item_to_db(STEquipmentScript* script,
                                     unsigned int idx, int mode, int key);
    bool insert_stackable_item_to_db(STStackableScript* script);
    bool truncateCeraShopItemTable();
    bool insertCeraShopItemIntoDB(unsigned int itemIdx, unsigned int itemType,
                                  int value, int count, short c,
                                  const char* name, const char* iconName);
    bool truncate_artifact_info_table();

    // ---- 以下成员实现在 sync_script.cpp（权威 TU），此处仅声明供本文件调用 ----
    bool insert_artifact_info_to_db(STEquipmentScript* script);
    bool GetType(std::string name, ENUM_CHARACTERJOB job, int idx, int& out1,
                 int& out2, bool flag);
    const char* GetURL(const char* str, int len, int idx);
    const char* GetAttachType(int type);
    const char* GetAttackElement(EquipmentParameterInfo* param);
    const char* GetUsableCharacter(STEquipmentScript* script);
    const char* GetUsableCharacter(STStackableScript* script);

    bool m_field0;                                   // +0x00
    std::vector<unsigned char> m_vec1;               // +0x04
    std::vector<unsigned char> m_vec2;               // +0x10
    std::map<std::string, SITEMTYPE> m_itemTypeMap;  // +0x1c
    std::map<SWEAPONTYPE, SITEMTYPE> m_weaponMap;    // +0x34
    std::map<SARMORTYPE, SITEMTYPE> m_armorMap;      // +0x4c
    std::map<unsigned char, std::string> m_map64;    // +0x64
    std::map<unsigned char, std::string> m_map7c;    // +0x7c
};

// 权威定义在 InitScriptProducers_4.cpp（loadRDARScriptListFile 填充）。
extern "C" std::map<int, std::string> g_stEquipmentScriptFileList;  // ORIG 0x94fac00
extern "C" std::map<int, std::string> g_stStackableScriptFileList;  // ORIG 0x94fdc00

namespace
{
// ORIG 里是 CFieldLengthChecker::fieldNames_ / fieldLength_（0x94734a0 /
// 0x94734ac）。引用地址在 AE 口径下归一，本地全局即可。
std::vector<std::string> g_fieldNames;
std::vector<unsigned int> g_fieldLengths;
}

CSyncScript* GetInstanceSyncScript()                  // ORIG 0x861cbb4
{
    static CSyncScript instance;
    return &instance;
}

void DisplayMaxLength()                               // ORIG 0x861ca94
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    for (unsigned int i = 0; i < g_fieldNames.size(); ++i)
    {
        if (g_fieldLengths[i] == 0)
        {
            continue;
        }
        db->set_query("seLect max(length(%s)) from dnf_item_info",
                      g_fieldNames[i].c_str());
        if (!db->exec(true))
        {
            continue;
        }
        if (!db->fetch())
        {
            continue;
        }
        unsigned int len = 0;
        if (!db->get_uint(0, len))
        {
            continue;
        }
        if (g_fieldLengths[i] != len) { }
    }
}

CSyncScript::CSyncScript()
{
    m_field0 = 0;
}

CSyncScript::~CSyncScript()
{
    m_vec1.clear();
    m_vec2.clear();
    m_itemTypeMap.clear();
    m_weaponMap.clear();
    m_armorMap.clear();
    m_map64.clear();
    m_map7c.clear();
}

bool CSyncScript::truncate_dnf_item_info_table()      // ORIG 0x8622374
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table dnf_item_info");
    if (!db->exec(true))
    {
        return false;
    }
    db->set_query("trUncate table avatar_select_ability");
    if (!db->exec(true))
    {
        return false;
    }
    if (!truncate_artifact_info_table())
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncate_artifact_info_table()      // ORIG 0x8624b0a
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_artifact_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncateCeraShopItemTable()         // ORIG 0x8622a5a
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table dnf_item_price; ");
    return db->exec(true);
}

bool CSyncScript::insert_equiptment_item_to_db(STEquipmentScript* script,
                                              unsigned int idx, int mode,
                                              int key)
{
    if (m_field0 != 1)
    {
        return true;
    }
    EquipScriptFull* es = (EquipScriptFull*)script;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    unsigned int local_154 = 0;
    int local_158 = 0;
    int foundAni = 0;
    for (int i = 0; i <= 0xa; ++i)
    {
        if (*(char*)&es->m_aniFlags[i] != 0)
        {
            foundAni = i;
            break;
        }
    }
    int local_74 = 0;
    unsigned int param_2 = idx;
    if (mode == 1)
    {
        local_74 = (int)param_2;
        param_2 = (unsigned int)-1;
    }
    else if (mode == 2)
    {
        local_74 = (int)param_2;
        param_2 = (unsigned int)-2;
    }
    else if (mode == 0)
    {
        std::string typeName = es->m_strbc;
        if (!GetType(typeName, (ENUM_CHARACTERJOB)foundAni, es->m_field1b8,
                              (int&)local_154, local_158, true))
        {
            return false;
        }
        param_2 = (unsigned int)es->m_field0;
        std::map<int, EquipParamInfo>::iterator it = es->m_map678.begin();
        while (it != es->m_map678.end())
        {
            EquipScriptFull temp;
            temp.m_equipParam = it->second;
            insert_equiptment_item_to_db((STEquipmentScript*)&temp, param_2, 1,
                                        it->first);
            ++it;
        }
    }
    char local_162[10];
    local_162[0] = 0;
    local_162[1] = 0;
    local_162[2] = 0;
    local_162[3] = 0;
    local_162[4] = 0;
    int socketCount = 0;
    if (!es->m_vec6cc.empty())
    {
        for (std::vector<unsigned short>::iterator vit = es->m_vec6cc.begin();
             vit != es->m_vec6cc.end() && socketCount <= 4; ++vit)
        {
            local_162[socketCount + 5] = GetSocketTypeCharacter(*vit);
            ++socketCount;
        }
    }
    if (!es->m_vec6c0.empty())
    {
        for (std::vector<stAvatarTypeSelectFull>::iterator ait =
                 es->m_vec6c0.begin();
             ait != es->m_vec6c0.end(); ++ait)
        {
            if (ait->m_field10 > 0)
            {
                for (int k = 0; k < ait->m_field10 && socketCount < 5;
                     ++socketCount)
                {
                    local_162[socketCount + 5] =
                        GetSocketTypeCharacter(ait->m_ushort14[8 + k]);
                    ++k;
                }
            }
        }
    }
    char local_1fc[64] = {0};
    int equipCount = 0;
    if (!es->m_vec768.empty())
    {
        for (std::vector<ENUM_EQUIPMENTTYPE>::iterator eit =
                 es->m_vec768.begin();
             eit != es->m_vec768.end(); ++eit)
        {
            char tmp[8] = {0};
            if (equipCount == 0)
            {
                sprintf(tmp, "%d", (int)*eit);
            }
            else
            {
                sprintf(tmp, "*%d", (int)*eit);
            }
            strcat(local_1fc, tmp);
            ++equipCount;
        }
    }
    char local_2fc[256] = {0};
    char local_3fc[256] = {0};
    char local_4fc[256] = {0};
    char local_5fc[256] = {0};
    char local_69 = 0;
    for (int i = 0; i <= 0xa; ++i)
    {
        if (*(char*)&es->m_aniFlags[i] == 0)
        {
            continue;
        }
        // ORIG 0x861df29：script->getAniInfo((ENUM_CHARACTERJOB)i) 返回
        // EquipmentAniInfoScript*；读 m_30/m_34/m_c/m_10 帧号字段与 m_ani0/m_ani18
        // 动画序列（逐指令转写，见 insert_equiptment_item_to_db 反汇编）。
        EquipmentAniInfoScript* aniInfo =
            script->getAniInfo((ENUM_CHARACTERJOB)i);
        if (aniInfo == 0)
        {
            continue;
        }
        if (aniInfo->m_30 == -1 && aniInfo->m_34 == -1 &&
            aniInfo->m_c == -1 && aniInfo->m_10 == -1)
        {
            continue;
        }
        int f30 = aniInfo->m_30;
        int f34 = aniInfo->m_34;
        int fc_ = aniInfo->m_c;
        int f10 = aniInfo->m_10;
        if ((f30 != -1 || f34 != -1) && (fc_ == -1 && f10 == -1))
        {
            char tmp[256] = {0};
            sprintf(tmp, "%02d%02d", f30, f34);
            if (local_69 != 0)
            {
                char t2[8] = {0};
                sprintf(t2, "*%d", (int)aniInfo->m_ani0.size());
                strcat(tmp, t2);
            }
            char t3[8] = {0};
            sprintf(t3, "*%d", (int)aniInfo->m_ani0.size());
            strcat(tmp, t3);
            for (std::vector<STEquipmentAniScript>::iterator ait =
                     aniInfo->m_ani0.begin();
                 ait != aniInfo->m_ani0.end(); ++ait)
            {
                strcat(tmp, "*");
                char clean[256] = {0};
                strcpy(clean, ait->m_str60.c_str());
                int w = 0;
                for (unsigned int r = 0; r < strlen(clean); ++r)
                {
                    if (clean[r] != '_')
                    {
                        clean[w++] = clean[r];
                    }
                }
                clean[w] = 0;
                strcat(tmp, clean);
            }
            if (local_69 == 1)
            {
                strcpy(local_4fc, tmp);
            }
            else
            {
                strcpy(local_2fc, tmp);
                local_69 = 1;
            }
        }
        else if (fc_ != -1 || f10 != -1)
        {
            char tmp[256] = {0};
            sprintf(tmp, "%02d%02d", fc_, f10);
            if (local_69 != 0)
            {
                char t2[8] = {0};
                sprintf(t2, "*%d", i);
                strcat(tmp, t2);
            }
            char t3[8] = {0};
            sprintf(t3, "*%d", (int)aniInfo->m_ani18.size());
            strcat(tmp, t3);
            for (std::vector<STEquipmentAniScript>::iterator ait =
                     aniInfo->m_ani18.begin();
                 ait != aniInfo->m_ani18.end(); ++ait)
            {
                strcat(tmp, "*");
                char clean[256] = {0};
                strcpy(clean, ait->m_str60.c_str());
                int w = 0;
                for (unsigned int r = 0; r < strlen(clean); ++r)
                {
                    if (clean[r] != '_')
                    {
                        clean[w++] = clean[r];
                    }
                }
                clean[w] = 0;
                strcat(tmp, clean);
            }
            if (local_69 == 1)
            {
                strcpy(local_5fc, tmp);
            }
            else
            {
                strcpy(local_3fc, tmp);
                local_69 = 1;
            }
        }
    }
    local_162[0] = 0;
    local_162[1] = 0;
    local_162[2] = 0;
    local_162[3] = 0;
    local_162[4] = 0;
    if (!es->m_vec198.empty())
    {
        int cnt = 0;
        for (std::vector<int>::iterator iit = es->m_vec198.begin();
             iit != es->m_vec198.end() && cnt <= 4; ++iit)
        {
            local_162[cnt] = (char)*iit + '0';
            ++cnt;
        }
    }
    char local_3e90[0x2c00] = {0};
    int local_68 = 4;
    std::string local_168;
    if (es->m_strb4.empty())
    {
        local_168 = es->m_str6a4;
    }
    else
    {
        local_168 = es->m_strb4;
    }
    char local_9fc[1024] = {0};
    if (!CodePage::script2Database((char*)es->m_str6ac.c_str(), local_9fc))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x398, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), es->m_str6ac.c_str(),
                      "bool sync_script::CSyncScript::insert_equiptment_item_to_db(STEquipmentScript*, unsigned int, int, int)",
                      0x398);
        return false;
    }
    char local_1290[1024] = {0};
    if (!CodePage::script2Database((char*)local_168.c_str(), local_1290))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x39f, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), es->m_strb4.c_str(),
                      "bool sync_script::CSyncScript::insert_equiptment_item_to_db(STEquipmentScript*, unsigned int, int, int)",
                      0x39f);
        return false;
    }
    MyStream buf;
    buf << param_2;
    buf << ", '" << GetEscapeString(db, local_9fc, local_3e90) << "'";
    buf << ", '"
        << GetEscapeString(db, (char*)es->m_str6b0.c_str(), local_3e90 + 0x400)
        << "'";
    buf << ", '" << GetEscapeString(db, local_1290, local_3e90 + 0x800) << "'";
    buf << ", " << local_158;
    buf << ", " << local_154;
    buf << ", '" << this->GetUsableCharacter(script) << "'";
    buf << ", " << es->m_field4;
    buf << ", " << es->m_field5c;
    buf << ", " << es->m_field184;
    buf << ", " << es->m_fieldc;
    buf << ", " << es->m_field10;
    buf << ", " << es->m_field34;
    buf << ", " << es->m_field1c;
    buf << ", " << es->m_field1bc;
    buf << ", " << es->m_field68;
    buf << ", " << es->m_field1c4;
    buf << ", " << es->m_field1cc;
    buf << ", " << es->m_field1d4;
    buf << ", " << es->m_field1d8;
    buf << ", " << es->m_field1dc;
    buf << ", " << es->m_field1e4;
    buf << ", " << es->m_field214;
    buf << ", " << es->m_field21c;
    buf << ", " << es->m_field224;
    buf << ", " << es->m_field22c;
    buf << ", " << es->m_field270;
    buf << ", " << es->m_field274;
    buf << ", " << es->m_field278;
    buf << ", " << es->m_field27c;
    buf << ", " << es->m_field280;
    buf << ", " << es->m_field284;
    buf << ", " << es->m_field288;
    buf << ", " << es->m_field28c;
    buf << ", " << es->m_field290;
    buf << ", " << es->m_field294;
    buf << ", " << es->m_field298;
    buf << ", " << es->m_field29c;
    buf << ", " << es->m_field2a0;
    buf << ", " << es->m_field2a4;
    buf << ", " << es->m_field2a8;
    buf << ", " << es->m_field2ac;
    buf << ", " << es->m_field2b0;
    buf << ", " << es->m_field2b4;
    buf << ", " << es->m_field2b8;
    buf << ", " << es->m_field2bc;
    buf << ", " << es->m_field2c0;
    buf << ", " << es->m_field2c4;
    buf << ", " << es->m_field2c8;
    buf << ", " << es->m_field2f0;
    buf << ", " << es->m_field2f4;
    buf << ", " << es->m_field304;
    buf << ", " << es->m_field308;
    buf << ", " << es->m_field30c;
    buf << ", " << es->m_field3e0;
    buf << ", " << es->m_field3e4;
    buf << ", " << (unsigned int)es->m_field3ec;
    buf << ", " << (unsigned int)es->m_field3fc;
    buf << ", " << (unsigned int)es->m_field3fd;
    buf << ", " << es->m_field2f8;
    buf << ", " << es->m_field3e8;
    buf << ", " << (unsigned int)es->m_field3ec;
    buf << ", '"
        << std::for_each(es->m_equipParam.m_levelUpSkills.begin(), es->m_equipParam.m_levelUpSkills.end(),
                         StringMaker()).c_str()
        << "'";
    const char* yn = "n";
    if (es->m_str668.empty() && es->m_vec66c.empty() &&
        es->m_field6a0 == -1 && es->m_field798 == -1)
    {
        yn = "n";
    }
    else
    {
        yn = "y";
    }
    buf << ", '" << yn << "'";
    buf << ", '" << local_162 + 5 << "'";
    int iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)es->m_str6ac.c_str(), local_3e90 + iVar)
        << "'";
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)es->m_str6b8.c_str(), local_3e90 + iVar)
        << "'";
    buf << ", " << es->m_field440;
    buf << ", " << es->m_field24;
    buf << ", " << es->m_field184;
    buf << ", '" << es->m_field28 << "," << es->m_field2c << "'";
    buf << ", " << es->m_field234;
    buf << ", " << es->m_field238;
    buf << ", " << es->m_field23c;
    buf << ", " << es->m_field240;
    buf << ", " << es->m_field244;
    buf << ", " << es->m_field248;
    buf << ", " << es->m_field448;
    buf << ", " << es->m_field44c;
    buf << ", " << es->m_field450;
    buf << ", " << es->m_field454;
    buf << ", " << es->m_field458;
    buf << ", " << es->m_field2ac;
    buf << ", " << es->m_field2c4;
    buf << ", " << es->m_field2c8;
    buf << ", " << es->m_field444;
    buf << ", '"
        << std::for_each(es->m_equipParam.m_vec188.begin(), es->m_equipParam.m_vec188.end(),
                         StringMaker()).c_str()
        << "'";
    buf << ", " << es->m_field2fc;
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)es->m_str668.c_str(), local_3e90 + iVar)
        << "'";
    buf << ", '"
        << std::for_each(es->m_vec66c.begin(), es->m_vec66c.end(),
                         StringMaker()).c_str()
        << "'";
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)es->m_str6b0.c_str(), local_3e90 + iVar)
        << "'";
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)es->m_str6b4.c_str(), local_3e90 + iVar)
        << "'";
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)es->m_str6ac.c_str(), local_3e90 + iVar)
        << "'";
    buf << ", " << es->m_field798;
    buf << ", '" << local_1fc << "'";
    buf << ", '"
        << std::for_each(es->m_equipParam.m_vec1b8.begin(), es->m_equipParam.m_vec1b8.end(),
                         StringMaker()).c_str()
        << "'";
    buf << ", '"
        << std::for_each(es->m_vec198.begin(), es->m_vec198.end(),
                         StringMaker()).c_str()
        << "'";
    buf << ", " << es->m_field434;
    buf << ", '"
        << this->GetURL((char*)es->m_str6ac.c_str(),
                     (int)es->m_str6ac.length(), es->m_field70)
        << "'";
    buf << ", " << es->m_fieldc4;
    db->set_query(
        "inSert into dnf_item_info(it_no, it_name, it_eng_name, it_explain, "
        "master_type, sub_type, job, class, revert, level, skill, "
        "create_ratio, rarity, weight, price, cash, medal, durability, "
        "cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, "
        "equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, "
        "ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, "
        "ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, "
        "ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, "
        "ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, "
        "inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, "
        "hit_recovery, jump, att_element, att_active_status, "
        "att_active_status_ratio, att_active_status_pow, att_backforce, "
        "att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, "
        "stuck_rate, att_defenseIgnore, skill_levelup, set_type, jewel_type, "
        "detail_explain, flavor_text, anti_evil, value, required_skill, "
        "need_material, physical_absolute_damage, physical_damage_reduce, "
        "physical_absolute_defense, magical_absolute_damage, "
        "magical_damage_reduce, magical_absolute_defense, fire_attack, "
        "water_attack, dark_attack, light_attack, all_attack, "
        "weapon_break_resistance, armor_break_resistance, "
        "all_activestatus_resistance, rigidity, item_aura, "
        "magical_critical_hit, set_name, set_item, fullset_basic_explain, "
        "fullset_detail_explain, parameter_basic_explain, "
        "parameter_detail_explain, part_set_index, hide_equipment, "
        "skill_data_up, it_set_no, db_piece_count, set_item_master, "
        "ani_variation, ani_variation_expand, ani_variation2, "
        "ani_variation_expand2, hide_growtype_avatar, "
        "room_list_move_speed_rate, url, icon_mark_number, "
        "extra_icon_idx_list, hp_max_rate, mp_max_rate) values(%s)",
        buf.c_str());
    CFieldLengthChecker fc;
    fc.AddField(db->GetQueryString());
    if (!db->exec(true))
    {
        return false;
    }
    if (es->m_equipParam.m_vec264.empty())
    {
        return true;
    }
    for (std::vector< ::STAvatarAbilityVariation>::iterator avit =
             es->m_equipParam.m_vec264.begin();
         avit != es->m_equipParam.m_vec264.end(); ++avit)
    {
        db->set_query(
            "inSert into avatar_select_ability(it_no, ability_no, "
            "ability_type, rate_change, value, job, skill_index, "
            "skill_level) values(%d,%d,%d,%d,%f,%d,%d,%d)",
            0, 0, 0, 0, 0.0f, 0, 0, 0);
        if (!db->exec(true))
        {
            return false;
        }
    }
    std::string artifactName = "artifact red";
    bool isArtifact = false;
    SITEMTYPE* st = &m_itemTypeMap[artifactName];
    if (st->m_field4 == (unsigned short)key)
    {
        isArtifact = true;
    }
    else
    {
        artifactName = "artifact blue";
        st = &m_itemTypeMap[artifactName];
        if (st->m_field4 == (unsigned short)key)
        {
            isArtifact = true;
        }
        else
        {
            artifactName = "artifact green";
            st = &m_itemTypeMap[artifactName];
            if (st->m_field4 == (unsigned short)key)
            {
                isArtifact = true;
            }
        }
    }
    if (isArtifact)
    {
        this->insert_artifact_info_to_db(script);
    }
    return true;
}

bool CSyncScript::insert_stackable_item_to_db(STStackableScript* script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    EquipParamInfo* ep;
    EquipParamInfo* e2;
    MySQL* db;
    int key;
    db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    std::string typeName = ((StackScriptFull*)script)->m_flavorText;
    int out2;
    int out1;
    if (!GetType(typeName, (ENUM_CHARACTERJOB)0, 0, out1, out2, false))
    {
        return false;
    }
    MyStream buf;
    char local_1874[1024] = {0};
    char local_1474[1024];
    char acStack_1074[1024];
    char local_c74[1024];
    char local_874[1024] = {0};
    char local_474[1024] = {0};
    if (!CodePage::script2Database(
            (char*)((StackScriptFull*)script)->m_name.c_str(), local_474))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x523, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(),
                      ((StackScriptFull*)script)->m_name.c_str(),
                      "bool sync_script::CSyncScript::insert_stackable_item_to_db(STStackableScript*)",
                      0x523);
        return false;
    }
    if (!CodePage::script2Database(
            (char*)((StackScriptFull*)script)->m_explain.c_str(), local_874))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x52a, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(),
                      ((StackScriptFull*)script)->m_explain.c_str(),
                      "bool sync_script::CSyncScript::insert_stackable_item_to_db(STStackableScript*)",
                      0x52a);
        return false;
    }
    buf << ((StackScriptFull*)script)->m_itNo;
    buf << ", '" << GetEscapeString(db, local_474, local_1874) << "'";
    buf << ", '"
        << GetEscapeString(db, (char*)((StackScriptFull*)script)->m_engName.c_str(),
                    local_1474)
        << "'";
    buf << ", '" << GetEscapeString(db, local_874, acStack_1074) << "'";
    buf << ", " << out1;
    buf << ", " << out2;
    buf << ", '" << this->GetUsableCharacter(script) << "'";
    buf << ", " << ((StackScriptFull*)script)->m_class;
    buf << ", '"
        << this->GetAttachType(((StackScriptFull*)script)->m_revertType)
        << "'";
    buf << ", " << ((StackScriptFull*)script)->m_level;
    buf << ", " << 0;
    buf << ", " << ((StackScriptFull*)script)->m_createRatio;
    buf << ", " << ((StackScriptFull*)script)->m_rarity;
    buf << ", " << ((StackScriptFull*)script)->m_weight;
    buf << ", " << ((StackScriptFull*)script)->m_price;
    buf << ", " << ((StackScriptFull*)script)->m_cash;
    buf << ", " << ((StackScriptFull*)script)->m_medal;
    buf << ", " << 0;
    buf << ", " << ((StackScriptFull*)script)->m_coolTime;
    key = 0x13;
    ep = (EquipParamInfo*)((StackScriptFull*)script)->m_map4bc[key];
    if (ep == 0)
    {
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", ''";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";
        buf << ", 0.0";  buf << ", 0.0";  buf << ", 0.0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", ''";
    }
    else
    {
        buf << ", " << ep->m_hpMax;
        e2 = ep;
        buf << ", " << e2->m_mpMax;
        buf << ", " << e2->m_phyAtt;
        buf << ", " << e2->m_phyDef;
        buf << ", " << e2->m_magAtt;
        buf << ", " << e2->m_magDef;
        buf << ", " << e2->m_equipPhyAtt;
        buf << ", " << e2->m_equipPhyDef;
        buf << ", " << e2->m_equipMagAtt;
        buf << ", " << e2->m_equipMagDef;
        buf << ", " << e2->m_refFire;
        buf << ", " << e2->m_refWater;
        buf << ", " << e2->m_refDark;
        buf << ", " << e2->m_refLight;
        buf << ", " << e2->m_refAll;
        buf << ", " << e2->m_refSlow;
        buf << ", " << e2->m_refFreeze;
        buf << ", " << e2->m_refPoison;
        buf << ", " << e2->m_refPoison;
        buf << ", " << e2->m_refStun;
        buf << ", " << e2->m_refCus;
        buf << ", " << e2->m_refBlind;
        buf << ", " << e2->m_refLite;
        buf << ", " << e2->m_refSton;
        buf << ", " << e2->m_refSleep;
        buf << ", " << e2->m_refDeekement;
        buf << ", " << e2->m_refDeadlystrike;
        buf << ", " << e2->m_refBleeding;
        buf << ", " << e2->m_refConfuse;
        buf << ", " << e2->m_refHold;
        buf << ", " << e2->m_refAllStat;
        buf << ", " << e2->m_refPierce;
        buf << ", " << e2->m_refStuck;
        buf << ", " << e2->m_invenMax;
        buf << ", " << e2->m_hpRegenRate;
        buf << ", " << e2->m_mpRegenRate;
        buf << ", " << e2->m_movSpeed;
        buf << ", " << e2->m_attSpeed;
        buf << ", " << e2->m_quest;
        buf << ", " << e2->m_hitRecovery;
        buf << ", " << e2->m_jump;
        buf << ", '" << this->GetAttackElement((EquipmentParameterInfo*)e2) << "'";
        buf << ", " << e2->m_attActiveStatus;
        buf << ", " << e2->m_attActiveStatusRatio;
        buf << ", " << e2->m_attActiveStatusPow;
        buf << ", " << e2->m_attBackforce;
        buf << ", " << e2->m_attUpforce;
        buf << ", " << (unsigned int)(unsigned char)e2->m_attHpDrain;
        buf << ", " << (unsigned int)(unsigned char)e2->m_attMpDrain;
        buf << ", " << e2->m_criticalHitRate;
        buf << ", " << e2->m_stuckRate;
        buf << ", " << (unsigned int)(unsigned char)e2->m_attDefenseIgnore;
        buf << ", " << e2->m_roomListMoveSpeedRate;
        buf << ", " << e2->m_hpMaxRate;
        buf << ", " << e2->m_mpMaxRate;
        buf << ", " << e2->m_fireAttack;
        buf << ", " << e2->m_waterAttack;
        buf << ", " << e2->m_darkAttack;
        buf << ", " << e2->m_lightAttack;
        buf << ", " << e2->m_allAttack;
        buf << ", " << e2->m_rigidity;
        buf << ", " << e2->m_magicalCriticalHit;
        buf << ", " << e2->m_physAbsDmg;
        buf << ", " << e2->m_physDmgReduce;
        buf << ", " << e2->m_physAbsDef;
        buf << ", " << e2->m_magAbsDmg;
        buf << ", " << e2->m_magDmgReduce;
        buf << ", " << e2->m_magAbsDef;
        buf << ", " << e2->m_armorBreakRes;
        buf << ", '"
            << std::for_each(e2->m_levelUpSkills.begin(),
                             e2->m_levelUpSkills.end(),
                             StringMaker()).c_str()
            << "'";
    }
    buf << ", 0";
    buf << ", '"
        << GetEscapeString(db,
                    this->GetURL(
                              (char*)((StackScriptFull*)script)->m_url.c_str(),
                              (int)((StackScriptFull*)script)->m_url.length(),
                              ((StackScriptFull*)script)->m_urlLen),
                    local_c74)
        << "'";
    buf << ", " << ((StackScriptFull*)script)->m_iconMarkNumber;
    CFieldLengthChecker fc;
    db->set_query(
        "inSert into dnf_item_info(it_no, it_name, it_eng_name, it_explain, "
        "master_type, sub_type, job, class, revert, level, skill, "
        "create_ratio, rarity, weight, price, cash, medal, durability, "
        "cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, "
        "equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, "
        "ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, "
        "ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, "
        "ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, "
        "ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, "
        "inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, "
        "hit_recovery, jump, att_element, att_active_status, "
        "att_active_status_ratio, att_active_status_pow, att_backforce, "
        "att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, "
        "stuck_rate, att_defenseIgnore, room_list_move_speed_rate, hp_max_rate, "
        "mp_max_rate, fire_attack, water_attack, dark_attack, light_attack, "
        "all_attack, rigidity, magical_critical_hit, physical_absolute_damage, "
        "physical_damage_reduce, physical_absolute_defense, "
        "magical_absolute_damage, magical_damage_reduce, "
        "magical_absolute_defense, armor_break_resistance, skill_levelup, "
        "set_type, url, icon_mark_number) values(%s)",
        buf.c_str());
    fc.AddField(db->GetQueryString());
    return db->exec(true);
}

bool CSyncScript::insertCeraShopItemIntoDB(unsigned int itemIdx,
                                           unsigned int itemType, int value,
                                           int count, short c, const char* name,
                                           const char* iconName)
{
    if (m_field0 != 1)
    {
        return true;
    }
    if (itemIdx != (unsigned int)-1)
    {
        char query[255] = {0};
        switch (c)
        {
        case 1:
            memcpy(query, "tem_", 4);
            break;
        case 2:
            memcpy(query, "30\xc0\xcf", 5);
            break;
        case 3:
            memcpy(query, "\xbf\xb5\xb1\xb8", 5);
            break;
        case 4:
            memcpy(query, "\xbf\xa5\xba\xed\xb7\xbd", 7);
            break;
        }
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
        db->set_query(
            "inSert into dnf_item_price(ipg_no, it_no, it_cnt, cera_price) "
            "values(%d,%d,%d,%d)",
            itemIdx, itemType, value, count);
        return db->exec(true);
    }
    return true;
}
}

class CDataManagerExt
{
public:
    char m_pad[0x4a50];
    int m_amplifyRate;  // +0x4a50 (float 位型)
    char m_pad4a54[0x41c];
    std::map<int, std::map<int, BaseItemValue> > m_baseItemInfo;  // +0x4e70
};

// ====================================================================

CItemList::CItemList()
{
}

CItemList::~CItemList()
{
    destroy();
}

void CItemList::destroy()
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it = m_itemMap.begin();
    __gnu_cxx::hash_map<int, CItem*>::iterator end = m_itemMap.end();
    while (it != end)
    {
        CItem* item = it->second;
        delete item;
        ++it;
    }
    m_itemMap.clear();
    if (m_gradeItemList)
    {
        delete[] m_gradeItemList;
        m_gradeItemList = 0;
    }
}

CItem* CItemList::find_item(int idx)
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it;
    it = m_itemMap.find(idx);
    if (it == m_itemMap.end())
    {
        printf("[CItemList::find_item] Can't find item : index(%d)\n", idx);
        return 0;
    }
    return it->second;
}

CItem* CItemList::find_item(const char* name)
{
    for (__gnu_cxx::hash_map<int, CItem*>::iterator it = m_itemMap.begin();
         it != m_itemMap.end(); ++it)
    {
        CItem* item = it->second;
        if (strcmp(item->GetItemName(), name) == 0)
        {
            return item;
        }
    }
    return 0;
}

CCeraShopGoods* CItemList::FindGoods(int goodsIdx)
{
    __gnu_cxx::hash_map<int, CCeraShopGoods*>::iterator it;
    __gnu_cxx::hash_map<int, CCeraShopGoods*>::iterator e = m_goodsMap.end();
    it = m_goodsMap.find(goodsIdx);
    if (it == e)
    {
        return 0;
    }
    return it->second;
}

BonusItem* CItemList::GetProperBonusItem(unsigned int bonusIdx)
{
    for (unsigned int i = 0; i < m_bonusItem.size(); ++i)
    {
        if (m_bonusItem[i].m_min <= bonusIdx && bonusIdx < m_bonusItem[i].m_max)
        {
            return &m_bonusItem[i];
        }
    }
    return 0;
}

void CItemList::getCeraShopBonusItem(int idx, unsigned long& outItem,
                                     unsigned int& outCount) const
{
    outItem = (unsigned long)-1;
    outCount = 0;
    int rand;
    std::vector<stCeraShopBonusInfo_t>::const_iterator it =
        m_bonusList.begin();
    std::vector<stCeraShopBonusInfo_t>::const_iterator end =
        m_bonusList.end();
    while (it != end)
    {
        if (it->m_field0 <= idx && idx <= it->m_field4)
        {
            rand = get_rand_int(1000);
            int sum = 0;
            std::vector<stCeraShopBonusItemInfo_t>::const_iterator it2 =
                it->m_items.begin();
            std::vector<stCeraShopBonusItemInfo_t>::const_iterator end2 =
                it->m_items.end();
            while (it2 != end2)
            {
                sum = it2->m_field8 + sum;
                if (sum > rand)
                {
                    // ORIG 仅写 64 位出参低 32 位（int→ulong 不做零扩展高位写）。
                    (unsigned int&)outItem = it2->m_field0;
                    outCount = (unsigned short)it2->m_field4;
                    return;
                }
                ++it2;
            }
        }
        ++it;
    }
}

void CItemList::get_lotto_item(int idx, char* out)
{
    Inven_Item* pItem = (Inven_Item*)out;
    CItem* item = find_item(idx);
    if (!item)
    {
        pItem->m_addInfo = 0;
        pItem->set_add_info(0);
        pItem->m_fieldb = 0;
        pItem->m_field1 = 0;
        pItem->ResetItemAttr();
        return;
    }
    if (!item->is_stackable())
    {
        pItem->m_addInfo = 0;
        pItem->set_add_info(0);
        pItem->m_fieldb = 0;
        pItem->m_field1 = 1;
        pItem->ResetItemAttr();
    }
    else
    {
    if (((CStackableItem*)item)->m_nItemType != 6 &&
        ((CStackableItem*)item)->m_nItemType != 7 &&
        ((CStackableItem*)item)->m_nItemType != 0xd)
        {
            pItem->m_addInfo = 0;
            pItem->set_add_info(0);
            pItem->m_fieldb = 0;
            pItem->m_field1 = 2;
            pItem->ResetItemAttr();
        }
        else
        {
            ((CStackableItem*)item)->get_lotto_item(out);
        }
    }
}

void CItemList::create_item(ENUM_ITEM_CREATE_TYPE type, Inven_Item& item,
                            int param)
{
    item.set_add_info(param);
    CItem* pItem = find_item(item.m_addInfo);
    if (!pItem)
    {
        LogManager::logFormat(1, "item.cpp",
            __PRETTY_FUNCTION__,
            0x98f, "find_item(%d)", item.m_addInfo);
        item.m_addInfo = -1;
        item.set_add_info(0);
        item.m_fieldb = 0;
        item.m_field1 = 0;
        item.ResetItemAttr();
        return;
    }
    pItem->make_item(item);
    if (pItem->is_stackable())
    {
        if (item.get_add_info() < 0)
        {
            item.set_add_info(0);
        }
        if (item.get_add_info() > 999)
        {
            cMyTrace(
                __PRETTY_FUNCTION__,
                0x9a1, 5)(
                "CItemList::create_item, add_info >= STACK_LIMIT_ERROR , %d ITEM #%d",
                item.get_add_info(), item.m_addInfo);
        }
    }
    item.m_amp.reset();
    if (type == 1)
    {
        if (GlobalData::s_itemAmplifier_->checkInvestableItem(pItem, item, 0))
        {
            GlobalData::s_itemAmplifier_->generateAmplifyItem(pItem, item);
        }
        if (item.m_field1 == 1)
        {
            int equip = ((CEquipItem*)pItem)->get_endurance();
            int rand = get_rand_int(100);
            int rate = ((CDataManagerExt*)G_CDataManager())->m_amplifyRate;
            int bonus = (int)((float)equip * ((float)(100 - rate) / 100.0f));
            int result = (int)((float)equip + (float)bonus * ((float)rand / 100.0f));
            if (result < 0)
            {
                result = 1;
            }
            else if (result > equip)
            {
                result = equip;
            }
            item.m_fieldb = (unsigned short)result;
        }
    }
}

bool CItemList::insert_item(int group, int idx, CItem* item)
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it;
    it = m_gradeItemList[group].find(idx);
    if (it != m_gradeItemList[group].end())
    {
        m_gradeItemList[group].insert(std::pair<const int, CItem*>(idx, item));
        m_itemMap.insert(std::pair<const int, CItem*>(idx, item));
        if (item->GetQuestItemDropBouns())
        {
            m_lottoCount.insert(
                std::make_pair(item->GetQuestItemDropBouns(), (int)idx));
        }
        typedef bool (*VCheckFn)(void*);
        if ((*(VCheckFn*)(*(char**)item + 0x4c))(item))
        {
            BaseItemKey key(item->get_rarity(), item->getUsableLevel(),
                            item->getItemGroupName());
            std::map<int, std::map<int, BaseItemValue> >::iterator it2 =
                ((CDataManagerExt*)G_CDataManager())
                    ->m_baseItemInfo.find(*(int*)&key);
            int value = item->get_index();
            if (it2 !=
                ((CDataManagerExt*)G_CDataManager())->m_baseItemInfo.end())
            {
                it2->second.insert(std::make_pair<int, BaseItemValue&>(
                    item->get_grade(), *(BaseItemValue*)&value));
            }
            else
            {
                std::map<int, BaseItemValue> inner;
                inner.insert(std::make_pair<int, BaseItemValue&>(
                    item->get_grade(), *(BaseItemValue*)&value));
                ((CDataManagerExt*)G_CDataManager())
                    ->m_baseItemInfo.insert(
                        std::make_pair<int&, std::map<int, BaseItemValue>&>(
                            *(int*)&key, inner));
            }
        }
        return true;
    }
    LogManager::logFormat(1, "item.cpp",
        __PRETTY_FUNCTION__, 0x81f,
        "Insert Fail Item Group[%d] Index: %d", group, idx);
    return false;
}

bool CItemList::init()
{
    m_gradeItemList = new __gnu_cxx::hash_map<int, CItem*, __gnu_cxx::hash<int>,
                        std::equal_to<int>,
                        std::allocator<CItem*> >[0xc9];
    if (!m_gradeItemList)
    {
        LogManager::logFormat(1, "item.cpp", __PRETTY_FUNCTION__, 0x78c,
            "m_pGradeItemList = new ITEM_LIST[ITEM_GRADE_COUNT] fail");
        return false;
    }
    if (!load_item_files())
    {
        LogManager::logFormat(1, "item.cpp", __PRETTY_FUNCTION__, 0x792,
            "load_item_files() fail");
        return false;
    }
    ((ExtreamDungeon::CCompound_ExtreamDun_Item*)
        GlobalData::s_pCompoundBindCube)->after_addItem();
    if (!Load_Goods_File(G_CDataManager()->GetCeraShopGoodsName()))
    {
        LogManager::logFormat(1, "item.cpp", __PRETTY_FUNCTION__, 0x7a5,
            "Load_Goods_File() of ItemVendingMachine fail ");
        return false;
    }
    if (!Load_Goods_File(G_CDataManager()->SuddenShopFileName1()))
    {
        LogManager::logFormat(1, "item.cpp", __PRETTY_FUNCTION__, 0x7c8,
            "SuddenShopFileName1() of ItemVendingMachine fail ");
        return false;
    }
    if (!Load_Goods_File(G_CDataManager()->SuddenShopFileName2()))
    {
        LogManager::logFormat(1, "item.cpp", __PRETTY_FUNCTION__, 0x7ce,
            "SuddenShopFileName2() of ItemVendingMachine fail ");
        return false;
    }
    if (!Load_Goods_File(G_CDataManager()->SuddenShopFileName3()))
    {
        LogManager::logFormat(1, "item.cpp", __PRETTY_FUNCTION__, 0x7d4,
            "SuddenShopFileName3() of ItemVendingMachine fail ");
        return false;
    }
    return true;
}

bool CItemList::load_item_files()
{
    // ORIG 控制流：装备表循环内失败立即 return false；path 为空或表尾则
    // 转入可堆叠表循环；可堆叠表尾/空 path 时 DisplayMaxLength 后成功返回。
    STStackableScript stackScript;
    CEquipItem* pEquip = 0;
    CStackableItem* pStack = 0;
    const char* path = 0;
    std::map<int, std::string>::iterator it;

    sync_script::GetInstanceSyncScript()->truncate_dnf_item_info_table();

    it = sync_script::g_stEquipmentScriptFileList.begin();
    while (it != sync_script::g_stEquipmentScriptFileList.end())
    {
        STEquipmentScript eqScript;
        path = it->second.c_str();
        if (!path)
        {
            break;
        }
        if (!ImportEquipmentScript(&eqScript, path, false, 0, it->first))
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x8e8,
                "ImportEquipmentScript() fail file - %s", path);
            return false;
        }
        sync_script::CSyncScript* sync = sync_script::GetInstanceSyncScript();
        if (!sync->insert_equiptment_item_to_db(&eqScript, 0, 0, 0) &&
            eqScript.m_field1b4 != 0x16 && eqScript.m_field1b4 != 0x17 &&
            eqScript.m_field1b4 != 0x18 && eqScript.m_field1b4 != 0x19)
        {
            printf("insert_equiptment_item_to_db() fail file - %s", path);
        }
        pEquip = new CEquipItem;
        if (pEquip == 0)
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x903,
                "new CEquipItem error");
            return false;
        }
        pEquip->set_item(eqScript);
        if (!pEquip->verify())
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x90a,
                "Equip item verify fail. [%s]", path);
            return false;
        }
        insert_item(pEquip->get_grade(), pEquip->get_index(), pEquip);
        ((ExtreamDungeon::CCompound_ExtreamDun_Item*)
            GlobalData::s_pCompoundBindCube)->check_N_addItem(*pEquip);
        ++it;
    }

    it = sync_script::g_stStackableScriptFileList.begin();
    while (it != sync_script::g_stStackableScriptFileList.end())
    {
        path = it->second.c_str();
        if (!path)
        {
            sync_script::DisplayMaxLength();
            return true;
        }
        if (!ImportStackableScript(&stackScript, path, false, it->first))
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x926,
                "importStackableScript fail file - %s", path);
            return false;
        }
        if (GlobalInstance<Arad_GiftItem_Set>::inst()->open(&stackScript))
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x931,
                "[ERROR] Arad GiftItem Setting!! [item_idx:%d][levelt:%d]",
                stackScript.m_field0, stackScript.m_field5c);
            return false;
        }
        if (!sync_script::GetInstanceSyncScript()
                 ->insert_stackable_item_to_db(&stackScript))
        {
            printf("insert_stackable_item_to_db() fail file - %s", path);
        }
        pStack = new CStackableItem;
        if (pStack == 0)
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x941,
                "ptemp_stack_data is null");
            return false;
        }
        pStack->set_item(stackScript);
        if (!pStack->verify())
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x948,
                "Stackable item verify fail. [%s]", path);
            return false;
        }
        if (pStack->GetItemType() == 7 &&
            pStack->m_lotto.m_items.size() != pStack->m_lotto.m_counts.size())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x94f, 5);
            trace("Stackable item set_item fail (file [%s])", path);
            return false;
        }
        if (pStack->GetItemType() == 0xd &&
            pStack->m_lotto.m_items.size() != pStack->m_lotto.m_counts.size())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x957, 5);
            trace("Stackable item set_item fail 2 (file [%s])", path);
            return false;
        }
        insert_item(pStack->get_grade(), pStack->get_index(), pStack);
        ++it;
    }
    sync_script::DisplayMaxLength();
    return true;
}

bool CItemList::Load_Goods_File(const char* path)
{
    // ORIG 形态：16 个 vector<STCeraShopItem> 桶；内层循环先缓存 count 与
    // shopList[i][j] 引用（ix 调用位次与 ORIG 一致），再逐字段访问。
    std::vector<STCeraShopItem> shopList[0x10];

    if (!importCashShopItemList((char*)path, shopList, m_bonusList, 0, 0, 0, 0))
    {
        LogManager::logFormat(1, "item.cpp",
            __PRETTY_FUNCTION__, 0xa76,
            "importCashShopItemList() fail file - %s", path);
        return false;
    }
    sync_script::GetInstanceSyncScript()->truncateCeraShopItemTable();
    for (int i = 0; i < 0x10; ++i)
    {
        const int count = (int)shopList[i].size();
        for (int j = 0; j < count; ++j)
        {
            CCeraShopGoods* goods = new CCeraShopGoods(shopList[i][j]);
            m_goodsMap[shopList[i][j].m_field8] = goods;
            STCeraShopItem& item = shopList[i][j];
            int value;
            if (item.m_field14 == 1)
            {
                value = item.m_field38;
            }
            else
            {
                CEquipItem* equip = (CEquipItem*)
                    m_goodsMap[shopList[i][j].m_field8]->GetRelatedItem();
                std::vector<stAvatarTypeSelect_t>* avatarList =
                    (std::vector<stAvatarTypeSelect_t>*)
                    equip->getAvatarTypeSelect();
                value = item.m_field38;
                if (item.m_fieldc == 0 && item.m_field8 != -1 &&
                    !avatarList->empty())
                {
                    value =
                        (*avatarList)[(int)(short)item.m_field22 - 1].m_fieldc;
                }
            }
            if (value != 0 &&
                ((short)item.m_field22 == 1 || (short)item.m_field22 == 0))
            {
                CItem* pItem = G_CDataManager()->find_item(item.m_field14);
                if (pItem != 0 && item.m_field8 != -1)
                {
                    const char* iconName = pItem->GetItemIconName();
                    const char* name = pItem->GetItemName();
                    if (!sync_script::GetInstanceSyncScript()
                             ->insertCeraShopItemIntoDB(
                                 item.m_field8, item.m_field14, item.m_field1c,
                                 value, (short)item.m_field22, name, iconName))
                    {
                        LogManager::logFormat(
                            1, "item.cpp",
                            __PRETTY_FUNCTION__,
                            0xb01, "insertCeraShopItemIntoDB() fail IPG - %d",
                            item.m_field8);
                        return false;
                    }
                }
            }
            if (shopList[i][j].m_field10 != 0)
            {
                m_lottoMap.insert(std::make_pair<unsigned long&, int&>(
                    (unsigned long&)shopList[i][j].m_field10,
                    shopList[i][j].m_field14));
            }
        }
    }
    return true;
}

// ============================================================================
// Arad_GiftItem_Set::open（ORIG 0x081a2f1e，_ZN17Arad_GiftItem_Set4openEP17STStackableScript）
// 语义：解析 ST_EVENT_GIFT_ITEM* 脚本，把可用的"礼物槽位"登记进 this+0x04 的
// map<long long, Data>。key = ((long long)kind << 32) | ((uint)field5c + (slot << 16))。
// 返回 0 表示成功；0x122 表示 key 已存在（重复登记）；脚本未启用/未知类型返回 0。
// 依据 docs/class_func_reports/Arad_GiftItem_Set/open.md 逐分支恢复。
// ============================================================================
// ============================================================================
// Arad_GiftItem_Set::Arad_GiftItem_Set（ORIG 0x081a2a68，247 insn）
// 语义：填 +0x1c 的 map<string,int>：ST_EVENT_GIFT_ITEM{,2..6} → kind 1..6。
//（依据 ctor 报告逐条 make_pair→insert 转写。）
// ============================================================================
// ORIG T 0x081a2a68（247 insn）：基类 Arad_ScriptLoader + vptr + 两 map 构造后，
// 6 条 "ST_EVENT_GIFT_ITEM"[2..6] → kind 1..6 登记（ctor 报告逐条实证）。
Arad_GiftItem_Set::Arad_GiftItem_Set()
    : m_map(), m_kindMap()
{
    m_kindMap.insert(std::make_pair(std::string("ST_EVENT_GIFT_ITEM"), 1));
    m_kindMap.insert(std::make_pair(std::string("ST_EVENT_GIFT_ITEM2"), 2));
    m_kindMap.insert(std::make_pair(std::string("ST_EVENT_GIFT_ITEM3"), 3));
    m_kindMap.insert(std::make_pair(std::string("ST_EVENT_GIFT_ITEM4"), 4));
    m_kindMap.insert(std::make_pair(std::string("ST_EVENT_GIFT_ITEM5"), 5));
    m_kindMap.insert(std::make_pair(std::string("ST_EVENT_GIFT_ITEM6"), 6));
}

// ORIG T 0x081a2dea：先写 vptr（基类 Arad_ScriptLoader vtable），再逆序析构
// +0x1c map<string,int> 与 +0x04 map<long long,Data>（无虚析构，按值析构）。
Arad_GiftItem_Set::~Arad_GiftItem_Set()
{
}

int Arad_GiftItem_Set::open(STStackableScript* script)
{
    // ORIG：if (*(int*)(param_1 + 0x178) != 0) —— Arad_STStackableScript.m_0（+0x178）
    if (script->m_arad178.m_0 == 0)
    {
        return 0;
    }

    // ORIG：kind = str_to_enum(c_str(script + 0x180))——调用真实 str_to_enum。
    const char* name = script->m_arad178.m_str8.c_str();
    const int kind = str_to_enum(name);
    if (kind == 0)
    {
        return 0;
    }

    const int itemId = script->m_field0;                      // *(param_1 + 0x00)
    const unsigned int base = (unsigned int)script->m_field5c; // *(param_1 + 0x5c)

    // ORIG：for (local_10 = 0; local_10 != 0xb; ++local_10)，读取 +0x38 起 11 字节
    for (int i = 0; i != 0xb; ++i)
    {
        if (((const char*)script + 0x38)[i] != 0)
        {
            const long long key =
                (long long)base +
                (long long)(unsigned int)(i << 16) +
                ((long long)kind << 32);
            Data data;
            data.m_item = itemId;
            data.m_str4 = script->m_arad178.m_strc;   // +0x184
            data.m_str8 = script->m_arad178.m_str10;  // +0x188
            std::pair<std::map<long long, Data>::iterator, bool> inserted =
                m_map.insert(std::make_pair(key, data));
            if (!inserted.second)
            {
                return 0x122;   // 重复登记
            }
        }
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x127, 0)(
        "[!] Arad GiftItem Loding Sucess!! Item:%d", itemId);
    return 0;
}

// ============================================================================
// Arad_GiftItem_Set::str_to_enum（ORIG 0x081a2e3e，_ZN17Arad_GiftItem_Set11str_to_enumEPKc）
// 语义：在 this+0x1c 的 map<string,int> 中按 name 查找，命中返回 second，否则 0。
// ============================================================================
int Arad_GiftItem_Set::str_to_enum(const char* name)
{
    if (name == 0)
        return 0;
    std::map<std::string, int>::iterator it = m_kindMap.find(std::string(name));
    if (it == m_kindMap.end())
        return 0;
    return it->second;
}

// ============================================================================
// Arad_GiftItem_Set::find（ORIG 0x081a311e，_ZN17Arad_GiftItem_Set4findEiii）
// 语义：按 64 位 key = ((long long)kind << 32) | (((long long)job << 16) + usableLevel)
// 查 this+0x04 的 map<long long, Data>，命中返回 &it->second，否则 0。
// ============================================================================
Arad_GiftItem_Set::Data* Arad_GiftItem_Set::find(int kind, int job, int usableLevel)
{
    const long long key =
        ((long long)kind << 32) |
        (((long long)job << 16) + (long long)usableLevel);
    std::map<long long, Data>::iterator it = m_map.find(key);
    if (it == m_map.end())
        return 0;
    return &it->second;
}

// ============================================================================
// Arad_GiftItem_Set::level_check（ORIG 0x081a31c4，_ZN17Arad_GiftItem_Set11level_checkEP5CUserPK14CStackableItem）
// 语义：
//   kind  = str_to_enum(item->m_str150.c_str())
//   job   = user->get_charac_job()
//   ulv   = item->getUsableLevel()
//   data  = find(this, kind, job, ulv)
//   data==0 -> 1；job 不可用（find 未命中）则报 "user level limit" 日志并返回 0；
//   user->get_charac_level() < ulv -> 报日志并返回 0；否则 1。
// NOTE：汇编中 get_charac_level 比较对象是 CUserCharacInfo 基类，故对 user 强转访问。
// ============================================================================
int Arad_GiftItem_Set::level_check(CUser* user, const CStackableItem* item)
{
    if (user == 0 || item == 0)
        return 1;
    int kind = str_to_enum(item->m_str150.c_str());
    int job = user->get_charac_job();
    int ulv = item->getUsableLevel();
    Data* data = find(kind, job, ulv);
    if (data == 0)
    {
        return 1;
    }
    int userLevel = user->get_charac_level();
    if (userLevel < ulv)
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x14b, "[ERROR] user level limit charac_no(%d)", userLevel);
        return 0;
    }
    return 1;
}

// ============================================================================
// ExtreamDungeon::CCompound_ExtreamDun_Item（ORIG bindsphere.etc 域）
// 类布局（ctor 0x832c402 / BindCube_ScriptData ctor 0x832e058 实证）：
//   +0x000 CMTRand*（new CMTRand(time)，对象 0x9c8）
//   +0x004 BindCube_ScriptData m_data（0xb88：+0x30 合成行表 0x3b×4、
//          +0xb40 结果表 ×4、+0xb70 强制规则 map——绝对偏移即 +0xb74）
//   +0xb8c BindCube_LvBonusScriptData m_lvBonus[2]（各 0x30，普通/白银两套）
// ============================================================================
namespace ExtreamDungeon
{

// ---- ORIG 域常量（nm：_ZN14ExtreamDungeonL…E，r 段；值自 ORIG rodata 读取）----
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
static const int GROUP_NUM_MAX = 59;          // 0x08bc4390 (0x3b)
static const int RARITY_RATIO_MAX = 10000;    // 0x08bc438c (0x2710)
static const int BIND_SPHERE_INDEX = 1251;    // 0x08bc4380 (0x4e3)
static const int GOLD_BIND_SPHERE_INDEX = 1252;    // 0x08bc4384 (0x4e4)
static const int SILVER__BIND_SPHERE_INDEX = 2670287;  // 0x08bc4388 (0x28becf)

forceRule_ResultItem::forceRule_ResultItem(bool flag, int value)   // ORIG 0x832e00a
{
    m_flag = flag;
    m_value = value;
}

OUTPUT_ITEM_STR::OUTPUT_ITEM_STR(int usableLevel, int index)       // ORIG 0x832dfbe
{
    m_usableLevel = usableLevel;
    m_index = index;
}

bool OUTPUT_ITEM_STR::operator<(const OUTPUT_ITEM_STR& other) const
{
    if (m_usableLevel == other.m_usableLevel)
    {
        return m_index < other.m_index;
    }
    return m_usableLevel < other.m_usableLevel;
}

// 子脚本对象生命周期全部由具名容器成员承担（与 ORIG ctor/dtor 的逐元素
// 构造/逆序析构一致；此处仅提供符号落点）。
BindCube_ScriptData::BindCube_ScriptData() {}
BindCube_ScriptData::~BindCube_ScriptData() {}
BindCube_LvBonusScriptData::BindCube_LvBonusScriptData() {}
BindCube_LvBonusScriptData::~BindCube_LvBonusScriptData() {}

// ── 构造（ORIG 0x832c402）：成员初始化序 m_data → m_lvBonus[2]，
//    随后 body 内 new CMTRand(time(0)) 存 +0x00。──
CCompound_ExtreamDun_Item::CCompound_ExtreamDun_Item()
{
    const unsigned long seed = time(0);
    m_rand = new CMTRand(seed);
}

CCompound_ExtreamDun_Item::~CCompound_ExtreamDun_Item()
{
    // ORIG 0x832c524：delete 随机数句柄并置空；m_lvBonus[2]/m_data 由成员
    // 析构按声明逆序销毁（LvBonus 逆序 → ScriptData），顺序与 ORIG 一致。
    if (m_rand != 0)
    {
        delete m_rand;
    }
    m_rand = 0;
}

void CCompound_ExtreamDun_Item::after_addItem()                     // ORIG 0x832de18
{
    for (int i = 1; i < GROUP_NUM_MAX; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::sort(m_data.m_bindItems[i][j].begin(),
                      m_data.m_bindItems[i][j].end());
        }
    }
    for (int k = 0; k < 4; ++k)
    {
        std::sort(m_data.m_resultItems[k].begin(), m_data.m_resultItems[k].end());
    }
}

bool CCompound_ExtreamDun_Item::_AddItem(CEquipItem& item)          // ORIG 0x832dd1c
{
    // cat=_convertOutputItem(GetUsableEquipmentType)；cat==4 → 不可入表；
    // 否则 {usableLevel,index} 同记结果表[cat] 与合成行表[group][cat]。
    const int cat = _convertOutputItem(
        (ENUM_UNUSABLE_EQUIPMENT_TYPE)item.GetUsableEquipmentType());
    if (cat == 4)
    {
        return false;
    }
    OUTPUT_ITEM_STR entry(item.getUsableLevel(), item.get_index());
    m_data.m_resultItems[cat].push_back(static_cast<OUTPUT_ITEM_STR&&>(entry));
    OUTPUT_ITEM_STR entry2(item.getUsableLevel(), item.get_index());
    m_data.m_bindItems[item.getItemGroupName()][cat].push_back(
        static_cast<OUTPUT_ITEM_STR&&>(entry2));
    return true;
}

bool CCompound_ExtreamDun_Item::_isPossibleResultItem(              // ORIG 0x832db4e
    CEquipItem& item) const
{
    if ((unsigned int)item.getItemGroupName() > 0x3a ||
        item.getItemGroupName() <= 0)
    {
        return false;
    }

    // 强制规则命中即一票裁决：flag==0（强制允许）→ true，非零 → false。
    std::map<int, forceRule_ResultItem>::const_iterator endIt =
        m_data.m_forceRules.end();
    std::map<int, forceRule_ResultItem>::const_iterator it =
        m_data.m_forceRules.find(item.get_index());
    if (it != endIt)
    {
        if (it->second.m_flag != 0)
        {
            return false;
        }
        if (it->second.m_flag != 1)
        {
            return true;
        }
    }

    if (_convertInputItem(
            (ENUM_UNUSABLE_EQUIPMENT_TYPE)item.GetUsableEquipmentType()) == 2 &&
        item.getItemGroupName() > 0x19 && item.getItemGroupName() < 0x33)
    {
        return false;
    }

    if (item.getUsableLevel() > 0x46)
    {
        return false;
    }

    if (item.get_gen_rate() == 0)
    {
        return _convertInputItem(
                   (ENUM_UNUSABLE_EQUIPMENT_TYPE)
                       item.GetUsableEquipmentType()) == 1;
    }

    // gen_rate!=0 → 虚表槽 +0x4c 回调裁决（非零→false，零→true）。
    typedef char (*VCheckFn)(const void*);
    void* const* vtbl = *(void* const* const*)&item;
    VCheckFn fn = (VCheckFn)vtbl[0x4c / 4];
    return fn((const void*)&item) == 0;
}

bool CCompound_ExtreamDun_Item::check_N_addItem(CEquipItem& item)   // ORIG 0x832db00
{
    if (_isPossibleResultItem(item) != true)
    {
        return false;
    }
    if (_AddItem(item) != true)
    {
        return false;
    }
    return true;
}

void CCompound_ExtreamDun_Item::ForceResultItemRule_Setting(        // ORIG 0x832d784
    int idx, bool flag, int value)
{
    m_data.m_forceRules[idx] = forceRule_ResultItem(flag, value);
}

int CCompound_ExtreamDun_Item::_convertOutputItem(                  // ORIG 0x832c5ac
    ENUM_UNUSABLE_EQUIPMENT_TYPE eqType) const
{
    // docs/class_func_reports/ExtreamDungeon__CCompound_ExtreamDun_Item/_convertOutputItem.md
    // ORIG 为二分 cmp 决策树（==2 先、>2 分支、<=2 只查 ==1），有符号 jg。
    if ((int)eqType == 2)
        return 2;
    if ((int)eqType > 2)
    {
        if ((int)eqType == 3)
            return 3;
        if ((int)eqType == 9)
            return 0;
    }
    else
    {
        if ((int)eqType == 1)
            return 1;
    }
    return 4;
}

int CCompound_ExtreamDun_Item::_convertInputItem(                   // ORIG 0x832c620
    ENUM_UNUSABLE_EQUIPMENT_TYPE eqType) const
{
    // 同型决策树；映射：1→0, 2→1, 3→2, 9→0, 其余→3。
    if ((int)eqType == 2)
        return 1;
    if ((int)eqType > 2)
    {
        if ((int)eqType == 3)
            return 2;
        if ((int)eqType == 9)
            return 0;
    }
    else
    {
        if ((int)eqType == 1)
            return 0;
    }
    return 3;
}

int CCompound_ExtreamDun_Item::checkCompoundable(                   // ORIG T 0x832d7d6
    Inven_Item* items, const CEquipItem** refs, char kind) const
{
    int inputType[2];
    for (int i = 0; i < 2; ++i)
    {
        if (items[i].isEmpty() || refs[i] == 0)
        {
            return 0x31;
        }
        inputType[i] = _convertInputItem(
            (ENUM_UNUSABLE_EQUIPMENT_TYPE)refs[i]->GetUsableEquipmentType());
        // vtable +0x4c 槽（P5 直调）
        if (refs[i]->IsRandomOption())
        {
            return 7;
        }
        if (inputType[i] == 3)
        {
            return 0x17;
        }
        if (inputType[i] == 2 && refs[i]->getItemGroupName() > 0x19 &&
            refs[i]->getItemGroupName() <= 0x32)
        {
            return 0x17;
        }
        if (!items[i].m_amp.isIdentified())
        {
            return 0x13;
        }
        if ((unsigned char)items[i].m_field0 != 1)
        {
            return 0x12;
        }
        int group = refs[i]->getItemGroupName();
        if (group > 0 && group > 0x3a)
        {
            return 0x11;
        }
        if (refs[i]->GetAttachType() == 1)
        {
            return 1;
        }
        // vtable +0x48 槽（P5 直调）
        if (refs[i]->PeriodAfterUnsealing() != 0)
        {
            return 1;
        }
        if (items[i].m_upgradeSep.IsTradeRestriction())
        {
            return 1;
        }
    }
    if (inputType[0] != inputType[1])
    {
        return 0x16;
    }
    if (kind == 0)
    {
        if (inputType[0] != 0)
        {
            return 0x11;
        }
    }
    else if (kind == 1 || kind == 3)
    {
        if (inputType[0] != 1 && inputType[0] != 2)
        {
            return 0x11;
        }
    }
    else
    {
        return 0x11;
    }
    if (kind == 3)
    {
        if (refs[0]->getUsableLevel() > 0x3c || refs[1]->getUsableLevel() > 0x3c)
        {
            return 0x11;
        }
    }
    else
    {
        if (refs[0]->getUsableLevel() > 0x45 || refs[1]->getUsableLevel() > 0x45)
        {
            return 0x11;
        }
    }
    return 0;
}

bool CCompound_ExtreamDun_Item::makeCompoundItem(                   // ORIG T 0x832ccc0
    const CEquipItem** refs, char select, Inven_Item& out, eBindSphereType bind)
{
    out.reset();
    int baseLevel = _get_base_level(refs);
    int inRarity = _convertInputItem(
        (ENUM_UNUSABLE_EQUIPMENT_TYPE)refs[0]->GetUsableEquipmentType());

    // 双次 randInt()%1000000 + 1（magic 常量 0x431bde83 实证）
    unsigned int r0 = (unsigned int)(m_rand->randInt() % 1000000) + 1;
    unsigned int r1 = (unsigned int)(m_rand->randInt() % 1000000) + 1;
    (void)r0;  // ORIG 计算了两个随机数，仅第二个参与判定

    unsigned int outputRarity = 4;
    if (inRarity == 0)
    {
        outputRarity = (unsigned int)_determine_rarity(r1,
            (ENUM_INPUTITEMS_RARITY)inRarity);
    }
    else
    {
        outputRarity = (unsigned int)_convertInput_To_Output(
            (ENUM_INPUTITEMS_RARITY)inRarity);
    }

    std::pair<unsigned int, unsigned int> bonus = _determine_level_bonus(
        (ENUM_INPUTITEMS_RARITY)inRarity, (unsigned int)baseLevel,
        (ENUM_OUTPUTITEMS_RARITY)outputRarity, bind);

    int found = 0;
    if (!(bonus.second == 0xa && inRarity != 1 && (baseLevel % 10) > 4))
    {
        found = _FindItemAll_BindSphere(refs, select,
            (ENUM_OUTPUTITEMS_RARITY)outputRarity,
            std::make_pair(bonus.first + (unsigned int)baseLevel,
                           bonus.second + (unsigned int)baseLevel));
    }
    if (found == 0)
    {
        if (inRarity >= 1 && inRarity <= 2)
        {
            found = _FindItemAll_BindSphere(refs, select,
                (ENUM_OUTPUTITEMS_RARITY)outputRarity,
                std::make_pair((unsigned int)(baseLevel + 5),
                               (unsigned int)(baseLevel + 1)));
            if (found == 0)
            {
                found = _FindItemAll_BindSphere(refs, select,
                    (ENUM_OUTPUTITEMS_RARITY)outputRarity,
                    std::make_pair((unsigned int)(baseLevel + 0xa),
                                   (unsigned int)(baseLevel + 6)));
            }
        }
        else if (inRarity == 0)
        {
            found = _exception_Rare(refs, select,
                (ENUM_OUTPUTITEMS_RARITY)outputRarity, (unsigned int)baseLevel);
        }
    }
    if (found == 0)
    {
        return false;
    }
    out.m_addInfo = found;
    CItem* info = G_CDataManager()->find_item(found);
    if (info == 0)
    {
        return false;
    }
    info->make_item(out);
    return true;
}

int CCompound_ExtreamDun_Item::_get_base_level(                     // ORIG W 0x832e40c
    const CEquipItem* const* refs) const
{
    return (refs[0]->getUsableLevel() + refs[1]->getUsableLevel()) >> 1;
}

int CCompound_ExtreamDun_Item::_convertInput_To_Output(             // ORIG T 0x832c5f0
    ENUM_INPUTITEMS_RARITY inRarity) const
{
    switch ((int)inRarity)
    {
    case 1: return 2;
    case 2: return 3;
    case 0: return 0;
    default: return 4;
    }
}

int CCompound_ExtreamDun_Item::_determine_rarity(                   // ORIG T 0x832c976
    unsigned int randVal, ENUM_INPUTITEMS_RARITY inRarity) const
{
    if ((int)inRarity < 0 || (int)inRarity > 2)
    {
        return 0;
    }
    for (int i = 0; i <= 3; ++i)
    {
        if ((unsigned int)m_data.m_rarityDicision[(int)inRarity][i] >= randVal)
        {
            return i;   // 循环耗尽时返回 4（ORIG 如此）
        }
    }
    return 4;
}

std::pair<unsigned int, unsigned int>                              // ORIG T 0x832c9ca
CCompound_ExtreamDun_Item::_determine_level_bonus(
    unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity,
    unsigned int& idxOut, eBindSphereType bind) const
{
    idxOut = _determine_level_bonus_index(baseLevel, inRarity, bind);
    const std::vector<unsigned int>& rates =
        m_lvBonus[(int)bind].m_rates[(int)inRarity];
    if (idxOut == 0 || idxOut >= rates.size())
    {
        return std::pair<unsigned int, unsigned int>(0u, 0u);
    }
    return std::pair<unsigned int, unsigned int>(rates[idxOut], rates[idxOut - 1] + 1);
}

std::pair<unsigned int, unsigned int>                              // ORIG T 0x832cadc
CCompound_ExtreamDun_Item::_determine_level_bonus(
    ENUM_INPUTITEMS_RARITY inRarity, unsigned int baseLevel,
    ENUM_OUTPUTITEMS_RARITY outRarity, eBindSphereType bind) const
{
    unsigned int idx = _determine_level_bonus_index(baseLevel, inRarity, bind);
    if ((int)inRarity == 0 && (int)outRarity > 1 && (int)outRarity <= 3 && idx != 0)
    {
        --idx;
    }
    const std::vector<unsigned int>& rates =
        m_lvBonus[(int)bind].m_rates[(int)inRarity];
    if (idx == 0 || idx >= rates.size())
    {
        return std::pair<unsigned int, unsigned int>(0u, 0u);
    }
    return std::pair<unsigned int, unsigned int>(rates[idx], rates[idx - 1] + 1);
}

unsigned int CCompound_ExtreamDun_Item::_determine_level_bonus_index(  // ORIG T 0x832cc02
    unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity,
    eBindSphereType bind) const
{
    if ((int)inRarity < 0 || (int)inRarity > 2)
    {
        return 0;
    }
    unsigned int idx = 0;
    for (;;)
    {
        const std::vector<unsigned int>& rates =
            m_lvBonus[(int)bind].m_rates[(int)inRarity];
        if (rates.size() == 0)
        {
            break;
        }
        if (rates[idx] >= baseLevel)
        {
            break;
        }
        ++idx;
    }
    return idx;
}

int CCompound_ExtreamDun_Item::_exception_Rare(                     // ORIG T 0x832cffe
    const CEquipItem** refs, char select, ENUM_OUTPUTITEMS_RARITY outRarity,
    unsigned int baseLevel) const
{
    int found = 0;
    for (unsigned int g = 0; g <= 1u; ++g)
    {
        for (int c = (int)baseLevel; c >= 0; --c)
        {
            if (c == 0)
            {
                found = _FindItemAll_BindSphere(refs, select, outRarity,
                    std::make_pair(baseLevel + g * 5u + 5u, baseLevel + g * 5u + 1u));
            }
            else if (c > 1)
            {
                found = _FindItemAll_BindSphere(refs, select, outRarity,
                    std::make_pair(baseLevel + g, baseLevel + g));
            }
            if (found != 0)
            {
                return found;
            }
        }
    }
    return found;
}

bool BindTable_Predicate(                                           // ORIG T 0x832c3cc
    const OUTPUT_ITEM_STR& a, const OUTPUT_ITEM_STR& b)
{
    if (a.m_usableLevel != b.m_usableLevel)
    {
        return a.m_usableLevel < b.m_usableLevel;
    }
    return a.m_index < b.m_index;
}

int CCompound_ExtreamDun_Item::_FindSelectItem(                     // ORIG T 0x832d15e
    unsigned int group, ENUM_OUTPUTITEMS_RARITY outRarity,
    const std::pair<unsigned int, unsigned int>& range) const
{
    if ((int)group <= 0 || group > 0x3a)
    {
        return 0;
    }
    typedef std::vector<OUTPUT_ITEM_STR> ItemVec;
    // ORIG 绝对偏移 this+0x30（= &m_data + 0x2c）；头注释与绝对值差 4，
    // 按 objdump 实证偏移访问。
    typedef std::vector<OUTPUT_ITEM_STR> ItemVec;
    const ItemVec& table = *reinterpret_cast<const ItemVec*>(
        reinterpret_cast<const char*>(&m_data) + 0x2c +
        48u * group + 12u * (unsigned int)outRarity);
    OUTPUT_ITEM_STR lowKey((int)range.first, 0);
    ItemVec::const_iterator lo = std::lower_bound(
        table.begin(), table.end(), lowKey, &BindTable_Predicate);
    OUTPUT_ITEM_STR highKey((int)range.second, 0x7fffffff);
    ItemVec::const_iterator hi = std::upper_bound(
        table.begin(), table.end(), highKey, &BindTable_Predicate);
    if (lo == hi)
    {
        return 0;
    }
    return _FindItemRelateCreationRate(lo, hi);
}

int CCompound_ExtreamDun_Item::_FindRandomItem(                     // ORIG T 0x832d2ce
    ENUM_OUTPUTITEMS_RARITY outRarity,
    const std::pair<unsigned int, unsigned int>& range) const
{
    if ((int)outRarity < 0 || (int)outRarity > 3)
    {
        return 0;
    }
    typedef std::vector<OUTPUT_ITEM_STR> ItemVec;
    // ORIG 绝对偏移 this+0xb44（= &m_data + 0xb40 = m_resultItems，实证：
    // _AddItem push_back 目标亦为 +0xb44）。旧值 0xb3c 偏 4，已按 ORIG 修正。
    const ItemVec& table = *reinterpret_cast<const ItemVec*>(
        reinterpret_cast<const char*>(&m_data) + 0xb40 +
        12u * (unsigned int)outRarity);
    OUTPUT_ITEM_STR lowKey((int)range.first, 0);
    ItemVec::const_iterator lo = std::lower_bound(
        table.begin(), table.end(), lowKey, &BindTable_Predicate);
    OUTPUT_ITEM_STR highKey((int)range.second, 0x7fffffff);
    ItemVec::const_iterator hi = std::upper_bound(
        table.begin(), table.end(), highKey, &BindTable_Predicate);
    if (lo == hi)
    {
        return 0;
    }
    return _FindItemRelateCreationRate(lo, hi);
}

int CCompound_ExtreamDun_Item::_FindItemAll_BindSphere(             // ORIG T 0x832d438
    const CEquipItem** refs, char select, ENUM_OUTPUTITEMS_RARITY outRarity,
    std::pair<unsigned int, unsigned int> range) const
{
    if ((int)outRarity == 2)
    {
        return _FindRandomItem(outRarity, range);
    }
    if ((int)outRarity == 0 || (int)outRarity == 1)
    {
        int group = refs[(int)select]->getItemGroupName();
        return _FindSelectItem((unsigned int)group, outRarity, range);
    }
    return 0;
}

int CCompound_ExtreamDun_Item::_FindItemRelateCreationRate(         // ORIG T 0x832d4b4
    std::vector<OUTPUT_ITEM_STR>::const_iterator begin,
    std::vector<OUTPUT_ITEM_STR>::const_iterator end) const
{
    if (begin == end)
    {
        return 0;
    }
    std::vector<std::pair<int, unsigned int> > entries;   // (权重, itemIdx)
    int totalGen = 0;
    for (std::vector<OUTPUT_ITEM_STR>::const_iterator it = begin; it != end; ++it)
    {
        CItem* info = G_CDataManager()->find_item(it->m_index);
        if (info == 0)
        {
            continue;
        }
        std::map<int, forceRule_ResultItem>::const_iterator rule =
            m_data.m_forceRules.find(it->m_index);
        if (rule != m_data.m_forceRules.end())
        {
            entries.push_back(std::make_pair((int)rule->second.m_value,
                                             (unsigned int)it->m_index));
            totalGen += rule->second.m_value;
        }
        else
        {
            entries.push_back(std::make_pair(info->get_gen_rate(),
                                             (unsigned int)it->m_index));
            totalGen += info->get_gen_rate();
        }
    }
    unsigned int pick = 0;
    if (totalGen == 0)
    {
        if (entries.size() == 0)
        {
            return 0;
        }
        return entries[m_rand->randInt() % entries.size()].second;
    }
    pick = (unsigned int)(m_rand->randInt() % totalGen) + 1;
    unsigned int acc = 0;
    for (unsigned int i = 0; i < entries.size(); ++i)
    {
        acc += (unsigned int)entries[i].first;
        if (pick <= acc)
        {
            return entries[i].second;
        }
    }
    return 0;
}


// ── LoadScript（ORIG T 0x0832c65e）：RDAR 脚本节解析。
// [insert rarity] 丢 3 token；[rarity dicision] 写 3×4 网格；
// [level dicision]/[silver level dicision] 分别写 m_lvBonus[0]/[1]。
int CCompound_ExtreamDun_Item::LoadScript(const char* dir, const char* path)
{
    if (!loadRDARScriptFile(dir, path))
    {
        return 0x62;
    }

    std::string token;
    std::string scratch;

    while (ScanType(token, true))
    {
        if (token == "[insert rarity]")
        {
            for (int i = 0; i < 3; ++i)
            {
                ScanStr(&scratch);
            }
        }
        else if (token == "[rarity dicision]")
        {
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    m_data.m_rarityDicision[i][j] = ScanInt((bool*)0);
                }
            }
        }
        else if (token == "[level dicision]")
        {
            const unsigned int count = (unsigned int)ScanInt((bool*)0);
            for (unsigned int k = 0; k < count; ++k)
            {
                m_lvBonus[0].m_levels.push_back((unsigned int)ScanInt((bool*)0));
            }
            for (int r = 0; r < 3; ++r)
            {
                for (unsigned int k = 0; k < count; ++k)
                {
                    m_lvBonus[0].m_rates[r].push_back(
                        (unsigned int)ScanInt((bool*)0));
                }
            }
        }
        else if (token == "[silver level dicision]")
        {
            const unsigned int count = (unsigned int)ScanInt((bool*)0);
            for (unsigned int k = 0; k < count; ++k)
            {
                m_lvBonus[1].m_levels.push_back((unsigned int)ScanInt((bool*)0));
            }
            for (int r = 0; r < 3; ++r)
            {
                for (unsigned int k = 0; k < count; ++k)
                {
                    m_lvBonus[1].m_rates[r].push_back(
                        (unsigned int)ScanInt((bool*)0));
                }
            }
        }
    }
    return 0;
}

}

int CItemList::GetRestrictCode(int itemIdx)
{
    __gnu_cxx::hash_map<unsigned int, int, __gnu_cxx::hash<unsigned int>,
                        std::equal_to<unsigned int>,
                        std::allocator<int> >::iterator it =
        m_lottoMap.find((unsigned int)itemIdx);
    if (it == m_lottoMap.end())
        return 0;
    return it->second;
}

// ORIG W 0x0869476e _ZNK9CItemList21GetQuestItemDropBounsERKm：
// m_lottoCount(+0x5c, std::map<unsigned long,int>).find(key)；
// 命中返 second（mov 0x4 偏移），未命中返 0。
int CItemList::GetQuestItemDropBouns(const unsigned long& key) const
{
    std::map<unsigned long, int>::const_iterator it = m_lottoCount.find(key);
    if (it == m_lottoCount.end())
        return 0;
    return it->second;
}
