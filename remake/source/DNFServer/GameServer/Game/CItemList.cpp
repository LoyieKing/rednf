#include "LogManager.h"
#include "GameRand.h"
// df_game_r CItemList 还原（2026-08-16 / 2026-08-17 第九批 I1 收尾）。
// 成员布局以 ORIG 构造/析构为基准；函数形态以 class_func_reports/CItemList.md
// 与 class_func_reports/sync_script__CSyncScript.md 逐条对照
// （AE 口径：调用目标/全局地址归一，指令形态逐条一致）。

#include "CItemList.h"
#include "STEquipmentScript.h"   // EquipmentAniInfoScript / STEquipmentAniScript（insert ani 段真实类型）

#include "CStreamBuffer.h"        // 权威 ::CStreamBuffer（ORIG 0x86269be 域）
#include "STStackableScript.h"    // STStackableScript::m_module4bc（map<ENUM_MODULE_TYPE,
                                  //   EquipmentParameterInfo*>，ORIG 0x8628bc6 operator[] 实证）

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
// 流缓冲/字符串拼接使用权威类：
//   ::CStreamBuffer（CStreamBuffer.h，ORIG 0x86269be 域，外部 TU 提供实现）
//   sync_script::CStringMaker（声明于下方 namespace sync_script 块；实现体在
//   sync_script.cpp，ORIG 0x8375f6c/0x8626cd0 域）。ORIG 本二函数全部字段
//   输出经 CStreamBuffer::operator<<(char const*/int/unsigned/float)，
//   列表拼接经 std::for_each(..., CStringMaker)。
// ============================================================================
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


// EquipmentParameterInfo 字段镜像（TU 私有）：真名取自 dnf_item_info 列名；
// insert_stackable_item_to_db 的 m_module4bc[0x13] 值按本镜像偏移读取（ORIG 全为
// 直接偏移 load，无调用差异）。
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


 
// ===================== 外部依赖（其它 TU 提供，仅声明） =====================

namespace sync_script
{
// ---- CStringMaker（ORIG 0x8375f6c/0x8626cd0 域；实现体在 sync_script.cpp TU，
// 声明与 sync_script.h 权威头逐项一致：ctor/copy/dtor/operator()(重载)/c_str）。
// 本 TU 仅经 std::for_each<iterator<X*>, CStringMaker> 与 c_str() 使用。 ----
class CStringMaker
{
public:
    CStringMaker();
    CStringMaker(const CStringMaker&);
    ~CStringMaker();

    void operator()(int v);
    void operator()(float v);
    void operator()(EquipmentSkillDataUp v);
    void operator()(LevelUpSkill v);
    void operator()(ItemAuraInfo v);

    const char* c_str();

