// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "CItemList/GetSocketTypeCharacter.inc"
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

// ============================================================================
// [T1 #2 / P327 GI 键名对齐] 定义前移：ORIG 期望本 TU 的 `_GLOBAL__I_` 键名为
//   `_ZN14ExtreamDungeon19BindTable_PredicateERKNS_15OUTPUT_ITEM_STRES2_`
//   （ORIG 地址 0x0832dfa2，其 CU = Compound_ExtreamDun_Item.cpp）。
//   P327：键名 = 本 TU 内**第一个「强符号 · 外部链接 · 本 TU 定义」**的 file-scope
//   实体（外联函数取 mangled）。原定义位于下方 namespace ExtreamDungeon 块中
//   （文件后部），使键名落到 `sync_script::GetInstanceSyncScript`（= only_new）。
//   依 P327「只移函数定义、禁移带动态初始化的对象」把本**函数**定义前移到首个
//   强符号位（include 区之后、`namespace sync_script` 之前）；函数体逐字未改。
//   验证：`nm /tmp/tu_CItemList.o | grep -o '_GLOBAL__I_.*'` == ORIG 期望键名 +
//   `check_tu_routed.sh` 四数零回退（693/1357/6/14）。
// ============================================================================
namespace ExtreamDungeon
{

#include "CItemList/BindTable_Predicate.inc"
}

}  // namespace ExtreamDungeon

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

#include "CItemList/DisplayMaxLength.inc"
}

#include "CItemList/CSyncScript.inc"
}

#include "CItemList/dtor_CSyncScript.inc"
}

#include "CItemList/truncate_dnf_item_info_table.inc"
}

#include "CItemList/truncate_artifact_info_table.inc"
}

#include "CItemList/truncateCeraShopItemTable.inc"
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
#include "CItemList/insert_equiptment_item_to_db.inc"
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
#include "CItemList/insert_stackable_item_to_db.inc"
}

#include "CItemList/insertCeraShopItemIntoDB.inc"
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

#include "CItemList/CItemList.inc"
}

#include "CItemList/dtor_CItemList.inc"
}

#include "CItemList/destroy.inc"
}

#include "CItemList/find_item.inc"
}

#include "CItemList/find_item_1.inc"
}

#include "CItemList/FindGoods.inc"
}

#include "CItemList/GetProperBonusItem.inc"
}

#include "CItemList/getCeraShopBonusItem.inc"
}

#include "CItemList/get_lotto_item.inc"
}

#include "CItemList/create_item.inc"
}

// [b28d-20] insert_item（ORIG 0x0851129e，309/309，nd 43）：本轮修 P5 裸虚表
//   → `item->IsRandomOption()`（CItem vtable +0x4c 第 19 槽）与 P167
//   `operator==` 形；p167 调用点差集 45/45 **CLEAR**（0 missing/0 extra）。
//   残差 = 尾部 map 插入段的槽号/寄存器分配（BaseItemKey 临时 + inner map）⇒
//   语义完成、byte 层 N7/N82 封顶。
// [b28d-solo-CItemList] DIFF | 修：正向早返守卫（消 xor$1 物化；p167 CLEAR
//   45/45）；残 N7/N82 槽与寄存器分配。
#include "CItemList/insert_item.inc"
}

#include "CItemList/init.inc"
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
#include "CItemList/load_item_files.inc"
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
#include "CItemList/Load_Goods_File.inc"
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
#include "CItemList/Arad_GiftItem_Set.inc"
}

// ORIG T 0x081a2dea：先写 vptr（基类 Arad_ScriptLoader vtable），再逆序析构
// +0x1c map<string,int> 与 +0x04 map<long long,Data>（无虚析构，按值析构）。
#include "CItemList/dtor_Arad_GiftItem_Set.inc"
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
#include "CItemList/open.inc"
}

// ============================================================================
// Arad_GiftItem_Set::str_to_enum（ORIG 0x081a2e3e，_ZN17Arad_GiftItem_Set11str_to_enumEPKc）
// 语义：在 this+0x1c 的 map<string,int> 中按 name 查找，命中返回 second，否则 0。
// ============================================================================
#include "CItemList/str_to_enum.inc"
}

// ============================================================================
// Arad_GiftItem_Set::find（ORIG 0x081a311e，_ZN17Arad_GiftItem_Set4findEiii）
// 语义：按 64 位 key = ((long long)kind << 32) | (((long long)job << 16) + usableLevel)
// 查 this+0x04 的 map<long long, Data>，命中返回 &it->second，否则 0。
// ============================================================================
#include "CItemList/find.inc"
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
#include "CItemList/level_check.inc"
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

#include "CItemList/forceRule_ResultItem.inc"
}

#include "CItemList/OUTPUT_ITEM_STR.inc"
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
#include "CItemList/BindCube_ScriptData.inc"
#include "CItemList/dtor_BindCube_ScriptData.inc"
#include "CItemList/BindCube_LvBonusScriptData.inc"
#include "CItemList/dtor_BindCube_LvBonusScriptData.inc"
#include "CItemList/CCompound_ExtreamDun_Item.inc"
}

#include "CItemList/dtor_CCompound_ExtreamDun_Item.inc"
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
#include "CItemList/after_addItem.inc"
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
#include "CItemList/_AddItem.inc"
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
#include "CItemList/_isPossibleResultItem.inc"
}

#include "CItemList/check_N_addItem.inc"
}

#include "CItemList/ForceResultItemRule_Setting.inc"
}

#include "CItemList/_convertOutputItem.inc"
}
#include "CItemList/_convertInputItem.inc"
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
#include "CItemList/checkCompoundable.inc"
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
#include "CItemList/makeCompoundItem.inc"
}

#include "CItemList/_get_base_level.inc"
}

#include "CItemList/_convertInput_To_Output.inc"
}
#include "CItemList/_determine_rarity.inc"
}
// [b28d-solo-CItemList] AE（91/91，ndiff 0）| 本轮修：清零序解耦——声明序定槽
//   （lo 浅 -0x10 / hi 深 -0x14）、赋值语句序定 store 序（`hi=0; lo=0;`）复现
//   ORIG 先深后浅；4.4 对「声明+初始化」恒按声明序 store（旧 N93 lo/hi 槽序差
//   由此消除）。
std::pair<unsigned int, unsigned int>                              // ORIG T 0x832c9ca
#include "CItemList/_determine_level_bonus.inc"
}

// [b28d-solo-CItemList] AE（95/95，ndiff 0）| 本轮修：同 Eii 版清零序解耦
//   （声明 lo/hi + 赋值 `hi=0; lo=0;` 复现 ORIG 先 -0x18 后 -0x14）。
std::pair<unsigned int, unsigned int>                              // ORIG T 0x832cadc
#include "CItemList/_determine_level_bonus_1.inc"
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
#include "CItemList/_determine_level_bonus_index.inc"
}

#include "CItemList/_exception_Rare.inc"
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
#include "CItemList/_FindSelectItem.inc"
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
#include "CItemList/_FindRandomItem.inc"
}

#include "CItemList/_FindItemAll_BindSphere.inc"
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
#include "CItemList/_FindItemRelateCreationRate.inc"
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
#include "CItemList/LoadScript.inc"
}

}

#include "CItemList/GetRestrictCode.inc"
}

// ORIG W 0x0869476e _ZNK9CItemList21GetQuestItemDropBounsERKm：
// m_lottoCount(+0x5c, std::map<unsigned long,int>).find(key)；
// 命中返 second（mov 0x4 偏移），未命中返 0。
#include "CItemList/GetQuestItemDropBouns.inc"
}
