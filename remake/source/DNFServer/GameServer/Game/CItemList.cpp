#include "LogManager.h"
#include "GameRand.h"
// df_game_r CItemList 还原（2026-08-16 / 2026-08-17 第九批 I1 收尾）。
// 成员布局以 ORIG 构造/析构为基准；函数形态以 class_func_reports/CItemList.md
// 与 class_func_reports/sync_script__CSyncScript.md 逐条对照
// （AE 口径：调用目标/全局地址归一，指令形态逐条一致）。
//
// [b28d-solo-CItemList 泳道判词] 本轮 3 件转 AE（p167 全 CLEAR）：
//   · CCompound_ExtreamDun_Item::C1 — ORIG 0x0832c48d `lea -0x1c(%ebp),%esi`
//     在 operator new(0x832c497) 之前 ⇒ P307 取址实参 register 指针形。
//   · _FindRandomItem — ORIG 0x0832d30c/-0x30、0x0832d38a/-0x20 关键槽 +
//     0x0832d3ff `operator!=(end,lo)` 实参序 ⇒ 匿名临时 key + 交换操作数。
//   · Arad_GiftItem_Set::open — ORIG 0x081a302f converting ctor 直构 +
//     0x081a3059 析构 pair 后才读 .second ⇒ `pair<const long long,Data>` 直构
//     + 具名 insert 结果局部。
//   其余件按 T1–T5/S1 停止规则收敛（详见各函数头判词），near/diff 不上升。
// [2026-09-10 SweepCEquipItem2 复核] dual_rank 候选 4 件逐件 fresh 核对（四数
//   696/1365/7/14/480 零回退）：
//   · insert_equiptment_item_to_db gap 11（orig 2898 / our 2888）：§6.2 三件套
//     —— p167 488/487（唯一 MISSING 为**自调用误报**：ORIG 0x0861da66 `call
//     861d87e` 1 次、我方 `R_386_PC32` 同符号 1 次，双向各 1 已逐位核实；其余
//     全为 C1/C2 同址别名）；立即数多重集 MISSING 空（仅 EXTRA 0x400 = `lea
//     0x400(%eax)` vs `add $0x400` 寻址拆分，N4/P30）；差异为 N18a 单段
//     regmove、N7 寄存器角色与 N82 槽号 ⇒ 判语义完成。
//   · insert_stackable_item_to_db gap 7（orig 1437 / our 1430）：p167 309/309；
//     立即数差 `$0x1`×2/`$0x0`×1 已定位为 ORIG 的 4.1 bool 归一化块
//     （`xor $0x1,%eax; mov $0x0,%ebx; mov $0x1,%ebx`，4.4 不生成）⇒ N 封顶。
//   · insert_item gap 1：三形块序实测记录见函数头（形① 最优，N104 封顶）；
//     虚槽 +0x4c = CItem::IsRandomOption 与 ORIG 0x08511xxx 一致。
//   · _determine_level_bonus_index gap 1：register/语句表达式两形实测反劣
//     （见函数头判词），N7/N82 封顶。

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
// ORIG T 0x0861ca1b：socket 类型 → 字符（1→'1' … 0xffff→'7'，其余 0）。
// [b28d-Lane16] TU 归属修正：ORIG 中它与 static_init @0x0862690c 同 TU
//   （`orig_tu_names.tsv:107` 键名 `_GLOBAL__I__Z22GetSocketTypeCharactert`
//   = SyncScript.cpp），定义已迁至 CFieldLengthChecker.cpp；本 TU 仅引用。
int GetSocketTypeCharacter(unsigned short type);


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


 
// [b28d-Lane16] TU 归属修正（P186/P108 键名 + 地址邻接双向证据）：
//   本 TU 的 ORIG static_init @0x0851398b 只含 ioinit（26 条）⇒
//   CFieldLengthChecker::fieldNames_/fieldLength_ 的定义**不属本 TU**；
//   其真正的 ORIG 归属是 static_init @0x0862690c 所在的 TU
//   （含 `CFieldLengthChecker::AddField`@0x086272e8、`GetSocketTypeCharacter`
//   @0x0861ca1b，GI 键名 `_GLOBAL__I__Z22GetSocketTypeCharactert`）。
//   定义已迁至 CFieldLengthChecker.cpp；本 TU 仅经权威头引用（U 引用）。

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

CSyncScript* GetInstanceSyncScript()                  // ORIG 0x861cbb4
{
    static CSyncScript instance;
    return &instance;
}