    std::string m_str;   // ORIG +0x00 单 string 成员
};

// 布局依据 ORIG 构造（0x861cc6c）/ 析构（0x861cd7a）：
//   +0x00 bool m_field0
//   +0x04 vector<unsigned char>（12 字节）
//   +0x10 vector<unsigned char>
//   +0x1c map<string, SITEMTYPE>
//   +0x34 map<SWEAPONTYPE, SITEMTYPE>
//   +0x4c map<SARMORTYPE, SITEMTYPE>
// 嵌套类型定义与 sync_script.h 权威头一致（ORIG mangled 为
// sync_script::CSyncScript::{SITEMTYPE,SWEAPONTYPE,SARMORTYPE} 嵌套名；
// 本地曾用全局 struct，导致 C1/D1 子对象构造 mangled 与 ORIG 不符）。
class CSyncScript
{
public:
    CSyncScript();
    ~CSyncScript();

struct SITEMTYPE
{
    SITEMTYPE()
    {
        // ORIG 0x08626c08：string@+0x8 隐式默认构造 call 先行（POD 成员无 ctor），
        // 标量清零在体（逐条 reload this + movb/movw store）。
        m_field0 = 0;
        m_field1 = 0;
        m_field2 = 0;
        m_field4 = 0;
    }
    // ORIG C1 _ZN...SITEMTYPEC1EbhhtSs：第5参 std::string 按值。
    SITEMTYPE(bool usable, unsigned char f1, unsigned char f2,
              unsigned short f4, std::string name)
    {
        // 成员默认构造 + 函数体内赋值（ORIG 形态）
        m_field0 = usable;
        m_field1 = f1;
        m_field2 = f2;
        m_field4 = f4;
        m_str8 = name;
    }
    bool m_field0;        // +0x00  // b28 BM4 P211 同型化，与 sync_script.h 同步
    unsigned char m_field1;  // +0x01
    unsigned char m_field2;  // +0x02
    unsigned short m_field4; // +0x04（ORIG GetType 读 *(ushort*)(p+4)）
    std::string m_str8;      // +0x08
};

struct SWEAPONTYPE
{
    // ORIG C1/C2：_ZN...SWEAPONTYPEC1E17ENUM_CHARACTERJOBi，双成员初始化。
    SWEAPONTYPE() : m_job(0), m_idx(0) {}
    SWEAPONTYPE(ENUM_CHARACTERJOB job, int idx) : m_job(job), m_idx(idx) {}
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
    // ORIG C1/C2 _ZN...SARMORTYPEC1EhSs：(unsigned char, std::string)。
    SARMORTYPE() : m_field0(0), m_str4("") {}
    SARMORTYPE(unsigned char f0, std::string name)
    {
        // 成员默认构造 + 函数体内赋值（ORIG 形态）
        m_field0 = f0;
        m_str4 = name;
    }
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

// [b28d-20] insert_equiptment_item_to_db（ORIG 0x0861d87e，2891/2881，nd 54）：
//   §6.2 三件套 —— p167 调用点差集 488/488 **CLEAR**（0 missing/0 extra）、
//   gap=10（全为寄存器角色互换 esi↔ebx 与一处 jmp 岛）、槽位集一致 ⇒ 语义完成、
//   byte 层 N7/N82 封顶。
bool CSyncScript::insert_equiptment_item_to_db(STEquipmentScript* script,
                                              unsigned int idx, int mode,
                                              int key)
{
    // 依据 docs/class_func_reports/sync_script__CSyncScript/
    // insert_equiptment_item_to_db.md（ORIG 0x861d87e）反编译 C 整函数转写：
    // 全部字段以权威 STEquipmentScript/STItemScript/EquipmentParameterInfo
    // 偏移直接访问，删除 EquipScriptFull 镜像布局（其声明序与权威偏移不符）。
    // switch 块序 = 源码 case 序（ORIG 物理 case0@+0xaa → case1@+0x26d →
    // case2@+0x27c，4.1 直落 case0 与跳板一致）。
    // 残差豁免（O=2898/N=2887，canon 级全部为跨版本形态）：
    //   [N18a] &buf[i] 元素址：ORIG mov/lea/shl/add/mov 5 段（结果先 eax
    //     后拷 callee-saved），4.4 恒 4 段直出 ebx（×6）；
    //   [N18a] ", '" 实参两跳：ORIG mov mem,%eax; mov %eax,%ebx，4.4 折
    //     mov mem,%ebx（×2）；
    //   [P23] GetEscapeString arg3 预计算位：ORIG +0x1001 提前 lea 入 ebx
    //     跨调用保持，4.4 挪至 push 位（等长窗移）。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG：先声明占浅槽 → local_154@-0x150（arg5&/比较/后输出），
    // local_158@-0x154（arg4&/先输出）。
    int local_154 = 0;
    int local_158 = 0;
    int foundAni;
    for (int i = 0; i < 0xb; ++i)
    {
        if (script->m_usable[i] != 0)
        {
            foundAni = i;
            break;
        }
    }
    int local_74 = 0;
    switch (mode)
    {
    // ORIG +0xaa case0 直落、case1@+0x26d、case2@+0x27c：物理块序 0,1,2
    case 0:
    {
        if (!GetType(script->m_strbc, (ENUM_CHARACTERJOB)foundAni,
                     script->m_1b8, local_158, local_154, true))
        {
            return false;
        }
        idx = (unsigned int)script->m_field0;
        // ORIG：map<int,EquipmentParameterInfo>（+0x678）逐档递归；
        // 递归实参为整对象 STEquipmentScript temp（C1 构造，m_param1c0 = it->second）。
        for (std::map<int, EquipmentParameterInfo>::iterator it =
                 script->m_paramMap678.begin();
             it != script->m_paramMap678.end(); ++it)
        {
            EquipmentParameterInfo& par = it->second;
            STEquipmentScript temp;
            temp.m_param1c0 = par;
            insert_equiptment_item_to_db(&temp, idx, 1, it->first);
        }
        break;
    }
    case 1:
        local_74 = idx;
        idx = (unsigned int)-1;
        break;
    case 2:
        local_74 = idx;
        idx = (unsigned int)-2;
        break;
    }
    // ---- socket（local_162[5..9]）：0x6cc ushort 段 + 0x6c0 avatar 段 ----
    char socketChars[5] = {0};
    int socketCount = 0;
    if (script->m_vec6cc.size() != 0)
    {
        std::vector<unsigned short>::iterator vit = script->m_vec6cc.begin();
        while (vit != script->m_vec6cc.end())
        {
            if (socketCount > 4)
            {
                break;
            }
            unsigned short usVal = *vit;
            socketChars[socketCount] = GetSocketTypeCharacter(usVal);
            ++socketCount;
            vit++;
        }
    }
    if (script->m_avatar6c0.size() != 0)
    {
        std::vector<stAvatarTypeSelect_t>::iterator ait =
                 script->m_avatar6c0.begin();
        while (ait != script->m_avatar6c0.end())
        {
            // ORIG：*it 整对象拷贝到局部（8 dword mov），再从副本读字段。
            stAvatarTypeSelect_t av = *ait;
            if (av.m_field10 > 0)
            {
                int k = 0;
                while (k < av.m_field10)
                {
                    if (socketCount > 4)
                    {
                        break;
                    }
                    socketChars[socketCount] =
                        GetSocketTypeCharacter(av.m_socketType[k]);
                    ++socketCount;
                    ++k;
                }
            }
            ait++;
        }
    }
    // ---- hide_equipment（local_1fc）：0x768 ENUM_EQUIPMENTTYPE 段 ----
    char local_1fc[64] = {0};
    if (script->m_equipTypes768.size() != 0)
    {
        int equipCount = 0;
        std::vector<ENUM_EQUIPMENTTYPE>::const_iterator eit =
                 script->m_equipTypes768.begin();
        while (eit != script->m_equipTypes768.end())
        {
            // ORIG：先 *it 存局部（local_50），两分支共用一份实参。
            int hideType = *eit;
            char tmp[8] = {0};
            if (equipCount == 0)
            {
                sprintf(tmp, "%d", hideType);
            }
            else
            {
                sprintf(tmp, "*%d", hideType);
            }
            strcat(local_1fc, tmp);
            eit++;
            ++equipCount;
        }
    }
    // ---- ani（local_2fc/3fc/4fc/5fc）----
    // ORIG：遍历 0..0xa 中 m_usable[i] 非 0 的职业，getAniInfo 为空即 break；
    // 四帧号任一非 -1 才处理；expand!=0 → (m_30,m_34)+m_ani18 → 3fc/5fc；
    // expand==0 → (m_c,m_10)+m_ani0 → 2fc/4fc（ORIG if 体为 expand，else 为普通层）。
    char local_2fc[256] = {0};
    char local_3fc[256] = {0};
    char local_4fc[256] = {0};
    char local_5fc[256] = {0};
    bool local_69 = 0;
    for (int i = 0; i < 0xb; ++i)
    {
        if (script->m_usable[i] == 0)
        {
            continue;
        }
        EquipmentAniInfoScript* aniInfo =
            script->getAniInfo((ENUM_CHARACTERJOB)i);
        if (aniInfo == 0)
        {
            break;
        }
        if (aniInfo->m_30 == -1 && aniInfo->m_34 == -1 &&
            aniInfo->m_c == -1 && aniInfo->m_10 == -1)
        {
            continue;
        }
        if (aniInfo->m_expand14 != 0)
        {
            // expand 层：帧 (m_30, m_34)，序列 m_ani18
            if (aniInfo->m_30 == -1 && aniInfo->m_34 == -1)
            {
                continue;
            }
            char tmp[0x100] = {0};
            sprintf(tmp, "%02d%02d", aniInfo->m_30, aniInfo->m_34);
            if (local_69 != 0)
            {
                char t2[8] = {0};
                sprintf(t2, "*%d", i);
                strcat(tmp, t2);
            }
            char t3[8] = {0};
            sprintf(t3, "*%d", (int)aniInfo->m_ani18.size());
            strcat(tmp, t3);
            std::vector<STEquipmentAniScript>::const_iterator ait =
                     aniInfo->m_ani18.begin();
            while (ait != aniInfo->m_ani18.end())
            {
                // ORIG：每轮先清 src/wb 两缓冲（deref 之前），去 '_' 为跨缓冲拷贝、
                // 不写终止符（预零即终止）。
                char src[0x100] = {0};
                char wb[0x100] = {0};
                const STEquipmentAniScript* entry = &*ait;
                strcat(tmp, "*");
                strcpy(src, entry->m_str60.c_str());
                int w = 0;
                unsigned int r = 0;
                while (r < strlen(src))
                {
                    if (src[r] != '_')
                    {
                        wb[w] = src[r];
                        ++w;
                    }
                    ++r;
                }
                strcat(tmp, wb);
                ait++;
            }
            if (!local_69)
            {
                strcpy(local_3fc, tmp);
                local_69 = 1;
            }
            else
            {
                strcpy(local_5fc, tmp);
            }
        }
        else
        {
            // 普通层：帧 (m_c, m_10)，序列 m_ani0
            if (aniInfo->m_c == -1 && aniInfo->m_10 == -1)
            {
                continue;
            }
            char tmp[0x100] = {0};
            sprintf(tmp, "%02d%02d", aniInfo->m_c, aniInfo->m_10);
            if (local_69 != 0)
            {
                char t2[8] = {0};
                sprintf(t2, "*%d", i);
                strcat(tmp, t2);
            }
            char t3[8] = {0};
            sprintf(t3, "*%d", (int)aniInfo->m_ani0.size());
            strcat(tmp, t3);
            std::vector<STEquipmentAniScript>::const_iterator ait =
                     aniInfo->m_ani0.begin();
            while (ait != aniInfo->m_ani0.end())
            {
                char src[0x100] = {0};
                char wb[0x100] = {0};
                const STEquipmentAniScript* entry = &*ait;
                strcat(tmp, "*");
                strcpy(src, entry->m_str60.c_str());
                int w = 0;
                unsigned int r = 0;
                while (r < strlen(src))
                {
                    if (src[r] != '_')
                    {
                        wb[w] = src[r];
                        ++w;
                    }
                    ++r;
                }
                strcat(tmp, wb);
                ait++;
            }
            if (!local_69)
            {
                strcpy(local_2fc, tmp);
                local_69 = 1;
            }
            else
            {
                strcpy(local_4fc, tmp);
            }
        }
    }
    // ---- hide_growtype_avatar（growtype[0..4]）：script+0x780 数值段 ----
    char growtype[5] = {0};
    // ORIG（+0xc80）：growtype 源是 script+0x780 的 vector<int>（非 +0x198
    // extra_icon 列，后者只进 for_each 输出）。
    if (!script->m_vec780.empty())
    {
        int cnt = 0;
        std::vector<int>::const_iterator iit = script->m_vec780.begin();
        while (iit != script->m_vec780.end())
        {
            if (cnt > 4)
            {
            break;
            }
            int v = *iit;
            growtype[cnt] = v + '0';
            ++cnt;
            iit++;
        }
    }
    char local_3e90[0x2c00] = {0};
    int local_68 = 4;
    // local_168：0xb4 空则取 0x6a4
    std::string local_168;
    // ORIG（+0xdac）：string::length() != 0 判非空（setne 物化），先 b4 后 6a4。
    if (script->m_strb4.length() != 0)
    {
        local_168 = script->m_strb4;
    }
    else
    {
        local_168 = script->m_str6a4;
    }
    char local_9fc[1024] = {0};
    if (!CodePage::script2Database((char*)script->m_strac.c_str(), local_9fc))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x398, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), script->m_strac.c_str(),
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
                      // ORIG（+0xf09）：trace 实参取 script+0xb4 的 c_str()。
                      CodePage::database(), script->m_strb4.c_str(),
                      "bool sync_script::CSyncScript::insert_equiptment_item_to_db(STEquipmentScript*, unsigned int, int, int)",
                      0x39f);
        return false;
    }
    CStreamBuffer buf;
    buf << idx;
    buf << ", '" << GetEscapeString(db, local_9fc, local_3e90) << "'";
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_strb0.c_str(), &local_3e90[0x400])
        << "'";
    buf << ", '" << GetEscapeString(db, local_1290, local_3e90 + 0x800) << "'";
    buf << ", " << local_158;
    buf << ", " << local_154;
    buf << ", '" << this->GetUsableCharacter(script) << "'";
    buf << ", " << script->m_field4;
    buf << ", '" << this->GetAttachType(script->m_field8) << "'";
    buf << ", " << script->m_field5c;
    buf << ", " << script->m_184;
    buf << ", " << script->m_fieldc;
    buf << ", " << script->m_field10;
    buf << ", " << script->m_field34;
    buf << ", " << script->m_field14;
    buf << ", " << script->m_field18;
    buf << ", " << script->m_field1c;
    buf << ", " << script->m_1bc;
    buf << ", " << script->m_field68;
    buf << ", " << script->m_param1c0.m_hpMax;
    buf << ", " << script->m_param1c0.m_mpMax;
    buf << ", " << script->m_param1c0.m_physicalAttack;
    buf << ", " << script->m_param1c0.m_physicalDefense;
    buf << ", " << script->m_param1c0.m_magicalAttack;
    buf << ", " << script->m_param1c0.m_magicalDefense;
    buf << ", " << script->m_param1c0.m_phyAtt[0];
    buf << ", " << script->m_param1c0.m_phyDef[0];
    buf << ", " << script->m_param1c0.m_magAtt[0];
    buf << ", " << script->m_param1c0.m_magDef[0];
    buf << ", " << script->m_param1c0.m_fireResistance;
    buf << ", " << script->m_param1c0.m_waterResistance;
    buf << ", " << script->m_param1c0.m_darkResistance;
    buf << ", " << script->m_param1c0.m_lightResistance;
    buf << ", " << script->m_param1c0.m_allElementalResistance;
    buf << ", " << script->m_param1c0.m_slowResistance;
    buf << ", " << script->m_param1c0.m_freezeResistance;
    buf << ", " << script->m_param1c0.m_poisonResistance;
    buf << ", " << script->m_param1c0.m_stunResistance;
    buf << ", " << script->m_param1c0.m_curseResistance;
    buf << ", " << script->m_param1c0.m_blindResistance;
    buf << ", " << script->m_param1c0.m_lightningResistance;
    buf << ", " << script->m_param1c0.m_stoneResistance;
    buf << ", " << script->m_param1c0.m_sleepResistance;
    buf << ", " << script->m_param1c0.m_burnResistance;
    buf << ", " << script->m_param1c0.m_weaponBreakResistance;
    buf << ", " << script->m_param1c0.m_bleedingResistance;
    buf << ", " << script->m_param1c0.m_confuseResistance;
    buf << ", " << script->m_param1c0.m_holdResistance;
    buf << ", " << script->m_param1c0.m_allActiveStatusResistance;
    buf << ", " << script->m_param1c0.m_piercingResistance;
    buf << ", " << script->m_param1c0.m_stuckResistance;
    buf << ", " << script->m_param1c0.m_inventoryLimit;
    buf << ", " << script->m_param1c0.m_hpRegenSpeed;
    buf << ", " << script->m_param1c0.m_mpRegenSpeed;
    buf << ", " << script->m_param1c0.m_moveSpeed;
    buf << ", " << script->m_param1c0.m_attackSpeed;
    buf << ", " << script->m_param1c0.m_castSpeed;
    buf << ", " << script->m_param1c0.m_hitRecovery;
    buf << ", " << script->m_param1c0.m_jumpPower;
    buf << ", '" << this->GetAttackElement(&script->m_param1c0) << "'";
    buf << ", " << script->m_param1c0.m_activeStatusType;
    buf << ", " << script->m_param1c0.m_activeStatusValue;
    buf << ", " << script->m_param1c0.m_activeStatusLevel;
    buf << ", " << script->m_param1c0.m_pushAside;
    buf << ", " << script->m_param1c0.m_liftUp;
    // ORIG（+0x1b37/+0x1b66）：movzbl 后走 operator<<(int)（byte→int 提升），
    // 不是 uint 重载。
    buf << ", " << script->m_param1c0.m_hpDrain;
    buf << ", " << script->m_param1c0.m_mpDrain;
    buf << ", " << script->m_param1c0.m_physicalCriticalHit;
    buf << ", " << script->m_param1c0.m_stuck;
    buf << ", " << script->m_param1c0.m_ignoreDefense;
    buf << ", '"
        << std::for_each(script->m_param1c0.m_levelUpSkills[0].begin(),
                         script->m_param1c0.m_levelUpSkills[0].end(),
                         CStringMaker()).c_str()
        << "'";
    // ORIG（+0x1d83）：三目直接物化到寄存器（mov $ptr,%eax），不占命名槽。
    buf << ", '"
        << ((script->m_str668.compare("") == 0 && script->m_vec66c.size() == 0 &&
             script->m_6a0 == -1 && script->m_798 == -1)
                ? "n"
                : "y")
        << "'";
    buf << ", '" << socketChars << "'";
    register char* escDst1 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_param1c0.m_str27c.c_str(), escDst1)
        << "'";
    register char* escDst2 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_strb8.c_str(), escDst2)
        << "'";
    buf << ", " << script->m_param1c0.m_antiEvil;
    buf << ", " << script->m_field24;
    buf << ", " << script->m_184;
    buf << ", '" << script->m_pair28.first << "," << script->m_pair28.second << "'";
    buf << ", " << script->m_param1c0.m_physicalAbsoluteDamage;
    buf << ", " << script->m_param1c0.m_physicalDamageReduce;
    buf << ", " << script->m_param1c0.m_physicalAbsoluteDefense;
    buf << ", " << script->m_param1c0.m_magicalAbsoluteDamage;
    buf << ", " << script->m_param1c0.m_magicalDamageReduce;
    buf << ", " << script->m_param1c0.m_magicalAbsoluteDefense;
    buf << ", " << script->m_param1c0.m_fireAttack;
    buf << ", " << script->m_param1c0.m_waterAttack;
    buf << ", " << script->m_param1c0.m_darkAttack;
    buf << ", " << script->m_param1c0.m_lightAttack;
    buf << ", " << script->m_param1c0.m_allElementalAttack;
    buf << ", " << script->m_param1c0.m_weaponBreakResistance;
    buf << ", " << script->m_param1c0.m_armorBreakResistance;
    buf << ", " << script->m_param1c0.m_allActiveStatusResistance;
    buf << ", " << script->m_param1c0.m_rigidity;
    buf << ", '"
        << std::for_each(script->m_param1c0.m_aura188.begin(),
                         script->m_param1c0.m_aura188.end(),
                         CStringMaker()).c_str()
        << "'";
    buf << ", " << script->m_param1c0.m_magicalCriticalHit;
    register char* escDst3 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_str668.c_str(), escDst3)
        << "'";
    buf << ", '"
        << std::for_each(script->m_vec66c.begin(), script->m_vec66c.end(),
                         CStringMaker()).c_str()
        << "'";
    register char* escDst4 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_str6b0.c_str(), escDst4)
        << "'";
    register char* escDst5 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_str6b4.c_str(), escDst5)
        << "'";
    register char* escDst6 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_param1c0.m_str278.c_str(), escDst6)
        << "'";
    register char* escDst7 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_param1c0.m_str27c.c_str(), escDst7)
        << "'";
    buf << ", " << script->m_798;
    buf << ", '" << local_1fc << "'";
    buf << ", '"
        << std::for_each(script->m_param1c0.m_skill1b8.begin(),
                         script->m_param1c0.m_skill1b8.end(),
                         CStringMaker()).c_str()
        << "'";
    buf << ", " << local_74;
    buf << ", " << key;
    buf << ", " << script->m_6a0;
    buf << ", '" << local_2fc << "'";
    buf << ", '" << local_3fc << "'";
    buf << ", '" << local_4fc << "'";
    buf << ", '" << local_5fc << "'";
    buf << ", '" << growtype << "'";
    buf << ", " << script->m_param1c0.m_roomListMoveSpeedRate;
    // ORIG（+0x29c6）：dst = 公共转义缓冲 +0xc00 先物化进 callee-saved %edi
    // 跨 GetURL 调用存活 ⇒ register 指针。
    register char* urlDst = &local_3e90[0xc00];
    buf << ", '"
        << GetEscapeString(
               db, this->GetURL((char*)script->m_str6c.c_str(),
                                (int)script->m_str6c.length(),
                                script->m_field70),
               urlDst)
        << "'";
    buf << ", " << script->m_fieldc4;
    buf << ", '"
        << std::for_each(script->m_vec198.begin(),
                         script->m_vec198.end(),
                         CStringMaker()).c_str()
        << "'";
    buf << ", " << script->m_param1c0.m_hpMaxRate;
    buf << ", " << script->m_param1c0.m_mpMaxRate;
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
    if (!script->m_param1c0.m_avatar264.empty())
    {
        int abilityIdx = 0;
        for (std::vector< ::STAvatarAbilityVariation>::iterator avit =
                 script->m_param1c0.m_avatar264.begin();
             avit != script->m_param1c0.m_avatar264.end(); ++avit)
        {
            // ORIG（+0x2ca4..+0x2d77）：*it 指针入槽后逐字段重读（-0x1c reload），
            // 全部实参先行求值（skill+8→深槽、skill+4→edi、job 三目→edx、
            // f8 flds、f4→esi、f0→ebx、idx++→ecx），再压参调 set_query。
            ::STAvatarAbilityVariation* pAv = &*avit;
            db->set_query(
                "inSert into avatar_select_ability(it_no, ability_no, "
                "ability_type, rate_change, value, job, skill_index, "
                "skill_level) values(%d,%d,%d,%d,%f,%d,%d,%d)",
                script->m_field0, abilityIdx++, pAv->m_field0, pAv->m_field4,
                pAv->m_field8,
                (pAv->m_skill.m_job == 0xb) ? 0x63 : pAv->m_skill.m_job,
                pAv->m_skill.m_field4, pAv->m_skill.m_field8);
            if (!db->exec(true))
            {
                return false;
            }
        }
    }
    // ORIG（+0x2db5..+0x2f07）：flags 全部预初始化（4×movb+edi）+ 末端统一
    // dtor 链 ⇔ || 短路链内联临时（allocator ctor + string(char*,alloc)+D1），
    // 比较 SITEMTYPE.m_field4(+0x4 ushort) 与 local_154（mode0 GetType sub_type
    // 输出；其余 mode 为 0）。
    // ORIG（+0x2e25）：movzwl 0x4(ushort) → 32 位扩展后与 uint local_154 比较
    // （无 (unsigned short) 截断路径）。
    if (m_itemTypeMap[std::string("artifact red")].m_field4 == local_154 ||
        m_itemTypeMap[std::string("artifact blue")].m_field4 == local_154 ||
        m_itemTypeMap[std::string("artifact green")].m_field4 == local_154)
    {
        this->insert_artifact_info_to_db(script);
    }
    return true;
}

