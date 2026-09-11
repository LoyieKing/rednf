// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// [b28d-tu-CItemList 2026-09-11 终轮复核] 11 件可攻件 fresh 复测：六项新旋钮实测
//   全部零变化或反劣并已回滚（本 TU 源码净改动 0；四数 694/1358/6/14 不变）：
//   · Load_Goods_File（0x085123da，386/387，nd 3）：ORIG 0x08512854
//     `lea 0x14(%eax),%edx` vs 4.4 `add $0x14,%eax; mov %eax,%edx`（末求值实参的
//     地址就地加）。探针 /tmp/probe_type.cpp（同一 4.4.6 路由后端）实证：字段为
//     enum 或 unsigned long、`(T&)x` 或 `*(T*)&x` 四形**恒发 add** ⇒ 与字段类型
//     及写法无关（改 CItemList.h 的 m_field14 类型亦无效），N4/P30 结构性伪影。
//   · _determine_level_bonus_index（0x0832cc02，65/64）：ORIG `push %ebx` +
//     循环体首 `mov -0xc(%ebp),%ebx` 跨 call 保活；`int idx` 实测 66 条（反劣）、
//     `register` 形前轮已证免槽（帧 0x28→0x14）⇒ N7/N82（P290 反面已用尽）。
//   · _AddItem（0x0832dd1c，78/79）/after_addItem（0x0832de18，101/103）/
//     _FindSelectItem（0x0832d15e，109/111）：`m_bindItems[i][j]` 索引乘法链
//     ORIG `mov -0x14,%edx; mov -0x10,%ecx`+`lea ...,%ecx` vs 4.4
//     `mov -0x14,%ecx; mov -0x10,%eax`+`lea ...,%eax; mov %eax,%edx`（多 1 mov，
//     即 gap 来源）；`unsigned int j` 实测零变化 ⇒ N86b（P291/P311 二维视图已是
//     正形，P231 纯 register 不适用于有槽件）。
//   · makeCompoundItem（0x0832ccc0，249/249）：ORIG `mov -0x24(%ebp),%eax;
//     test %eax,%eax` vs 4.4 `cmpl $0x0,-0x24(%ebp)`（N18a）+ ORIG `mov;add mem`
//     vs 4.4 `mov;mov;lea`；`inRarity > 0` 写法零变化、`register int inRarity`
//     槽整体错位（-0x24→-0x28 全移，反劣）⇒ N18a/N7，长度虽相等但助记符序列
//     不可齐（DIFF 保持）。
//   · insert_item（0x0851129e，309/310）：4.4 把两分支共用的 `&value`(-0xe8) 的
//     lea 提前到 call 前并缓存 %ebx（ORIG 0x08511598/0x08511614 两处均在用点
//     `lea -0xe8(%ebp),%edx`）；`(BaseItemValue&)value` 引用 cast 同树零变化、
//     末尾 goto 改 if/else 极性形零变化 ⇒ N95（4.4 栈槽地址 hoisting）+ N104 块布局。
//   · insert_*_to_db（0x08620968 等）：disp 多重集逐点对比仅差 ORIG 独有的
//     `0x400(%eax)`/`0xc00(%eax)`（`lea K(%eax)` 拆分，N4）；槽差是 4.1 浅区碎片
//     位置不同（我方 string-home -0x58 vs ORIG -0x5c，out1/out2 =-0x60/-0x64 两侧
//     一致）⇒ N82/N95，非声明序可翻（加死局部只会破坏已对齐的 out1/out2）。
//   · 语义轴附带核对：本 TU 135 条字符串字面量**全部逐字节存在于 ORIG**（含 SQL/
//     日志/PRETTY_FUNCTION 串）；EquipParamInfo/SITEMTYPE/SWEAPONTYPE/SARMORTYPE
//     与 CSyncScript 成员偏移 = 注释 = ORIG disp（无 pack 缺失）⇒ 父项 CLEAR。

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