void DisplayMaxLength()                               // ORIG 0x861ca94
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    for (unsigned int i = 0; i < CFieldLengthChecker::fieldNames_.size(); ++i)
    {
        if (CFieldLengthChecker::fieldLength_[i] == 0)
        {
            continue;
        }
        db->set_query("seLect max(length(%s)) from dnf_item_info",
                      CFieldLengthChecker::fieldNames_[i].c_str());
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
        if (CFieldLengthChecker::fieldLength_[i] != len) { }
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
// [b28d-solo-CItemList] DIFF（ndiff 95→89）| p167 CLEAR（488/488 calls）。
//   本轮 T3 直证修 ani 循环守卫：ORIG `movzbl 0x38(%edx,%eax,1),%eax;
//   test %al,%al; je`（无 xor/setcc）⇒ m_usable 为 bool 且源码正极性包裹
//   `if (m_usable[i] != 0) { body }`（`==0 continue` 被 4.4 折成
//   `xor $1; test; jne`，N122）。残 = 数组基址/两段中转寄存器角色
//   （ORIG `mov mem,%eax; mov %eax,%ebx` vs 4.4 单段）⇒ N18a/N86b/N107。
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
        // [b28d-solo] T3 直证（ORIG 0x0862xxxx `movzbl 0x38(%edx,%eax,1),%eax;
        //   test %al,%al; je`，无 xor/setcc）⇒ m_usable 为 bool 且源码是
        //   正极性包裹 `if (m_usable[i] != 0) { body }`（`== 0 continue` 在 4.4
        //   产 `xor $1; test; jne`，N122）。
        if (script->m_usable[i] != 0)
        {
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
    unsigned int local_68 = 4;
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
// [b28d-solo-CItemList] DIFF | 本轮修：去具名 typeName，GetType 内联实参
//   script->m_strbc（ORIG 0x86209b7 的 C1ERKSs 是**按值形参**拷贝、参数槽 -0x5c）
//   ⇒ p167 309/309 **CLEAR**（原 -2 _Unwind_Resume / +2 D1Ev / +1 C1ERKSs 全消）；
//   残 N104（EH 尾块相对序）+ N82（槽/帧）⇒ 语义完成、byte 层 N 封顶。
// [b28d-solo-CItemList] DIFF | p167 CLEAR（309/309 calls）。本轮 P171 照抄：
//   ORIG 0x086209ab-0x086209c2 无 out1/out2 清零（GetType 引用出参写入）⇒
//   删我方伪造的两条 `movl $0,-0x5c/-0x60`。残 = 相邻四数组零初始化合并
//   （ORIG 0x08620a43 一次 rep stos 0x400 dwords 覆盖 4×1024B；4.4 分开清，
//   实测加 `={0}` 反致 +3 rep stos ⇒ N78 封顶，保持 3 数组初始化原形）。
// [b28d-tu-CItemList] 真结构修正 + 块序翻案：ndiff 660→57，三数不变。
//   ① 缓冲重构（真语义）：前判「四数组一次 rep stos」系误——ORIG
//     0x08620a43 单条 rep stos 0x400 dwords = **一块 4KB escBuf**（基
//     -0x1870），GetEscapeString 四调用 dst = +0/+0x400/+0x800/+0xc00
//     （0x08620be6/0c3f/0cab/2215）；原六独立 1KB 数组系 Ghidra 镜像误读
//     ⇒ 改 3 缓冲形（escBuf 4KB + ac/b4 两个 1KB，b4 缓冲声明在两 if 之间
//     复现 ORIG memset 位次）。
//   ② ep==0/ep!=0 整块换位 + 条件翻 `if (ep != 0)`：ORIG 0x08620cc7
//     `cmpl $0x0,-0x20; je <ep==0 远块>`，原负极性形致 ~600 行 difflib
//     位移 ⇒ 单此一件 625→61。
//   ③ 出参槽序：声明序改 `int out2; int out1;`（ORIG out2@-0x60/out1@
//     -0x64，4.4 先声明占浅槽）⇒ @@@21-46/@@215 五块归零。
//   ④ moduleKey 声明提前（赋值原位复现 ORIG `movl $0x13,-0x38` 位次）：
//     槽 -0x64→-0x5c，距 ORIG -0x38 尚差 8 槽位 ⇒ 4.1 浅区碎片 gap
//     （-0x28..-0x34/-0x3c..-0x54 空置）4.4 恒紧凑分配，N82/N95 封顶。
//   残 57 行 = ④ 槽差 4B 级联（pf/temp 8 块）+ escDst2/dst4 预计算两段形
//   （ORIG `lea base; lea +0x400,%ebx` callee-saved 跨 call，4.4 用点后
//   `lea+add` ⇒ N86b）+ `mov 0x10(%eax),%eax; mov %eax,%ebx` 两段中转 ×2
//   （N18a）+ 尾 fc 析构/EH landing 块序与 `return` bool xor 岛（`== true`
//   形实测被 4.4 折叠，N104）。p167 复验 309/309 CLEAR。
//   [§3 取证补记] docs/class_func_reports/sync_script__CSyncScript/
//   insert_stackable_item_to_db.md 已复核：『## 汇编』段（L80 `mov $0x400,%edx`
//   rep stos@-0x1870、L195 `lea 0x400(%eax)` dst2 段）与本轮 4KB escBuf 重构
//   一致；『## 反编译 C』段（L1484-1487 四独立 1KB 数组）系 Ghidra 聚合假象 =
//   原误转写来源，按汇编段否定。[修正版 p167 复判] 309=309，严格 mangled 差 =
//   8 对 1:1 镜像 C2↔C1/D2↔D1 clone 别名，豁免后零差（Main 裁定口径）。
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
    // [b28d-tu-CItemList] 槽对齐（P326 时代重排）：ORIG 浅槽序 ep@-0x20、
    //   db@-0x24、moduleKey@-0x08620?? 的 -0x38、GetType 值形参串@-0x5c、
    //   out2@-0x60、out1@-0x64（`lea -0x64→0x10(%esp)`=out1、`-0x60→0x14`=out2）
    //   ⇒ moduleKey 声明紧随 db、出参声明序 out2 先于 out1；初始化仍在原位。
    ENUM_MODULE_TYPE moduleKey;
    int out2;
    int out1;
    // [b28d-solo-CItemList] P167/P183：ORIG 0x86209b7 的 `C1ERKSs` 是 GetType
    //   **按值形参**（mangled `7GetTypeESs17ENUM_CHARACTERJOBiRiS2_b`）的实参
    //   拷贝（参数槽 -0x5c，调用后 0x8620a05 D1Ev + landing 0x8620a1a D1Ev +
    //   _Unwind_Resume），**无具名 typeName 局部**——我方具名形多 1×C1ERKSs
    //   + 2×D1Ev EXTRA ⇒ 内联实参。
    if (!GetType(script->m_strbc, (ENUM_CHARACTERJOB)0, 0, out1, out2, false))
    {
        return false;
    }
    CStreamBuffer buf;
    // [b28d-tu-CItemList] 真结构修正：ORIG 0x08620a43 单次 rep stos 0x400
    //   dwords ⇒ 一块 4KB escape 大缓冲（槽基 -0x1870），三处
    //   GetEscapeString 的 dst 依次取其 +0/+0x400/+0x800 段
    //   （ORIG 0x08620be6/0x08620c3f/0x08620c9c）；两个 script2Database
    //   出参缓冲各 1KB（-0x470 memset@0x08620a59 顶部、-0x870
    //   memset@0x08620b14 在 ac 转换成功之后 ⇒ b4 缓冲声明位于两 if 之间）。
    //   原六块独立 1KB 数组（3 具名 dst + 3 未初始化）系误转写。
    char local_1870[0x1000] = {0};
    char local_470[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_strac.c_str(), local_470))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x523, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), script->m_strac.c_str(),
                      "bool sync_script::CSyncScript::insert_stackable_item_to_db(STStackableScript*)",
                      0x523);
        return false;
    }
    char local_870[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_strb4.c_str(), local_870))
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
    buf << ", '" << GetEscapeString(db, local_470, local_1870) << "'";
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_strb0.c_str(), local_1870 + 0x400)
        << "'";
    buf << ", '" << GetEscapeString(db, local_870, local_1870 + 0x800) << "'";
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
    //   [b28d-tu] 声明已提前（槽对齐），赋值原位。
    moduleKey = (ENUM_MODULE_TYPE)0x13;
    EquipParamInfo* ep = (EquipParamInfo*)script->m_module4bc[moduleKey];
    // [b28d-tu-CItemList] P222 正极性翻转（整块换位）：ORIG 0x08620cc7
    //   `cmpl $0x0,-0x20(%ebp); je <ep==0 远块>` ⇒ ep!=0 落体在前。
    //   原 `if (ep == 0){A}else{B}` 在 4.4 生 `jne <B>` + A 落体，
    //   与 ORIG 物理块序整体倒置（difflib 计 ~600 行位移残差）。
    //   改 `if (ep != 0){B}else{A}` 后 4.4 生 `je <A>`，块序与 ORIG 一致。
    if (ep != 0)
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
    else
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
    // ORIG（+0x18ad）：两分支汇合后固定输出 set_type 空串（DAT_08ce36e4）。
    buf << ", ''";
    buf << ", '"
        << GetEscapeString(
               db,
               this->GetURL((char*)script->m_str6c.c_str(),
                            (int)script->m_str6c.length(), script->m_field70),
               // [b28d-tu-CItemList] 真结构修正：ORIG 0x08622215 第 4 处
               // GetEscapeString 的 dst = 大缓冲 +0xc00 段（lea -0x1870;
               // lea 0xc00(%eax),%edi），非独立 1KB 数组。
               local_1870 + 0xc00)
        << "'";
    buf << ", " << script->m_fieldc4;
    // [b28d-tu-CItemList2 2026-09-09] 求值序对齐：ORIG call 序列
    //   0x086222d7 c_str → 0x086222ee set_query → 0x086222f9
    //   CFieldLengthChecker::C1Ev ⇒ 原码先 set_query（含 buf.c_str() 求值）
    //   再构造 fc；原重建把 fc 声明写在 set_query 之前，调用序整体前移 2 位。
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
    CFieldLengthChecker fc;
    fc.AddField(db->GetQueryString());
    // [b28d-tu-CItemList] P59：ORIG 0x0862232b-0x0862233e `xor $0x1,%eax;
    //   test %al,%al; je <mov $1>` 物化岛 ⇒ 原码 `== true` 比较形（仅看 al
    //   LSB，与 4.1 char-bool 语义一致），非裸 return。
    return db->exec(true) == true;
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
    // [b28d-solo-CItemList] NEAR 7 行 | N93：it/e 槽方向（4.1 先声明深
    //   -0x18/-0x10 vs 4.4 先声明浅），交换声明序倒转 C1Ev/end 调用序。
    // [b28c-21] N93 封顶：ORIG it(默认构造,-0x18)/e(-0x10)/find 临时(-0x20)；
    // 4.4 槽序与求值序绑定（交换声明/操作数均致槽或比较参反向），DIFF→NEAR。
    __gnu_cxx::hash_map<int, CCeraShopGoods*>::iterator it;
    // [b26N7b] it/e 槽分配方向 4.1(深→浅) vs 4.4(浅→深) 相反；换声明序虽对齐
    // 槽号但使 C1Ev/end call 序倒转 AE 判 DIFF 倒退，故保持 ORIG 声明序封 NEAR。
    // [b28d-20] fresh 双形实测（ORIG 0x8512358，it=-0x18/e=-0x10）：
    //   原形 it 先声明 ⇒ 4.4 it=-0x10/e=-0x18（槽反，nd=9，verdict NEAR）；
    //   交换形 e 先声明 ⇒ 槽全对齐但 `call C1Ev(it)` 与 `call end()` 顺序倒转
    //   （nd=6，verdict 退为 DIFF）。4.4 槽序=声明序且构造序=声明序，ORIG
    //   「it 深槽 + C1Ev 先执行」在 4.4 下不可兼得 ⇒ N93 封顶，保留 NEAR 形。
    // [b28d-tu-CItemList] P326 翻案实测（本件不适用 ⇒ 回滚，nd 仍 14 但
    //   NEAR→DIFF 倒退）：ORIG 0x08512358 入口序 C1Ev(it)→end→find→cmp，
    //   end() 构造在 find **之前**；P326 内联形把 end 挪到比较点 ⇒ 调用序
    //   倒转 2 处（end/find 交换 + 比较参槽 -0x14 临时介入），三数 near 5→3
    //   /diff 16→18。具名 e 形保持最优（NEAR/nd14，纯槽向漂移）。
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
    // [b28d-solo-CItemList] 复验 = AE（0 差异行）| 修：m_field0/m_field8
    //   int→unsigned int（P265/P211 同型化单段直存）+ P183 去 rates 引用缓存。
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
                sum = sum + it2->m_field8;
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
// [b28d-solo-CItemList] DIFF | 修：正向早返守卫（消 xor$1 物化；p167 CLEAR
//   45/45）；残 N7/N82 槽与寄存器分配。
bool CItemList::insert_item(int group, int idx, CItem* item)
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it;
    it = m_gradeItemList[group].find(idx);
    // [b28d-20] P167：ORIG 调用 operator==（无 ne）；[b28d-solo] 改正向早返守卫：
    //   ORIG 0x081fxxxx `test %al,%al; je <fail 块 +0x154>`（无 xor$1 物化）⇒
    //   源码为 `if (it == end()) { fail; return false; }` 正极性形（P59/P222）。
    // [b28d-tu-CItemList] 块序三形实测（并列读数，均不可兼得）：
    //   ① 本形 `if (it==end()){fail;return false;} {main}`：test/je 同 ORIG
    //     0x08511341、无 xor，但 4.4 把 fail 块内联主段前 ⇒ 42 行块位移，
    //     ndiff 123/gap 1（**最优，保持**）；
    //   ② `if (!(it==end())){main}fail`：4.4 生 `xor $1,%eax` 物化（ORIG 无）
    //     + 全函数位移，ndiff 123→400 反劣；
    //   ③ `if (it!=end()){main}fail`：改调 iterator::ne ⇒ 发明 ORIG 无的
    //     `_Ne` 符号，触铁律1，弃。
    //   ORIG（4.1）fail@+0x154 后置 = 4.1 块布局启发（热路径前置），4.4 不可
    //   从源码复现 ⇒ N104 封顶。[修正版 p167 复判] 45=45，10 对
    //   C2↔C1/D2↔D1 镜像别名，豁免后零差；取证以 CItemList/insert_item.md
    //   汇编段 + objdump 0x0851129e 直证（§3.2 兜底：本件改动仅回滚实验形，
    //   维持前轮封项）。
    if (it == m_gradeItemList[group].end())
    {
        m_gradeItemList[group].insert(std::pair<const int, CItem*>(idx, item));
        m_itemMap.insert(std::pair<const int, CItem*>(idx, item));
        if (item->GetQuestItemDropBouns())
        {
            // [SweepCItemList2 2026-09-10] 真语义 bug 修正（判据⑳ make_pair 键值序）：
            //   ORIG 0x08511441..0x0851146d 直证：call GetQuestItemDropBouns → `mov
            //   %eax,-0x8c`；`mov 0x10(%ebp),%eax → -0x88`；压栈 `0x8(%esp)=&-0x8c`
            //   (arg2) / `0x4(%esp)=&-0x88` (arg1) ⇒ arg1 = **idx**、arg2 = questBonus
            //   （实例 `_ZSt9make_pairImiE…` 要求 arg1 为 unsigned long ⇒ 源码
            //   `make_pair((unsigned long)idx, (int)questBonus)`）。语义互证：
            //   CUser.cpp:8529 查询键是 `Inven_Item::m_addInfo`（Inven_Item.h:269 =
            //   item idx）⇒ 本表键必须是 idx、值才是 questBonus；原键值互换使
            //   GetQuestItemDropBouns(idx) 恒查不到。
            m_lottoCount.insert(
                std::make_pair((unsigned long)idx,
                               (int)item->GetQuestItemDropBouns()));
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
    // [b28d-tu-CItemList2 2026-09-09] 真语义修正：ORIG 0x08511343
    //   `test %al,%al; je 0x085113f2`（fail 段）⇒ `it == end()` 时 fall-through
    //   进插入段、`it != end()`（已存在）才打印 "Insert Fail" 并 return false。
    //   原重建把两分支写反（未找到即报错、已存在反而重复插入）。条件仍调
    //   operator==（ORIG 无 `ne` 符号），不发明符号。
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
// [b28d-solo-CItemList] DIFF（ndiff 307→289）| p167 CLEAR（67/67 calls）。
//   本轮 P225 推广：两处 `GetItemType()==7/0xd && size()!=size()` 拆为嵌套
//   if ⇒ 复现 ORIG 每条件各发 `cmp; sete/setne %al; test %al,%al; je`
//   （合并 && 被 4.4 折成直跳，无 setcc）。残 = 局部槽序/帧偏移（ORIG
//   it=-0x58 紧贴 stackScript；我方 -0x3c）⇒ N82/N93/N95，语义已核。
// [b28d-tu-CItemList 2026-09-09] NEAR（450/450，gap 0）| 本轮两处真修：
//   ① receiver 内联 `GetInstanceSyncScript()->insert_equiptment_item_to_db(...)`，
//      删具名 sync 局部（ORIG 0x8511843 `call; mov %eax,(%esp)` 无槽往返，P0/P183）；
//   ② `!insert() && field!=0x16..0x19` 拆嵌套 if ⇒ 消 4.4 的 && 链 bool 物化
//      （mov$1/jmp/mov$0/test/je 共 5 条），复现 ORIG 纯短路链。p167 CLEAR；
//      残 = 槽基差（it -0x38 vs ORIG -0x58，N93/N95）+ .o 重定位伪差。
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
        // ORIG 0x8511843 `call GetInstanceSyncScript; mov %eax,(%esp)` 无槽往返
        // ⇒ 原码为 receiver 内联实参（P0/P183 禁具名指针缓存）。
        if (!sync_script::GetInstanceSyncScript()
                 ->insert_equiptment_item_to_db(&eqScript, 0, 0, 0))
        {
            if (eqScript.m_field1b4 != 0x16 && eqScript.m_field1b4 != 0x17 &&
                eqScript.m_field1b4 != 0x18 && eqScript.m_field1b4 != 0x19)
            {
                printf("insert_equiptment_item_to_db() fail file - %s", path);
            }
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
        // [b28d-solo] P225 推广：ORIG 0x8511xxx 两条件各发
        //   `cmp; sete/setne %al; test %al,%al; je` ⇒ 源码为嵌套 if 形
        //   （合并 && 被 4.4 折成直跳，无 setcc）。
        if (pStack->GetItemType() == 7)
        {
            if (pStack->m_lotto.m_items.size() !=
                pStack->m_lotto.m_counts.size())
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x94f, 5);
                trace("Stackable item set_item fail (file [%s])", path);
                return false;
            }
        }
        if (pStack->GetItemType() == 0xd)
        {
            if (pStack->m_lotto.m_items.size() !=
                pStack->m_lotto.m_counts.size())
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x957, 5);
                trace("Stackable item set_item fail 2 (file [%s])", path);
                return false;
            }
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
// [b28d-solo-CItemList] DIFF | p167 CLEAR（37/37）；残 N82/N95：帧差 0x10
//   （ORIG 0x13c / 我方 0x14c）+ shopList 基址槽 -0x114/-0x120 平移 0xc，
//   局部集合与调用位次逐条一致，属 4.4 槽基/outgoing 预留族 ⇒ 封顶。
// [b28d-tu-CItemList] 真语义修正 ×2（ndiff 260→242，p167 37/37 CLEAR 复验）：
//   ① `int value = 0;`：ORIG 0x085125d6 在 item 槽写入后、分支前发
//     `movl $0x0,-0x28(%ebp)` ⇒ 原码带零初始化（原重构漏，else 短路路径
//     会读脏栈）。
//   ② lotto insert key/value 对调：ORIG 0x08512865
//     `make_pair<unsigned long&,int&>` 实参槽 `0x4(%esp)=&+0x14`（key=
//     itemIdx 的 unsigned long 别名）、`0x8(%esp)=&+0x10`（value=
//     subGroupIdx）⇒ 正确形 make_pair((unsigned long&)m_field14,
//     (int&)m_field10)，原写反（真行为差，非纯 codegen）。
//   残 242 = 全函数 0xc 槽基平移：ORIG 把 icon/name（callee-saved ebx/esi
//   跨 call）与三个实参暂存免槽，4.4 恒槽化 ⇒ 局部集 +4 槽、shopList 基
//   随对齐平移（N7/N95/N82 族）⇒ 维持封顶。
//   [§3 取证补记 + 修正版 p167 复判] CItemList/Load_Goods_File.md 已核：L160
//   `movl $0x0,-0x28` 与 value=0 修正一致、L543 make_pair 首实参 `+0x14` 与
//   key/value 对调修正一致；37=37，6 对 C2↔C1/D2↔D1 镜像别名，豁免后零差。
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
            // [b28d-tu-CItemList] 真语义修正：ORIG 0x085125d6 `movl $0x0,-0x28(%ebp)`
            //   在 item 槽写入后、分支前 ⇒ 原码 `int value = 0;`。
            int value = 0;
            if (item.m_field14 == 1)
            {
                value = item.m_field38;
            }
            else
            {
                CEquipItem* equip = (CEquipItem*)
                    m_goodsMap[shopList[i][j].m_field8]->GetRelatedItem();
                // [b28d-solo] p167：ORIG 调 const 版 vector::operator[]
                //   （MISSING `operator[](unsigned int) const`）⇒ 指针 const 化。
                const std::vector<stAvatarTypeSelect_t>* avatarList =
                    (const std::vector<stAvatarTypeSelect_t>*)
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
                // [b28d-tu-CItemList] 真语义修正（参数颠倒）：ORIG 0x08512865
                // `make_pair<unsigned long&,int&>(unsigned long&,int&)` 实参槽
                // 0x4(%esp)=&+0x14（itemIdx）、0x8(%esp)=&+0x10（subGroupIdx）
                // ⇒ 原码 key 取 m_field14 的 unsigned long 别名、value 取
                // m_field10，原重构写反（key/value 对调是真行为差）。
                m_lottoMap.insert(std::make_pair<unsigned long&, int&>(
                    (unsigned long&)shopList[i][j].m_field14,
                    (int&)shopList[i][j].m_field10));
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
// [b28d-solo-CItemList] **AE（153/153，ndiff 0，p167 14/14 CLEAR）**：
//   ① insert 实参改 `std::pair<const long long, Data>(key, data)` 直构 ⇒
//   复现 ORIG 0x081a302f `pair<const long long,Data>::C1<long long&,Data&>`
//   converting ctor（消原 EXTRA 的引用对 + Data 拷贝）；
//   ② insert 结果改具名局部 `ins` ⇒ 复现 ORIG 0x081a3059 先析构临时 pair、
//   0x081a3075 再 `movzbl -0x58; xor $1; test` 读 .second 的次序。
int Arad_GiftItem_Set::open(STStackableScript* script)
{
    // ORIG：if (*(int*)(param_1 + 0x178) != 0) —— Arad_STStackableScript.m_0（+0x178）
    if (script->m_arad178.m_0 == 0)
    {
        return 0;
    }

    // ORIG：kind = str_to_enum(c_str(script + 0x180))——调用真实 str_to_enum。
    // [b28d-solo-CItemList] P183 去缓存：ORIG 直把 c_str() 返回值压 0x4(%esp)
    //   （无 name 具名槽；我方具名槽多 mov store+load 一对）。
    const int kind = str_to_enum(script->m_arad178.m_str8.c_str());
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
            // [b28d-solo] 具名局部形实测反劣（EXTRA 引用对构造 + 拷贝构造）；
            //   ORIG converting ctor `pair<const long long,Data>::C1<long long&,
            //   Data&>` 由内联临时形经 4.1 前端直构实现 ⇒ 保持内联。
            std::pair<std::map<long long, Data>::iterator, bool> ins =
                m_map.insert(std::pair<const long long, Data>(key, data));
            if (!ins.second)
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
// [b28d-solo-CItemList] **AE（101/101，ndiff 0，p167 10/10 CLEAR）**：改用
//   P307 形 `register const unsigned long* pseed = &seed;` + `new CMTRand(*pseed)`
//   ⇒ 4.4 复现 ORIG 0x0832c48d `lea -0x1c(%ebp),%esi`（取址实参提前入
//   callee-saved 跨 operator new 0x832c497 保活）。旧 N18a/N7 判词作废。
CCompound_ExtreamDun_Item::CCompound_ExtreamDun_Item()
{
    const unsigned long seed = time(0);
    register const unsigned long* pseed = &seed;
    m_rand = new CMTRand(*pseed);
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
// [b28d-solo-CItemList] DIFF（ndiff 30↓）| P310：`int i` + 显式
//   `(unsigned int)i < (unsigned int)GROUP_NUM_MAX` ⇒ 复现 ORIG 0x0832ded9
//   `mov -0x14(%ebp),%eax; cmp $0x3a,%eax; setbe` 两段尾测（原 unsigned i
//   被 4.4 折成 `cmpl $0x3a,-0x14` 单段）。T1/T2 直证：mov+cmp 两段 + setbe
//   无符号序。残 = m_bindItems[i][j] 乘法链寄存器角色（ORIG 0x0832de??
//   `lea 0x0(,%eax,4),%ecx`）⇒ N86b。
// [b28d-tu-CItemList 2026-09-09] 复核：101/103（gap +2）——两处 m_bindItems[i][j]
//   索引乘法链的寄存器角色（ORIG `lea 0x0(,%eax,4),%ecx` vs 4.4
//   `lea 0x0(,%edx,4),%eax; mov %eax,%edx`）；P309 全头部声明形实测零变化 ⇒ N86b。
void CCompound_ExtreamDun_Item::after_addItem()                     // ORIG 0x832de18
{
    for (int i = 1; (unsigned int)i < (unsigned int)GROUP_NUM_MAX; ++i)
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
// [b28d-solo-CItemList] DIFF（78/79，ndiff 13↓）| entry/entry2 改匿名临时实参
//   ⇒ 复现 ORIG 0x0832dd70/-0x1c、0x0832ddc1/-0x14 槽向（具名连排落
//   -0x14/-0x1c 反向，N93）。残 = m_bindItems[group][cat] 二维索引乘法链
//   寄存器角色（ORIG 0x0832dde2 `lea 0x0(,%eax,4),%ecx` 用 ecx；4.4 用
//   eax→edx 多 1 mov）⇒ N86b「同一算术多 codegen」；T5：类型已定（cat/索引
//   均 32 位），禁再改类型（S1 已用一次，unsigned cat 实测零变化）。
// [b28d-tu-CItemList 2026-09-09] 复核：78/79（gap +1）——同 after_addItem 的
//   m_bindItems[group][cat] 索引链 N86b 残差；p167 与 ORIG 调用多重集一致。
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
    m_data.m_resultItems[cat].push_back(
        OUTPUT_ITEM_STR(item.getUsableLevel(), item.get_index()));
    m_data.m_bindItems[item.getItemGroupName()][cat].push_back(
        OUTPUT_ITEM_STR(item.getUsableLevel(), item.get_index()));
    return true;
}

// [b28d-20] _isPossibleResultItem（ORIG 0x832db4e，140/135，nd 27→14）：
//   本轮修 P5 裸虚表→`item.IsRandomOption()`、P50 先声明后赋值（复现 ORIG
//   0x832db8d 独立 `iterator::C1Ev`）、正极性返回 `if (IsRandomOption())
//   return false; return true;`（ORIG 0x832dd09 `test; je <mov $1>`）。
//   残差 = it/endIt 槽互换（ORIG it=-0x14/endIt=-0x10，4.4 先声明占浅槽）
//   与帧 0x38/0x28 ⇒ N93 封顶（交换声明序会倒转 C1Ev/end 调用序，同
//   FindGoods 实证）。
// [b28d-solo-CItemList] NEAR | 修：尾 return 改 if-return 补 ORIG 的
//   bool 物化岛（sete→test/je/mov$1/jmp/mov$0；27→7 差异行）；残 N93 槽。
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
    // [b28d-tu-CItemList] P326 翻案实测（本件不适用 ⇒ 回滚具名形）：
    //   P326 形 `it != const_iterator(m.end())` 把 end() 挪到比较点，但
    //   ORIG 0x0832db8d 起入口序为 C1Ev(it)→end→find→cmp（end 构造在
    //   find **之前**）⇒ 内联形多 2 处调用序倒转，ndiff 14→20、NEAR→DIFF
    //   倒退。具名 endIt 形（4.4 先声明占浅槽 -0x10? 实测 it=-0x10/
    //   endIt=-0x14 反槽）保留为最优 NEAR/nd14 形。N93 判词对本件仍有效。
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
        if (_convertInputItem(
                (ENUM_UNUSABLE_EQUIPMENT_TYPE)
                    item.GetUsableEquipmentType()) == 1)
        {
            return true;
        }
        return false;
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
// [b28d-solo-CItemList] 复验 = IDENTICAL/AE（compare_tu 口径 0 差异行）。
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
        // ORIG +0x173 `jle`（有符号 ≤0 落 bVar1=true 岛）/ +0x18b `jbe`（无符号
        // ≤0x3a 落 bVar1=false 岛）双岛形 ⇒ 原码是短路 `||`：`(signed)a <= 0 ||
        // (unsigned)b > 0x3a`（旧 `&&` 写法把区间判反 = 真语义 bug）。
        if (refs[i]->getItemGroupName() <= 0 ||
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
// [b28d-solo-CItemList] DIFF | p167 CLEAR（18/18 calls）。本轮按 ORIG 槽序
//   （0x0832cced baseLevel=-0x28 / 0x0832cd0c inRarity=-0x24 / 0x0832cd0f
//   outputRarity=-0x20 / 0x0832cd42 r0=-0x1c / 0x0832cd71 r1=-0x18 /
//   0x0832cd74 found=-0x14）重排声明：outputRarity 前置、found 提到分支前。
//   残 = 全帧槽基差 4（我方 -0x24 起，ORIG -0x28 起；帧同为 0x94）⇒ N95/N115
//   （4.4 帧顶 8B gap/outgoing 预留，TU 级旗标，禁 pad 强凑）+ gap 5（除法链）。
// [b28d-tu-CItemList] 真语义修正 ×2（range 颠倒）：第 2/3 处
//   `_FindItemAll_BindSphere` 的 pair 实参原写 (baseLevel+5, +1) /
//   (baseLevel+0xa, +6)，ORIG 0x0832cea2-0x0832ceb7 ctor 槽
//   `+1→-0x3c→0x4(%esp)`(first)、`+5→-0x40→0x8(%esp)`(second) 与
//   0x0832cf09-0x0832cf1e `+6→0x4`、`+0xa→0x8` ⇒ 正确形 = (+1,+5) / (+6,+10)
//   （同 _exception_Rare 前轮修正形，本函数漏改）。p167 18/18 CLEAR 复验。
//   残差 = 帧基 4B 漂移（N95 级联）+ 条件短路 bool 临时槽物化岛（ORIG
//   `movl $0x0,-0x10` ×2，4.4 折叠，N7）+ `mov mem,%eax; test` 形 vs 4.4
//   `cmpl $0x0,mem` 折形（N18a）。
//   [§3 取证补记 + 修正版 p167 复判] ExtreamDungeon__CCompound_ExtreamDun_Item/
//   makeCompoundItem.md 汇编段 L165-171（+5→-0x40→0x8、+1→-0x3c→0x4）与
//   L194-197（+0xa→0x8、+6→0x4）逐行支撑本件 pair 修正；18=18，3 对
//   pairIjjEC2↔C1 镜像别名，豁免后零差。
bool CCompound_ExtreamDun_Item::makeCompoundItem(                   // ORIG T 0x832ccc0
    const CEquipItem** refs, char select, Inven_Item& out, eBindSphereType bind)
{
    out.reset();
    // [b28d-tu-CItemList2 2026-09-09] P211/P274 同型化实测反劣（250/250→248，
    //   inRarity 两处 `mov mem,%eax` 被折掉）⇒ 保持 int baseLevel（P321 T5：
    //   类型不用于凑装载序）。
    int baseLevel = _get_base_level(refs);
    int inRarity = _convertInputItem(
        (ENUM_UNUSABLE_EQUIPMENT_TYPE)refs[0]->GetUsableEquipmentType());

    // [b28d-solo] ORIG 槽序（0x832cced/-0x28、0x832cd0c/-0x24、0x832cd0f/-0x20、
    //   0x832cd42/-0x1c、0x832cd71/-0x18、0x832cd74/-0x14）⇒ outputRarity 与
    //   found 的声明须前置于 r0/r1 之间与分支之前（P28/P309 声明序=槽序）。
    unsigned int outputRarity = 4;
    // 双次 randInt()%1000000 + 1（magic 常量 0x431bde83 实证；ORIG 为无符号除法）
    unsigned int r0 = ((unsigned int)m_rand->randInt() % 1000000) + 1;
    unsigned int r1 = ((unsigned int)m_rand->randInt() % 1000000) + 1;
    // [b28d-tu-CItemList2 2026-09-09] ORIG 0x0832cd74 `movl $0x0,-0x14(%ebp)`：
    //   原码在两次 randInt 之后另有一个置 0 的具名 int 局部（Ghidra local_18，
    //   全函数只写不读），它占据 -0x14 槽并决定后续 found(-0x10) 与 sret(-0x60)
    //   的槽位。按 P0 双槽判据恢复该具名局部（死写照抄 P171）。
    int legacyValue = 0;
    (void)legacyValue;
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

    // [b28d-tu-CItemList2 2026-09-09] 真语义修正：ORIG 0x0832cdcc `mov
    //   -0x1c(%ebp),%edx; mov %edx,0xc(%esp)`（Ghidra `local_20` = 第一次
    //   randInt 的 r0）⇒ _determine_level_bonus 第二实参是 **r0**（掷点值），
    //   原重建误传 baseLevel。同时 found 声明后移至本调用之后（ORIG
    //   0x0832cde5 `movl $0x0,-0x10(%ebp)` 在 call 之后，-0x14 才是上句 legacy）。
    std::pair<unsigned int, unsigned int> bonus = _determine_level_bonus(
        (ENUM_INPUTITEMS_RARITY)inRarity, r0,
        (ENUM_OUTPUTITEMS_RARITY)outputRarity, bind);

    int found = 0;
    // [b28d-tu-CItemList2 2026-09-09] 正极性 if/else：ORIG 0x0832cdf2
    //   `jne 0x0832ce24`(else) + 真分支 `movl $0x0,-0x10(%ebp); jmp`
    //   （Ghidra C `if (cond) { local_14 = 0; } else { … }`）⇒ 条件成立时
    //   显式 found = 0，取反形会折成 `ja` 单跳少 2 条。
    if (bonus.second == 0xa && inRarity != 1 &&
        ((unsigned int)baseLevel % 10) > 4)
    {
        found = 0;
    }
    else
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
                // [b28d-tu-CItemList] 真语义修正（range 颠倒）：ORIG
                // 0x0832cea2-0x0832ceb7 段 ctor 实参槽 0x4(%esp)=-0x3c
                // （baseLevel+1）、0x8(%esp)=-0x40（baseLevel+5）⇒
                // pair = (first=+1, second=+5)；原写 (+5,+1) 使
                // range.first/second 颠倒（同 _exception_Rare 前轮修正形）。
                std::pair<unsigned int, unsigned int>(
                    (unsigned int)(baseLevel + 1),
                    (unsigned int)(baseLevel + 5)));
            if (found == 0)
            {
                found = _FindItemAll_BindSphere(refs, select,
                    (ENUM_OUTPUTITEMS_RARITY)outputRarity,
                    // [b28d-tu-CItemList] 真语义修正（同上）：ORIG
                    // 0x0832cf09-0x0832cf1e `+0xa → -0x30 → 0x8(%esp)`、
                    // `+6 → -0x2c → 0x4(%esp)` ⇒ pair = (+6, +10)。
                    std::pair<unsigned int, unsigned int>(
                        (unsigned int)(baseLevel + 6),
                        (unsigned int)(baseLevel + 0xa)));
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
    // [b28d-tu-CItemList2 2026-09-09] P92/P0：ORIG 0x0832cfc5-0x0832cfd4
    //   `mov 0x14(%ebp),%eax; mov -0x10(%ebp),%edx; mov %edx,0x2(%eax)` 后
    //   **重新** `mov 0x2(%eax),%eax; mov %eax,%ebx; call G_CDataManager` ⇒
    //   实参来源是 out.m_addInfo 字段而非 found 局部。
    CItem* info = G_CDataManager()->find_item(out.m_addInfo);
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
// [b28d-solo-CItemList] AE（91/91，ndiff 0）| 本轮修：清零序解耦——声明序定槽
//   （lo 浅 -0x10 / hi 深 -0x14）、赋值语句序定 store 序（`hi=0; lo=0;`）复现
//   ORIG 先深后浅；4.4 对「声明+初始化」恒按声明序 store（旧 N93 lo/hi 槽序差
//   由此消除）。
std::pair<unsigned int, unsigned int>                              // ORIG T 0x832c9ca
CCompound_ExtreamDun_Item::_determine_level_bonus(
    unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity,
    unsigned int& idxOut, eBindSphereType bind) const
{
    idxOut = _determine_level_bonus_index(baseLevel, inRarity, bind);
    // [b28d-solo] 真语义修正：ORIG 0x832c9ca 段三次 `add $0xb80; add 0xc(%ebp);
    //   add $0xc` = m_lvBonus[bind]+0x0 ⇒ **m_levels**（等级表），非 m_rates[0]
    //   （0xb80+0x18=0xb98=m_rates[0]）。m_rates 仅供 _determine_level_bonus_index
    //   掷点用；本函数返回 pair(下界=levels[idx-1]+1, 上界=levels[idx])。
    //   P183/P157：无 rates/levels 引用局部（ORIG 每次重算成员地址）。
    // [b28d-solo] ORIG 0x832ca06 `mov (%eax),%esi` + `cmp %eax,%esi; jb`：
    //   idx 在比较左位（先物化到 callee-saved 跨 size() call）。
    if (idxOut == 0 ||
        idxOut >= m_lvBonus[(int)bind].m_levels.size())
    {
        // [b28d-solo-CItemList] ORIG 清零序 = 深槽 -0x14(hi) 先、浅槽 -0x10(lo) 后；
        //   4.4 槽按声明序（lo 浅/hi 深）⇒ 声明不带初始化、赋值语句按 hi→lo 序。
        int lo;
        int hi;
        hi = 0;
        lo = 0;
        return std::pair<unsigned int, unsigned int>(lo, hi);  // pair<int,int> 形
    }
    return std::pair<unsigned int, unsigned int>(
        m_lvBonus[(int)bind].m_levels[idxOut - 1] + 1,
        m_lvBonus[(int)bind].m_levels[idxOut]);
}

// [b28d-solo-CItemList] AE（95/95，ndiff 0）| 本轮修：同 Eii 版清零序解耦
//   （声明 lo/hi + 赋值 `hi=0; lo=0;` 复现 ORIG 先 -0x18 后 -0x14）。
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
    // [b28d-solo] 同上：ORIG 0x832cadc 段 `add $0xc` = m_levels（非 m_rates[0]）；
    //   P183/P157 去引用缓存（ORIG 逐次重算成员地址）。
    if (idx == 0 || m_lvBonus[(int)bind].m_levels.size() <= idx)
    {
        // [b28d-solo-CItemList] 同 Eii 版：ORIG 先 hi(-0x18) 后 lo(-0x14)。
        int lo;
        int hi;
        hi = 0;
        lo = 0;
        return std::pair<unsigned int, unsigned int>(lo, hi);
    }
    return std::pair<unsigned int, unsigned int>(
        m_lvBonus[(int)bind].m_levels[idx - 1] + 1,
        m_lvBonus[(int)bind].m_levels[idx]);
}

// [b28d-20] _determine_level_bonus_index（ORIG 0x832cc02，64/63，nd 68）：
//   残差 = 帧 0x24(ORIG)/0x28(我方)+push ebx 有无 + idx 缓存寄存器（ORIG
//   `mov -0xc(%ebp),%ebx`）与 m_lvBonus 索引乘法展开（4.1 imul 链 vs 4.4
//   lea 链）；语义等价（RARITY 守卫与 while 短路链逐条对应）⇒ N7/N82 封顶。
// [b28d-solo-CItemList] DIFF（64/63）| 本轮修：合并 && → `while(size())`+
//   `if(rates[idx] >= baseLevel) break` 复现 ORIG 循环布局（短路第二项落 body、
//   第一项落尾条件；ndiff 75→45）；残 N7：idx 的 %ebx 跨 call 缓存
//   （ORIG `push %ebx` + `mov -0xc,%ebx`）与乘法链/`lea 0x18` vs `add $0x18`。
// [b28d-solo-CItemList] DIFF（64/63）| 本轮两形实测均反劣后封 N：
//   ① `register unsigned int idx` ⇒ idx 免槽（`mov $0,%ebx`）、帧 0x28→0x14，
//   与 ORIG 的 -0xc 槽往返不符；② 语句表达式实参 `[({ idx; })]` 零变化。
//   ORIG 0x0832cc2b `mov -0xc(%ebp),%ebx` 是 4.1 的提前装载（P75 跨版本调度），
//   4.4 恒在使用点 `mov -0xc,%eax` ⇒ 判 N（语义等价，p167 2/2 CLEAR）。
// [b28d-tu-CItemList 2026-09-09] 复核：65/64（gap −1）——残 = idx 的 %ebx 跨
//   call 保活（ORIG `push %ebx` + `mov -0xc(%ebp),%ebx`）与 4.4 使用点重载；
//   register 形实测 idx 免槽（帧 0x28→0x14）反劣，P231/P307 不适用 ⇒ N7/N82。
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
    // [b28d-solo] P231 不适用：ORIG 0x832cc02 的 idx 占槽 -0xc（每轮
    //   `mov -0xc(%ebp),%ebx` 重载）——register 形实测免槽（`mov $0,%ebx`）反劣。
    // [b28d-solo-CItemList] P225/循环旋转：ORIG 4.1 把短路链**第二项**
    //   (rates[idx] >= baseLevel，setae 物化) 落在 body、第一项 (size()!=0)
    //   落在尾条件；4.4 对合并 && 恒反向展开（++idx 落 body）。改
    //   while(size())+if-break 复现 ORIG 布局。
    unsigned int idx = 0;
    while (m_lvBonus[(int)bind].m_rates[(int)inRarity].size() != 0)
    {
        if (m_lvBonus[(int)bind].m_rates[(int)inRarity][idx] >= baseLevel)
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
    // [b28d-solo-CItemList] AE（106/106，ndiff 0）| 本轮 3 处真语义修正：
    //   ① 第三实参传**循环变量 c**（ORIG `mov -0xc(%ebp),%ebx; mov %ebx,0xc(%esp)`），
    //   非 outRarity 原值；② c>1 分支第二实参 = baseLevel + g*5u（ORIG 0x832d0bc
    //   段 `shl $2; add; add baseLevel`），原写 baseLevel+g 使上界错位；
    //   ③ pair 顺序 (+1,+5)（ORIG ctor 0x4/0x8(%esp) = -0x28/-0x2c），原 (+5,+1)
    //   使 range.first/second 颠倒。另：`c <= 0` 复现 `cmpl $0; jg <else>` 布局、
    //   分支内各自 found 检查复现 ORIG 逐分支 `cmpl/je/mov/jmp`。
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
            // [b28d-solo-CItemList] 真语义修正 ×2（ORIG 0x832cffe 汇编为准）：
            //   ① 第三实参传**循环变量 c**（ORIG `mov -0xc(%ebp),%ebx;
            //      mov %ebx,0xc(%esp)`），非 outRarity 原值；
            //   ② c>1 分支第二实参 = baseLevel + g*5u（ORIG 0x832d0bc 段
            //      `shl $2; add %edx,%eax; add 0x18(%ebp),%eax` = 5g+baseLevel），
            //      原写 baseLevel+g 使上界错位。
            //   条件用 `c <= 0` 复现 ORIG `cmpl $0x0,-0xc; jg <else>` 布局
            //   （`c == 0` 生 jne）。
            if (c <= 0)
            {
                // [b28d-solo] P167/P192：ORIG 0x832cffe 用 pair<unsigned,unsigned>
                //   直构（MISSING `pair<unsigned,unsigned>::pair<unsigned&&,
                //   unsigned&&>`），make_pair 落 EXTRA。
                //   真语义修正③：ORIG ctor 0x4/0x8(%esp) = -0x28/-0x2c ⇒
                //   pair = (+1, +5)（原写 (+5, +1) 使 range.first/second 颠倒）；
                //   4.1 右→左求值 ⇒ 先算 second(+5) 存 -0x2c、后算 first(+1) 存 -0x28。
                found = _FindItemAll_BindSphere(refs, select,
                    (ENUM_OUTPUTITEMS_RARITY)c,
                    std::pair<unsigned int, unsigned int>(
                        baseLevel + g * 5u + 1u, baseLevel + g * 5u + 5u));
                if (found != 0)
                {
                    return found;
                }
            }
            else if (c > 1)
            {
                found = _FindItemAll_BindSphere(refs, select,
                    (ENUM_OUTPUTITEMS_RARITY)c,
                    std::pair<unsigned int, unsigned int>(baseLevel + g,
                                                          baseLevel + g * 5u));
                if (found != 0)
                {
                    return found;
                }
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
// [b28d-solo-CItemList] DIFF（109/111，ndiff 30↓）| lowKey/highKey 改匿名临时
//   实参 ⇒ 复现 ORIG 0x0832d218/-0x18、0x0832d260/-0x10 key 槽（ndiff 54→30）。
//   残 = m_bindItems[group][outRarity] 二维索引乘法链寄存器角色（ORIG 0x0832d19f
//   `lea 0x0(,%eax,4),%ecx` vs 4.4 eax→edx，多 1 mov×2 处）⇒ N86b；T5 禁改类型
//   （S1 已试 (int)outRarity cast，零变化）。比较用 `lo == tend` 保持。
// [b28d-tu-CItemList 2026-09-09] 复核：109/111（gap +2）——两处
//   m_bindItems[group][outRarity] 索引链寄存器角色（N86b）；lower_bound/
//   upper_bound/BindTable_Predicate 调用点与 ORIG 一致，余为 .o 重定位伪差。
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
    // [b28d-solo] §4.3 具名化：m_data.m_bindItems[group][outRarity]（等价
    //   this+0x30+48*group+12*outRarity），去裸偏移 reinterpret_cast。
    // [b28d-solo-CItemList] P183/P167：ORIG 0x832d15e **无 table 引用槽**——
    //   begin()/end() 各自重算完整地址（0x832d193 与 0x832d1cd 两段 12 条
    //   全等地址链），且只调 begin()/end() 各一次，lower_bound/upper_bound
    //   复用 tbegin/tend ⇒ 边界缓存但地址内联（消 -0xc 引用槽）。
    ItemVec::const_iterator tbegin = m_data.m_bindItems[group][outRarity].begin();
    ItemVec::const_iterator tend = m_data.m_bindItems[group][outRarity].end();
    ItemVec::const_iterator lo = std::lower_bound(
        tbegin, tend, OUTPUT_ITEM_STR((int)range.first, 0),
        &BindTable_Predicate);
    ItemVec::const_iterator hi = std::upper_bound(
        tbegin, tend, OUTPUT_ITEM_STR((int)range.second, 0x7fffffff),
        &BindTable_Predicate);
    // [b28d-solo] P167/P171：ORIG 0x832d296 段 `operator==(lo, tend)`——比较
    //   对象是 **tend**(-0x20) 而非 hi(-0x28)（hi 仅供 _FindItemRelateCreationRate）。
    if (lo == tend)
    {
        return 0;
    }
    return _FindItemRelateCreationRate(lo, hi);
}

// [b28d-20] _FindRandomItem（ORIG 0x832d2ce，104/100，nd 48）：残差 =
//   m_resultItems 基址折叠形态（ORIG `add 0x8(%ebp); add $0x4` 两步 vs 4.4
//   lea 合并）+ pair 构造临时槽方向 + 尾块 `mov $0/-0x38` 岛；调用点与谓词
//   一致 ⇒ N86/N93 封顶。
// [b28d-solo-CItemList] **AE（104/104，ndiff 0，p167 11/11 CLEAR）**：
//   ① lowKey/highKey 改匿名临时实参 ⇒ 复现 ORIG 0x0832d318/-0x30、
//   0x0832d38a/-0x20 的 key 槽（具名形落 -0x28/-0x34，N93 槽向）；
//   ② 比较改 `table.end() != lo` ⇒ 复现 ORIG 0x0832d3ff `lea -0x34; mov
//   0x4(%esp)`（operator!=(end,lo) 实参序，从右向左压栈）。旧 NEAR 作废。
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
    // [b28d-solo] §4.3 具名化：直接成员访问 m_data.m_resultItems[outRarity]
    //   （等价 this+0xb40+4+12*outRarity），去掉裸偏移 reinterpret_cast。
    const ItemVec& table = m_data.m_resultItems[(int)outRarity];
    ItemVec::const_iterator lo = std::lower_bound(
        table.begin(), table.end(), OUTPUT_ITEM_STR((int)range.first, 0),
        &BindTable_Predicate);
    ItemVec::const_iterator hi = std::upper_bound(
        table.begin(), table.end(),
        OUTPUT_ITEM_STR((int)range.second, 0x7fffffff), &BindTable_Predicate);
    // [b28d-solo] P167/P220/P171：ORIG 0x832d3ea 在比较前**再调一次
    //   table.end()**（sret -0x10）后 `operator!=(lo, end)`（0x832d40c）⇒
    //   源码为 `lo != table.end()`（非 lo != hi；gap 6 = 该次 end 调用）。
    if (table.end() != lo)
    {
        return _FindItemRelateCreationRate(lo, hi);
    }
    return 0;
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

// [b28d-tu-CItemList 2026-09-09] NEAR（212/212，gap 0，p167 33/33 CLEAR）| 本轮四修真：
//   ① pair 显式构造 `std::pair<int,unsigned int>(a,b)` 替 make_pair+cast（ORIG
//      `C1IRKiS3_`/`C1IiRKiE` 引用对构造，消 2 处 cast 临时物化，P61/P81）；
//   ② `if (entries.size() != 0)` 正极性替 `==0` 早返（ORIG 0x832d67c setne 岛，P220）；
//   ③ `(unsigned)m_rand->randInt() % totalGen` 无符号除法（ORIG 0x832d6e1
//      `mov $0x0,%edx; divl -0x1c`；原形生 `sar $0x1f; idivl` 负数语义不同）；
//   ④ totalGen 与返回类型 unsigned 化（ORIG `divl mem` 直用内存操作数 + 两处
//      `mov 0x4(%eax),%ebx` 单段直落，P211/P254 同型化旋钮）。
//   残 = 槽基差（entries/it -0x3c/-0x40 vs ORIG -0x40/-0x44）+ .o 重定位伪差。
unsigned int CCompound_ExtreamDun_Item::_FindItemRelateCreationRate(  // ORIG T 0x832d4b4
    std::vector<OUTPUT_ITEM_STR>::const_iterator begin,
    std::vector<OUTPUT_ITEM_STR>::const_iterator end) const
{
    if (begin == end)
    {
        return 0;
    }
    std::vector<std::pair<int, unsigned int> > entries;   // (权重, itemIdx)
    // ORIG 0x832d6e1 `mov $0x0,%edx; divl -0x1c(%ebp)` 直接内存操作数 ⇒
    // totalGen 声明为 unsigned int（int 时 4.4 先 `mov -0x1c,%ecx` 再 div）。
    unsigned int totalGen = 0;
    for (std::vector<OUTPUT_ITEM_STR>::const_iterator it = begin; it != end; ++it)
    {
        CItem* info = G_CDataManager()->find_item(it->m_index);
        if (info == 0)
        {
            continue;
        }
        // ORIG 0x832d52b 独立 `_Rb_tree_const_iterator::C1Ev`（rule 默认构造）
        // + 0x832d543 `end()`（具名 endIt 直接构造）+ 0x832d570 `find` 临时槽
        // -0x4c 再拷贝回 rule(-0x48) ⇒ 原码为「先声明后赋值」+ 具名 endIt
        // （P50/P28；初始化式会折叠 C1Ev 与临时槽）。
        std::map<int, forceRule_ResultItem>::const_iterator rule;
        std::map<int, forceRule_ResultItem>::const_iterator endIt =
            m_data.m_forceRules.end();
        rule = m_data.m_forceRules.find(it->m_index);
        if (rule != endIt)
        {
            // ORIG 0x832d5be 调 `pair<int,unsigned>::C1<int const&,int const&>`
            // （`_ZNSt4pairIijEC1IRKiS3_EEOT_OT0_`）——实参直接取
            // `&rule->second.m_value` / `&it->m_index` 引用，无 cast 临时物化；
            // make_pair + (unsigned)cast 会多 2 条 mov + 2 个局部槽（P61/P81）。
            entries.push_back(std::pair<int, unsigned int>(
                rule->second.m_value, it->m_index));
            totalGen += rule->second.m_value;
        }
        else
        {
            // ORIG 0x832d615 调 `pair<int,unsigned>::C1<int,int const&>`
            // （`_ZNSt4pairIijEC1IiRKiEEOT_OT0_`）：第一参按值（get_gen_rate
            // 右值）、第二参取 `&it->m_index` 引用。
            entries.push_back(std::pair<int, unsigned int>(
                info->get_gen_rate(), it->m_index));
            totalGen += info->get_gen_rate();
        }
    }
    unsigned int pick = 0;
    if (totalGen == 0)
    {
        // ORIG 0x832d67a `test %eax,%eax; setne %al; test; je <mov $0,%ebx>`
        // ⇒ 正极性（size()!=0 fall-through 到 randInt 路径），非 `==0` 早返
        //   （4.4 对 `==0` 生 sete 反向岛，P220/P47）。
        if (entries.size() != 0)
        {
            return entries[m_rand->randInt() % entries.size()].second;
        }
        return 0;
    }
    // ORIG 0x832d6dc `mov $0x0,%edx; divl -0x1c(%ebp)` 为**无符号**除法 ⇒
    // 两操作数均按 unsigned 取模（原 `(unsigned)(randInt() % totalGen)` 生
    // `sar $0x1f; idivl` 有符号除法，负数语义不同）。
    pick = (unsigned int)m_rand->randInt() % totalGen + 1;
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
// [b28d-solo-CItemList] AE（222/222，ndiff 0）| 本轮修：① scratch/token 声明序
//   交换（复现 ORIG token=-0x50/scratch=-0x4c）；② 补 dead bool `bScan`（复现
//   ORIG `movb $0,-0x35` 预清，P171/P198①）；③ while(cond)→for(;;)+
//   `if(!ScanType) break`（复现头条件 + `xor $1` 物化，P283/P168a）；
//   ④ CItemList.h `m_rarityDicision` int→unsigned int（P211 字段侧旋钮，
//   复现 RHS-first store；sizeof/偏移不变）。
int CCompound_ExtreamDun_Item::LoadScript(const char* dir, const char* path)
{
    if (!loadRDARScriptFile(dir, path))
    {
        return 0x62;
    }

    // [b28d-solo-CItemList] 槽序：ORIG token=-0x50/scratch=-0x4c（构造序
    //   scratch 先）⇒ 源码声明序 scratch, token（4.4 先声明浅）。
    std::string scratch;
    std::string token;
    // [b28d-solo-CItemList] P171/P198①：ORIG 0x832c65e 循环前有
    //   `movb $0x0,-0x35(%ebp)`（只写不读的 bool 局部预清）⇒ 补 dead bool
    //   声明复现该 store。
    bool bScan = false;
    (void)bScan;

    // [b28d-solo-CItemList] P283/P168a：ORIG 循环条件在**头**（`jmp L_cond;
    //   L_cond: call ScanType; xor $1; test; jne <exit>`），4.4 对 while(cond)
    //   恒旋转成尾条件 ⇒ 改 for(;;)+if(!ScanType) break 复现头条件与 xor 物化。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
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