// [b28d-20] insert_stackable_item_to_db（ORIG 0x08620968，1437/1443，nd 738）：
//   §6.2 三件套 —— p167 差集仅 EH/string 临时族（-2 `_Unwind_Resume`、
//   +2 `~basic_string`、+1 `string::C1ERKSs`），无 MISSING 语义调用；gap=6；
//   残差全为 N104（EH 尾块相对序 rotate）+ N91（数组/临时 string cleanup）
//   族 ⇒ 语义完成、byte 层 N 封顶。
bool CSyncScript::insert_stackable_item_to_db(STStackableScript* script)
{
    // 依据 docs/class_func_reports/sync_script__CSyncScript/
    // insert_stackable_item_to_db.md（ORIG 0x8620968）：ORIG 汇编 + Ghidra C
    // 逐调用转写。字段走权威 STItemScript/STStackableScript 偏移命名，map 走
    // m_module4bc（ORIG operator[]@0x8628bc6，key 为局部 ENUM_MODULE_TYPE 0x13）；
    // 流输出用权威 ::CStreamBuffer，列表用 std::for_each(..., CStringMaker)。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    int out1 = 0;
    int out2 = 0;
    // ORIG（+0x43）：先按值拷贝 script+0xbc 构造临时 string 供 GetType。
    std::string typeName = script->m_strbc;
    if (!GetType(typeName, (ENUM_CHARACTERJOB)0, 0, out1, out2, false))
    {
        return false;
    }
    CStreamBuffer buf;
    char local_1874[1024] = {0};
    char local_1474[1024];
    char acStack_1074[1024];
    char local_c74[1024];
    char local_874[1024] = {0};
    char local_474[1024] = {0};
    if (!CodePage::script2Database((char*)script->m_strac.c_str(), local_474))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x523, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), script->m_strac.c_str(),
                      "bool sync_script::CSyncScript::insert_stackable_item_to_db(STStackableScript*)",
                      0x523);
        return false;
    }
    if (!CodePage::script2Database((char*)script->m_strb4.c_str(), local_874))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x52a, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), script->m_strb4.c_str(),
                      "bool sync_script::CSyncScript::insert_stackable_item_to_db(STStackableScript*)",
                      0x52a);
        return false;
    }
    buf << script->m_field0;
    buf << ", '" << GetEscapeString(db, local_474, local_1874) << "'";
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_strb0.c_str(), local_1474)
        << "'";
    buf << ", '" << GetEscapeString(db, local_874, acStack_1074) << "'";
    buf << ", " << out1;
    buf << ", " << out2;
    buf << ", '" << this->GetUsableCharacter(script) << "'";
    buf << ", " << script->m_field4;
    buf << ", '" << this->GetAttachType(script->m_field8) << "'";
    buf << ", " << script->m_field5c;
    buf << ", " << 0;
    buf << ", " << script->m_fieldc;
    buf << ", " << script->m_field10;
    buf << ", " << script->m_field34;
    buf << ", " << script->m_field14;
    buf << ", " << script->m_field18;
    buf << ", " << script->m_field1c;
    buf << ", " << 0;
    buf << ", " << script->m_field68;
    // ORIG（+0x61b）：局部 ENUM_MODULE_TYPE = 0x13 取引用查 m_module4bc。
    ENUM_MODULE_TYPE moduleKey = (ENUM_MODULE_TYPE)0x13;
    EquipParamInfo* ep = (EquipParamInfo*)script->m_module4bc[moduleKey];
    if (ep == 0)
    {
        // ORIG 字面量序列（Ghidra ep==0 分支逐条）：40×", 0"、", ''"
        // （att_element）、10×", 0"、3×", 0.0"、14×", 0"、", ''"
        // （skill_levelup）。
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
        buf << ", ''";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
        buf << ", 0.0";
        buf << ", 0.0";
        buf << ", 0.0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
        buf << ", ''";
    }
    else
    {
        EquipParamInfo* e2 = ep;
        buf << ", " << e2->m_hpMax;
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
        buf << ", '"
            << this->GetAttackElement((EquipmentParameterInfo*)e2) << "'";
        buf << ", " << e2->m_attActiveStatus;
        buf << ", " << e2->m_attActiveStatusRatio;
        buf << ", " << e2->m_attActiveStatusPow;
        buf << ", " << e2->m_attBackforce;
        buf << ", " << e2->m_attUpforce;
        // ORIG（+0xe08/+0xe37/+0xebc）：movzbl → operator<<(int)。
        buf << ", " << e2->m_attHpDrain;
        buf << ", " << e2->m_attMpDrain;
        buf << ", " << e2->m_criticalHitRate;
        buf << ", " << e2->m_stuckRate;
        buf << ", " << e2->m_attDefenseIgnore;
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
                             CStringMaker()).c_str()
            << "'";
    }
    // ORIG（+0x18ad）：两分支汇合后固定输出 set_type 空串（DAT_08ce36e4）。
    buf << ", ''";
    buf << ", '"
        << GetEscapeString(
               db,
               this->GetURL((char*)script->m_str6c.c_str(),
                            (int)script->m_str6c.length(), script->m_field70),
               local_c74)
        << "'";
    buf << ", " << script->m_fieldc4;
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
        CItem* item = (*it).second;   // ORIG 用 operator*（de）
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
    // [b28c-21] N93 封顶：ORIG it(默认构造,-0x18)/e(-0x10)/find 临时(-0x20)；
    // 4.4 槽序与求值序绑定（交换声明/操作数均致槽或比较参反向），DIFF→NEAR。
    __gnu_cxx::hash_map<int, CCeraShopGoods*>::iterator it;
    __gnu_cxx::hash_map<int, CCeraShopGoods*>::iterator e = m_goodsMap.end();
    // [b26N7b] it/e 槽分配方向 4.1(深→浅) vs 4.4(浅→深) 相反；换声明序虽对齐
    // 槽号但使 C1Ev/end call 序倒转 AE 判 DIFF 倒退，故保持 ORIG 声明序封 NEAR。
    // [b28d-20] fresh 双形实测（ORIG 0x8512358，it=-0x18/e=-0x10）：
    //   原形 it 先声明 ⇒ 4.4 it=-0x10/e=-0x18（槽反，nd=9，verdict NEAR）；
    //   交换形 e 先声明 ⇒ 槽全对齐但 `call C1Ev(it)` 与 `call end()` 顺序倒转
    //   （nd=6，verdict 退为 DIFF）。4.4 槽序=声明序且构造序=声明序，ORIG
    //   「it 深槽 + C1Ev 先执行」在 4.4 下不可兼得 ⇒ N93 封顶，保留 NEAR 形。
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
    // [b28c-21] N93 封顶：ORIG 双 const_iterator 槽 -0x1c/-0x14（外层先/内层后），
    // 4.4 恒反向（-0x14/-0x1c）；嵌套 for 声明序不可控。
    // [b28d-20] fresh 复验：迭代器槽已对齐（旧 N93 判词不再适用）；真实残差 = 唯一
    //   `sum = m_field8 + sum` 段（ORIG 0x8512xx 4 条：`mov 0x8(%eax),%edx;
    //   mov -0xc(%ebp),%eax; lea (%edx,%eax,1),%eax; mov %eax,-0xc(%ebp)`）被 4.4
    //   折叠为 2 条 `mov 0x8(%eax),%eax; add %eax,-0xc(%ebp)`（同型 int+int 恒折，
    //   P211 同型律）。异型化试形：sum 改 unsigned int + 比较处 (int)sum ⇒ 仍折叠
    //   （实测无变化）；volatile 致整帧槽漂移 111 vs 108 反劣。⇒ N86b/N18a 封顶。
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