#include "CItemList/GetSocketTypeCharacter.inc"


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
    const OUTPUT_ITEM_STR& a, const OUTPUT_ITEM_STR& b)
{
    if (a.m_usableLevel == b.m_usableLevel)
    {
        return a.m_index < b.m_index;
    }
    return a.m_usableLevel < b.m_usableLevel;
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


#include "CItemList/CSyncScript.inc"


#include "CItemList/dtor_CSyncScript.inc"


#include "CItemList/truncate_dnf_item_info_table.inc"


#include "CItemList/truncate_artifact_info_table.inc"


#include "CItemList/truncateCeraShopItemTable.inc"


#include "CItemList/insert_equiptment_item_to_db.inc"


#include "CItemList/insert_stackable_item_to_db.inc"


#include "CItemList/insertCeraShopItemIntoDB.inc"

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


#include "CItemList/dtor_CItemList.inc"


#include "CItemList/destroy.inc"


#include "CItemList/find_item.inc"


#include "CItemList/find_item_1.inc"


#include "CItemList/FindGoods.inc"


#include "CItemList/GetProperBonusItem.inc"


#include "CItemList/getCeraShopBonusItem.inc"


#include "CItemList/get_lotto_item.inc"


#include "CItemList/create_item.inc"


#include "CItemList/insert_item.inc"


#include "CItemList/init.inc"


#include "CItemList/load_item_files.inc"


#include "CItemList/Load_Goods_File.inc"


#include "CItemList/Arad_GiftItem_Set.inc"


#include "CItemList/dtor_Arad_GiftItem_Set.inc"


#include "CItemList/open.inc"


#include "CItemList/str_to_enum.inc"


#include "CItemList/find.inc"


#include "CItemList/level_check.inc"


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


#include "CItemList/OUTPUT_ITEM_STR.inc"


bool OUTPUT_ITEM_STR::operator<(const OUTPUT_ITEM_STR& other) const
{
    if (m_usableLevel == other.m_usableLevel)
    {
        return m_index < other.m_index;
    }
    return m_usableLevel < other.m_usableLevel;
}

#include "CItemList/BindCube_ScriptData.inc"
#include "CItemList/dtor_BindCube_ScriptData.inc"
#include "CItemList/BindCube_LvBonusScriptData.inc"
#include "CItemList/dtor_BindCube_LvBonusScriptData.inc"

#include "CItemList/CCompound_ExtreamDun_Item.inc"


#include "CItemList/dtor_CCompound_ExtreamDun_Item.inc"


#include "CItemList/after_addItem.inc"


#include "CItemList/_AddItem.inc"


#include "CItemList/_isPossibleResultItem.inc"


#include "CItemList/check_N_addItem.inc"


#include "CItemList/ForceResultItemRule_Setting.inc"


#include "CItemList/_convertOutputItem.inc"

#include "CItemList/_convertInputItem.inc"


#include "CItemList/checkCompoundable.inc"


#include "CItemList/makeCompoundItem.inc"


#include "CItemList/_get_base_level.inc"


#include "CItemList/_convertInput_To_Output.inc"

#include "CItemList/_determine_rarity.inc"

// [b28d-solo-CItemList] AE（91/91，ndiff 0）| 本轮修：清零序解耦——声明序定槽
//   （lo 浅 -0x10 / hi 深 -0x14）、赋值语句序定 store 序（`hi=0; lo=0;`）复现
//   ORIG 先深后浅；4.4 对「声明+初始化」恒按声明序 store（旧 N93 lo/hi 槽序差
//   由此消除）。
std::pair<unsigned int, unsigned int>                              // ORIG T 0x832c9ca
#include "CItemList/_determine_level_bonus.inc"


// [b28d-solo-CItemList] AE（95/95，ndiff 0）| 本轮修：同 Eii 版清零序解耦
//   （声明 lo/hi + 赋值 `hi=0; lo=0;` 复现 ORIG 先 -0x18 后 -0x14）。
std::pair<unsigned int, unsigned int>                              // ORIG T 0x832cadc
#include "CItemList/_determine_level_bonus_1.inc"


#include "CItemList/_determine_level_bonus_index.inc"


#include "CItemList/_exception_Rare.inc"


#include "CItemList/_FindSelectItem.inc"
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

#include "CItemList/_FindRandomItem.inc"
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

#include "CItemList/_FindItemAll_BindSphere.inc"


#include "CItemList/_FindItemRelateCreationRate.inc"



#include "CItemList/LoadScript.inc"


}

#include "CItemList/GetRestrictCode.inc"


#include "CItemList/GetQuestItemDropBouns.inc"