// [b28d-20] insert_item（ORIG 0x0851129e，309/309，nd 43）：本轮修 P5 裸虚表
//   → `item->IsRandomOption()`（CItem vtable +0x4c 第 19 槽）与 P167
//   `operator==` 形；p167 调用点差集 45/45 **CLEAR**（0 missing/0 extra）。
//   残差 = 尾部 map 插入段的槽号/寄存器分配（BaseItemKey 临时 + inner map）⇒
//   语义完成、byte 层 N7/N82 封顶。
bool CItemList::insert_item(int group, int idx, CItem* item)
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it;
    it = m_gradeItemList[group].find(idx);
    // [b28d-20] P167：ORIG 调用 `_ZN..._Hashtable_iterator...eqERKSD_`（operator==），
    //   我方原 `!=` 落 EXTRA `ne`。改 `!(==)` 形复现 ORIG 调用点。
    if (!(it == m_gradeItemList[group].end()))
    {
        m_gradeItemList[group].insert(std::pair<const int, CItem*>(idx, item));
        m_itemMap.insert(std::pair<const int, CItem*>(idx, item));
        if (item->GetQuestItemDropBouns())
        {
            m_lottoCount.insert(
                std::make_pair(item->GetQuestItemDropBouns(), (int)idx));
        }
        // [b28d-20] P5 直调：ORIG 0x081f???? `mov (%eax),%eax; add $0x4c,%eax;
        //   mov (%eax),%edx; … call *%edx` = CItem vtable +0x4c = IsRandomOption
        //   （CItem.h 虚序第 19 槽）；裸函数指针强转已删。
        if (item->IsRandomOption())
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

// [b28d-20] load_item_files（ORIG 0x08511738，449/458，nd 101）：本轮修
//   P28 增量侧（两处 `++it`→`it++`，复现 ORIG `_Rb_tree_iterator::ppEi`×2）
//   + P167 共享尾（DisplayMaxLength 调用点 2→1）；p167 差集 **CLEAR**。
//   残差 = 两循环的 string 局部槽序与帧 0xe3c 内偏移（N82/N93 族）⇒ 语义完成、
//   byte 层 N 封顶。
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
        // [b28d-20] P28 增量侧：p167 差集 ORIG 调 `_Rb_tree_iterator::ppEi`
        //   （后置，带 sret 临时槽）×2，我方 `++it` 落 EXTRA `ppEv` ⇒ 源写 it++。
        it++;
    }

    it = sync_script::g_stStackableScriptFileList.begin();
    while (it != sync_script::g_stStackableScriptFileList.end())
    {
        path = it->second.c_str();
        if (!path)
        {
            // [b28d-20] P167：ORIG 全函数仅 1 个 DisplayMaxLength 调用点
            //   （我方原写 2 处 ⇒ EXTRA +1）；改 break 落共享尾部。
            break;
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
        it++;   // [b28d-20] P28 同上一处（ORIG ppEi）
    }
    sync_script::DisplayMaxLength();
    return true;
}

// [b28d-20] Load_Goods_File（ORIG 0x085123da，386/384，nd 103）：p167 唯一
//   差集 = ORIG 调 `vector<stAvatarTypeSelect_t>::operator[](unsigned) const`
//   （我方非 const 版）⇒ 消费点 vector 需 const 上下文（const 引用/指针）；
//   其余残差 = 帧 0x13c/0x14c + 局部缓冲 -0x114/-0x120 偏移（N82/P232 族）。
//   语义等价（逐字段写与 ORIG 对应）⇒ 封顶。
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
                    (int&)shopList[i][j].m_field14));
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
    // ORIG 形态：make_pair 直接收字符串字面量 + ST_ITEM_KIND 枚举（pair 转换构造
    // 生成 _ZSt9make_pairIRA19_KcN17Arad_GiftItem_Set12ST_ITEM_KINDEE… 与 ORIG 一致）。
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM", ST_ITEM_KIND_1));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM2", ST_ITEM_KIND_2));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM3", ST_ITEM_KIND_3));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM4", ST_ITEM_KIND_4));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM5", ST_ITEM_KIND_5));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM6", ST_ITEM_KIND_6));
}

// ORIG T 0x081a2dea：先写 vptr（基类 Arad_ScriptLoader vtable），再逆序析构
// +0x1c map<string,int> 与 +0x04 map<long long,Data>（无虚析构，按值析构）。
Arad_GiftItem_Set::~Arad_GiftItem_Set()
{
}

// [b28d-20] Arad_GiftItem_Set::open（ORIG 0x081a2f1e，153/161，nd 34→32）：
//   本轮真语义修正 base 符号性（`unsigned int base`→`int base`，ORIG
//   0x081a2f?? 为 `mov %eax,%edx; sar $0x1f,%edx` 符号扩展，零扩展会改负值
//   时的 64 位 key）。残差 = Data/pair 插入路径多一次 Data 拷贝构造
//   （P81 引用 pair 与 4.4 转换构造差异）+ 槽 -0x3c/-0x54 漂移 ⇒ N7/P81 封顶。
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
    // [b28d-20] 原 unsigned 化致 4.4 零扩展（`mov $0x0,%edx`），ORIG 0x081a2f?? 为
    //   `mov %eax,%edx; sar $0x1f,%edx`（有符号 int 扩展）⇒ base 保持 int。
    const int base = script->m_field5c;                       // *(param_1 + 0x5c)

    // ORIG：for (local_10 = 0; local_10 != 0xb; ++local_10)，读取 +0x38 起 11 字节
    for (int i = 0; i != 0xb; ++i)
    {
        // STItemScript::m_usable@+0x38（继承成员具名，禁裸偏移）
        if (script->m_usable[i] != 0)
        {
            // ORIG +0x81-0xb5：L2R 64bit 进位链 kind<<32 → +i<<16(movzwl) → +base
            long long key =
                ((long long)kind << 32) +
                (long long)((unsigned short)i << 16) +
                (long long)base;
            Data data;
            data.m_item = itemId;
            data.m_str4 = script->m_arad178.m_strc;   // +0x184
            data.m_str8 = script->m_arad178.m_str10;  // +0x188
            // P81：ORIG pair<long long&, Data&> 引用对构造（非 make_pair 值对）
            if (!m_map.insert(
                    std::pair<long long&, Data&>(key, data)).second)
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
    // [b28c-L18] P140：ORIG 0x081a2e3e 入口直构 string(name)，无 null 防御（P171
    // 照抄原则）。ORIG 尾段实证调用符号为 iterator::operator!=、this=end 临时
    // ((esp)=-0xc)、实参 it(0x4(esp))，true→hit fall-through ⇒ 原码
    // `if (end() != it) return it->second; return 0;`（P222 正向包裹）。
    std::map<std::string, int>::iterator it = m_kindMap.find(std::string(name));
    if (m_kindMap.end() != it)
        return it->second;
    return 0;
}

// ============================================================================
// Arad_GiftItem_Set::find（ORIG 0x081a311e，_ZN17Arad_GiftItem_Set4findEiii）
// 语义：按 64 位 key = ((long long)kind << 32) | (((long long)job << 16) + usableLevel)
// 查 this+0x04 的 map<long long, Data>，命中返回 &it->second，否则 0。
// ============================================================================
Arad_GiftItem_Set::Data* Arad_GiftItem_Set::find(int kind, int job, int usableLevel)
{
    // ORIG：64bit 进位链 + 形（与 open 同构，| 会改 codegen）
    long long key =
        ((long long)kind << 32) +
        (long long)((unsigned short)job << 16) +
        (long long)usableLevel;
    std::map<long long, Data>::iterator it = m_map.find(key);
    // [b28c-L18] P222 正向包裹（两形互试）：ORIG 块序 = hit 紧跟条件 fall-through、
    // `mov $0` 尾岛直落 epilogue；负极性 `if(it==end)return 0;` 在 4.4 恒产反序岛。
    if (it != m_map.end())
        return &it->second;
    return 0;
}

// ============================================================================
// Arad_GiftItem_Set::level_check（ORIG 0x081a31c4，_ZN17Arad_GiftItem_Set11level_checkEP5CUserPK14CStackableItem）
// 语义：
//   kind  = str_to_enum(item->m_arad148.m_str8.c_str())
//   job   = user->get_charac_job()
//   ulv   = item->getUsableLevel()
//   data  = find(this, kind, job, ulv)
//   data==0 -> 1；job 不可用（find 未命中）则报 "user level limit" 日志并返回 0；
//   user->get_charac_level() < ulv -> 报日志并返回 0；否则 1。
// NOTE：汇编中 get_charac_level 比较对象是 CUserCharacInfo 基类，故对 user 强转访问。
// ============================================================================
bool Arad_GiftItem_Set::level_check(CUser* user, const CStackableItem* item)
{
    // ORIG 0x081a31c4：无空指针防御（P140）；usableLevel 值需跨
    // get_charac_job call 存活 ⇒ 内联实参直落 %ebx（P52）；
    // characLevel 在 < 比较与 log 实参各独立重调一次（P48）。
    int kind = str_to_enum(item->m_arad148.m_str8.c_str());
    Data* idx = find(kind, user->get_charac_job(), item->getUsableLevel());
    if (idx == 0)
    {
        return 1;
    }
    if (user->get_charac_level() < item->getUsableLevel())
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x14b, "[ERROR] user level limit charac_no(%d)",
                              user->get_charac_level());
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
// [b28c-lane17] fresh：101=101，残差 = new[] 返回值的异常槽 `lea -0x1c(%ebp)`
// 物化时刻（ORIG 4.1 提前入 %esi 跨 call 保活；4.4 在使用点重算入 %edx）
// + 清理块 %esi/%edx 互换 ⇒ N18a/N7（跨版本 callee-saved 分配），无源形。
// [b28d-20] fresh 复验 101=101，残差 3 处全在 new 表达式 ctor 实参 `&seed`：
//   ORIG 0x832c48d `lea -0x1c(%ebp),%esi` 位于 0x832c497 operator new **之前**
//   （4.1 先求 ctor 实参地址并入 callee-saved 跨 call 保活）；4.4 在 0x832c4a7
//   CMTRand::C1 前重算入 %edx。P226 两形实测均不闭合：
//   ① `new CMTRand(({ seed; }))` ⇒ 多临时槽 -0x20（103 vs 101）；
//   ② `new CMTRand(({ (const unsigned long&)seed; }))` ⇒ 仍右值化临时 -0x20
//   （GCC 语句表达式在 C++ 下强制 lvalue→rvalue）。⇒ 判 N18a/N7 封顶。
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

// [b28d-20] after_addItem（ORIG 0x832de18，101 insn / 我方 102，nd 26）：
//   i 改 unsigned 后尾测 `setbe` 与 ORIG 0x832ded9 对齐（原 int 生 setle）；
//   残差 = 循环体首 7 条寄存器角色互换（ORIG edx=i/ecx=j，4.4 ecx=i/eax=j）
//   + P209 附：4.4 对 unsigned 局部尾测恒折 `cmpl $0x3a,mem` 单段（ORIG 为
//   mov+cmp 两段）⇒ N7/N18a 封顶。索引算术已核（48i+12j 与 ORIG 同）。
void CCompound_ExtreamDun_Item::after_addItem()                     // ORIG 0x832de18
{
    for (unsigned int i = 1; i < (unsigned int)GROUP_NUM_MAX; ++i)
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

// [b28d-20] _AddItem（ORIG 0x832dd1c，78/79，nd 35）：ORIG entry@-0x1c、
//   entry2@-0x14（entry 先构造占深槽）；4.4 先声明占浅槽（entry@-0x14），
//   交换声明序会倒转两对象的构造/实参求值序（N93 槽方向律）⇒ 封顶。
//   语义与 ORIG 一致（P234 static_cast<T&&> 已用，无 std::move 外链）。
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
    // ORIG 槽：entry@-0x1c、entry2@-0x14（连排声明复现槽序）。
    OUTPUT_ITEM_STR entry(item.getUsableLevel(), item.get_index());
    OUTPUT_ITEM_STR entry2(item.getUsableLevel(), item.get_index());
    m_data.m_resultItems[cat].push_back(static_cast<OUTPUT_ITEM_STR&&>(entry));
    m_data.m_bindItems[item.getItemGroupName()][cat].push_back(
        static_cast<OUTPUT_ITEM_STR&&>(entry2));
    return true;
}

// [b28d-20] _isPossibleResultItem（ORIG 0x832db4e，140/135，nd 27→14）：
//   本轮修 P5 裸虚表→`item.IsRandomOption()`、P50 先声明后赋值（复现 ORIG
//   0x832db8d 独立 `iterator::C1Ev`）、正极性返回 `if (IsRandomOption())
//   return false; return true;`（ORIG 0x832dd09 `test; je <mov $1>`）。
//   残差 = it/endIt 槽互换（ORIG it=-0x14/endIt=-0x10，4.4 先声明占浅槽）
//   与帧 0x38/0x28 ⇒ N93 封顶（交换声明序会倒转 C1Ev/end 调用序，同
//   FindGoods 实证）。
bool CCompound_ExtreamDun_Item::_isPossibleResultItem(              // ORIG 0x832db4e
    CEquipItem& item) const
{
    if ((unsigned int)item.getItemGroupName() > 0x3a ||
        item.getItemGroupName() <= 0)
    {
        return false;
    }

    // 强制规则命中即一票裁决：flag==0（强制允许）→ true，非零 → false。
    // [b28d-20] P50 复活：ORIG 0x832db8d 有独立 `iterator::C1Ev`（lea -0x14;
    //   call C1Ev）先于 end()，⇒ 原码 it 先声明后赋值；it@-0x14/endIt@-0x1c。
    std::map<int, forceRule_ResultItem>::const_iterator it;
    std::map<int, forceRule_ResultItem>::const_iterator endIt =
        m_data.m_forceRules.end();
    it = m_data.m_forceRules.find(item.get_index());
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
    // [b28d-20] P5 直调：ORIG 0x832dcf7 `mov (%eax),%eax; add $0x4c,%eax;
    //   mov (%eax),%edx; … call *%edx` = 虚成员直调，非裸函数指针缓存；
    //   返回按 ORIG 0x832dd09 分支形态（`test %al,%al; je <mov $1>`）写 if/return。
    if (item.IsRandomOption())
    {
        return false;
    }
    return true;
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
    // ORIG 0x832c5ac：switch 降形态（pivot=2 树，case9 独立体在右叶之后）。
    switch ((int)eqType)
    {
    case 9:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    default:
        return 4;
    }
}
int CCompound_ExtreamDun_Item::_convertInputItem(                   // ORIG 0x832c620
    ENUM_UNUSABLE_EQUIPMENT_TYPE eqType) const
{
    // ORIG 0x832c620：switch 降形态；case1/9 共享 body（fall-through 空 case）。
    switch ((int)eqType)
    {
    case 1:
    case 9:
        return 0;
    case 2:
        return 1;
    case 3:
        return 2;
    default:
        return 3;
    }
}

// b21 NEAR：语义与 ORIG 全等（两真修已落，CItemList 编译绿）。残余仅 N-86b：
//   冗余 setne/test、虚拟调用寄存器选择（call* %eax vs call* %edx）、shl 寄存器分配、
//   IsTradeRestriction 后冗余 setne（结果本就是 0/1，行为全同）+ 全量 AE/分支偏移。
// [b28c-lane17] fresh 复验：258=258 助记符全等 ⇒ NEAR（残差同上 N86b/N7 族），
// 调用点差集无缺项，判「语义完成、byte 层 N 封顶」。
// [b28d-20] fresh 复验：4.4.6 路由下 256=256 **IDENTICAL**（归一化零差异；
//   旧 N7 `call *%eax/%edx` 差在 446h46 后端下消失，P230 分诊法命中）。
//   TSV 基线 NEAR/1 系 4.4.7 口径，已由本轮 routed 复验推翻。
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
        // ORIG +0x115/+0x123：两次独立 getItemGroupName 调用（P48 无 CSE）。
        // ORIG +0x173 `jle`（有符号>0）与 +0x18b `jbe`（无符号>0x3a）混合 ⇒
        // 第二个比较显式转 unsigned（Ghidra `0x3a < uVar5`）。
        if (refs[i]->getItemGroupName() > 0 &&
            (unsigned int)refs[i]->getItemGroupName() > 0x3a)
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
        // ORIG else 块返回 0x11 条件 = BOTH > 0x45（asm `cmp; jle 定 false` 两级
        // 与 Ghidra `!(L0<0x46) && !(L1<0x46)` 一致）；kind==3 块才是 OR
        // （`jg` 短路 true / `!(L0<0x3d)||!(L1<0x3d)`）。原 `||` 语义差异，改 &&。
        if (refs[0]->getUsableLevel() > 0x45 && refs[1]->getUsableLevel() > 0x45)
        {
            return 0x11;
        }
    }
    return 0;
}

// [b28d-20] makeCompoundItem（ORIG 0x832ccc0，249/244，nd 192→130）：本轮修
//   P192/p167 三处 `std::make_pair`→`std::pair<unsigned int,unsigned int>` 直构
//   （ORIG `C1IjjEEOT_OT0_`）；p167 差集 18/18 **CLEAR**。残差 = 随机数取模
//   除法链与 bonus/found 局部槽号（N82/N7 族）⇒ 语义完成、byte 层封顶。
bool CCompound_ExtreamDun_Item::makeCompoundItem(                   // ORIG T 0x832ccc0
    const CEquipItem** refs, char select, Inven_Item& out, eBindSphereType bind)
{
    out.reset();
    int baseLevel = _get_base_level(refs);
    int inRarity = _convertInputItem(
        (ENUM_UNUSABLE_EQUIPMENT_TYPE)refs[0]->GetUsableEquipmentType());

    // 双次 randInt()%1000000 + 1（magic 常量 0x431bde83 实证；ORIG 为无符号除法）
    unsigned int r0 = ((unsigned int)m_rand->randInt() % 1000000) + 1;
    unsigned int r1 = ((unsigned int)m_rand->randInt() % 1000000) + 1;
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
    if (!(bonus.second == 0xa && inRarity != 1 && ((unsigned int)baseLevel % 10) > 4))
    {
        // [b28d-20] P192/p167：ORIG 0x832ccc0 段三处均为 `std::pair<unsigned
        //   int,unsigned int>` 直构（`_ZNSt4pairIjjEC1IjjEEOT_OT0_`），
        //   make_pair 版本落 EXTRA（3 处）。
        found = _FindItemAll_BindSphere(refs, select,
            (ENUM_OUTPUTITEMS_RARITY)outputRarity,
            std::pair<unsigned int, unsigned int>(
                bonus.first + (unsigned int)baseLevel,
                bonus.second + (unsigned int)baseLevel));
    }
    if (found == 0)
    {
        if (inRarity >= 1 && inRarity <= 2)
        {
            found = _FindItemAll_BindSphere(refs, select,
                (ENUM_OUTPUTITEMS_RARITY)outputRarity,
                std::pair<unsigned int, unsigned int>(
                    (unsigned int)(baseLevel + 5),
                    (unsigned int)(baseLevel + 1)));
            if (found == 0)
            {
                found = _FindItemAll_BindSphere(refs, select,
                    (ENUM_OUTPUTITEMS_RARITY)outputRarity,
                    std::pair<unsigned int, unsigned int>(
                        (unsigned int)(baseLevel + 0xa),
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
    // ORIG 0x832c5f0：switch 降形态；case0 体前置（test 判叶、jne 直落 default）。
    switch ((int)inRarity)
    {
    case 0: return 0;
    case 1: return 2;
    case 2: return 3;
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
    // ORIG 0x832c9b7：i 为函数作用域，耗尽路径 `mov -0x4(%ebp),%eax` 返回 i(=4)。
    int i;
    for (i = 0; i <= 3; ++i)
    {
        if ((unsigned int)m_data.m_rarityDicision[(int)inRarity][i] >= randVal)
        {
            break;
        }
    }
    return i;
}
// [b28d-20] _determine_level_bonus(Eii)（ORIG 0x832c9ca，91/79，nd 44）：
//   残差 = 帧 0x20(ORIG)/0x30(我方) + 循环变量 c 的槽位/减量段形态
//   （ORIG `mov -0xc; cmp $0; je` 两段 vs 4.4 折 `cmpl $0,-0x10` 单段）⇒ N82。
std::pair<unsigned int, unsigned int>                              // ORIG T 0x832c9ca
CCompound_ExtreamDun_Item::_determine_level_bonus(
    unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity,
    unsigned int& idxOut, eBindSphereType bind) const
{
    idxOut = _determine_level_bonus_index(baseLevel, inRarity, bind);
    // ORIG +0xb8c+bind*0x30+0xc：**m_rates[0]**（不随 inRarity；rarity 只入
    // _index）。返回 pair(下界=v[idx-1]+1, 上界=v[idx])（+0xca61-0xcbf1 装配实证，
    // 0x4(%esp)=first 临时、0x8=second 引用）。
    const std::vector<unsigned int>& rates =
        m_lvBonus[(int)bind].m_rates[0];
    if (idxOut == 0 || rates.size() <= idxOut)
    {
        int lo = 0;
        int hi = 0;
        return std::pair<unsigned int, unsigned int>(lo, hi);  // pair<int,int> 形
    }
    return std::pair<unsigned int, unsigned int>(rates[idxOut - 1] + 1,
                                                 rates[idxOut]);
}

// [b28d-20] _determine_level_bonus（ORIG 0x832cadc，95/83，nd 71）：同 Eii 版
//   残差族（帧 0x20/0x30 + 循环变量槽段），语义（RARITY 守卫 + m_lvBonus
//   索引查表）逐条对应 ⇒ N82 封顶。
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
    // 同上：m_rates[0]，pair(下界, 上界)（ORIG 0x832cadc 段实证）
    const std::vector<unsigned int>& rates =
        m_lvBonus[(int)bind].m_rates[0];
    if (idx == 0 || rates.size() <= idx)
    {
        int lo = 0;
        int hi = 0;
        return std::pair<unsigned int, unsigned int>(lo, hi);
    }
    return std::pair<unsigned int, unsigned int>(rates[idx - 1] + 1, rates[idx]);
}

// [b28d-20] _determine_level_bonus_index（ORIG 0x832cc02，64/63，nd 68）：
//   残差 = 帧 0x24(ORIG)/0x28(我方)+push ebx 有无 + idx 缓存寄存器（ORIG
//   `mov -0xc(%ebp),%ebx`）与 m_lvBonus 索引乘法展开（4.1 imul 链 vs 4.4
//   lea 链）；语义等价（RARITY 守卫与 while 短路链逐条对应）⇒ N7/N82 封顶。
unsigned int CCompound_ExtreamDun_Item::_determine_level_bonus_index(  // ORIG T 0x832cc02
    unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity,
    eBindSphereType bind) const
{
    if ((int)inRarity < 0 || (int)inRarity > 2)
    {
        return 0;
    }
    // ORIG while 形：size()!=0 && v[idx]<baseLevel 短路链，两调用每次
    // 全地址重算（P48 无引用缓存）
    unsigned int idx = 0;
    while (m_lvBonus[(int)bind].m_rates[(int)inRarity].size() != 0 &&
           m_lvBonus[(int)bind].m_rates[(int)inRarity][idx] < baseLevel)
    {
        ++idx;
    }
    return idx;
}

int CCompound_ExtreamDun_Item::_exception_Rare(                     // ORIG T 0x832cffe
    const CEquipItem** refs, char select, ENUM_OUTPUTITEMS_RARITY outRarity,
    unsigned int baseLevel) const
{
    // [b28d-20] 真语义修正：ORIG 0x832d01e `mov 0x14(%ebp),%eax; mov %eax,-0xc(%ebp)`
    //   —— 内层 c 初值取自第 3 参 outRarity（0x14），非 baseLevel（0x18）；
    //   baseLevel 仅参与 pair 计算（0x832d03d/0x832d050 `add 0x18(%ebp),%eax`）。
    //   原实现以 baseLevel 为初值使 _FindItemAll_BindSphere 重复调用 baseLevel+1 次
    //   （该 callee 带随机语义），行为与 ORIG 不符。
    int found = 0;
    for (unsigned int g = 0; g <= 1u; ++g)
    {
        for (int c = (int)outRarity; c >= 0; --c)
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
    if (a.m_usableLevel == b.m_usableLevel)
    {
        return a.m_index < b.m_index;
    }
    return a.m_usableLevel < b.m_usableLevel;
}

// [b28d-20] _FindSelectItem（ORIG 0x832d15e，109/115，nd 85）：残差 = 帧
//   0x48(ORIG，无 push ebx)/0x54(我方，push ebx) + lower_bound/upper_bound 的
//   迭代器临时槽与 this+0x30 绝对偏移寻址寄存器分配；守卫与谓词调用序逐条
//   对应（group==0/>0x3a 双查 + outRarity 有符号窗口）⇒ N7/N82 封顶。
int CCompound_ExtreamDun_Item::_FindSelectItem(                     // ORIG T 0x832d15e
    unsigned int group, ENUM_OUTPUTITEMS_RARITY outRarity,
    const std::pair<unsigned int, unsigned int>& range) const
{
    // ORIG 0x832d164：group 为无符号双查（==0 je / >0x3a jbe），outRarity 独立
    // 有符号窗口（<0 js / >3 jle）——原合并写法被折成单 jle，且缺 outRarity 守卫。
    if ((group == 0) || (group > 0x3a))
    {
        return 0;
    }
    if ((int)outRarity < 0 || (int)outRarity > 3)
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

// [b28d-20] _FindRandomItem（ORIG 0x832d2ce，104/100，nd 48）：残差 =
//   m_resultItems 基址折叠形态（ORIG `add 0x8(%ebp); add $0x4` 两步 vs 4.4
//   lea 合并）+ pair 构造临时槽方向 + 尾块 `mov $0/-0x38` 岛；调用点与谓词
//   一致 ⇒ N86/N93 封顶。
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
    // ORIG 0x832d438：比较对象是 char select（spill -0x1c，cmpb），
    // 结果统一落 slot -0xc（int result = 0 初存 + 两支 call 后写回）。
    int result = 0;
    if (select == 2)
    {
        result = _FindRandomItem(outRarity, range);
    }
    else if (select == 0 || select == 1)
    {
        result = _FindSelectItem(refs[select]->getItemGroupName(), outRarity, range);
    }
    return result;
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
// [b28d-20] LoadScript（ORIG 0x0832c65e，222/218，nd 47）：残差 = 缺 ORIG
//   0x0832c6xx 的 `lea -0x4c; call C1Ev`（string 具名局部先声明后赋值形）与
//   其后的 bool 物化段（ORIG `movb $0; … call; xor $1; test; jne`），
//   4.4 把该 string 直构在 -0x50 且短路链折成正极性直跳 ⇒ P225/P28 族封顶。
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
    if (it != m_lottoCount.end())
    {
        return it->second;
    }
    return 0;
}
