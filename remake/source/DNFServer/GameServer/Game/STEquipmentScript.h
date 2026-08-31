#ifndef GAME_STEQUIPMENTSCRIPT_H_
#define GAME_STEQUIPMENTSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— STEquipmentScript（G5 装备脚本表）
// 布局依据 docs/class_func_reports/STEquipmentScript/（Clear 0x0898e9e4 /
// ctor 0x0898db82 / dtor 0x0898e298）与 ImportEquipmentScript.md
// （0x08991aeb）逐成员推导；基类 STItemScript（0x178）。
// CItemList.h 中的简化声明由本头替换。
//
// 支撑类型（全库无权威属主，按 ORIG 同 TU 段反汇编实证后定义于此，
// 与 STMapScriptFinal 处理 CMap.h 支撑类型同模式）：
//   EquipmentAniInfoScript      ctor/clear T 0x0898d03e / 0x0898d142
//   STEquipmentAniScript        ctor/clear T 0x0898ca36 / 0x0898ce62
//   GrowthEquipmentInfo         ctor/clear W 0x089c2fc4 / 0x089c300c
//   Position                    ctor       W 0x089c2c42
//   stAvatarTypeSelect_t::init  T 0x0898d1fc
//   effect::STEffect            clear      W 0x089c2c64
// ============================================================================

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "EquipmentParameterInfo.h"
#include "STItemScript.h"

// ---- [war room point bonus]/[war room monster exp bonus] 载体（8 字节 POD）。
// 与 CEquipItem.h 的 WpBonusPointData/ExpBonusPointData 同布局（int type/int value，
// ORIG clear 置 type=2/value=0）；为避免 STEquipmentScript.h ↔ CEquipItem.h 循环
// 包含，此处以同布局本地类型承载，ABI 完全一致。
#ifndef DNF_STBONUSPOINTDATA_DEFINED
#define DNF_STBONUSPOINTDATA_DEFINED
struct stBonusPointData_t
{
    int m_type;    // +0x00
    int m_value;   // +0x04

    // ORIG WpBonusPointData::clear（0x8513a82）/ExpBonusPointData::clear 同型：
    // type=2、value=0。ORIG C1 0x0898d25e 构造子对象时置位，STEquipmentScript::Clear
    // 不清 +0x81c/+0x824，故必须随构造初始化。
    stBonusPointData_t() : m_type(2), m_value(0) {}
};
#endif


class STKeyCommand;
// ---- 动画层类型（ORIG setEquipmentAniScriptLay 第 8 参 N16equipmentAniType1TE；
//      0=normal 层、1=level 层、2=job 层） ----
namespace equipmentAniType
{
enum T
{
    T_normal = 0,
    T_level = 1,
    T_job = 2
};
}

#ifndef DNF_ITEMUPGRADE_TYPE_DEFINED
#define DNF_ITEMUPGRADE_TYPE_DEFINED
namespace ItemUpgradeType
{
enum T
{
    T_0 = 0
};
}
#endif

#ifndef DNF_STAVATARTYPESELECT_T_DEFINED
#define DNF_STAVATARTYPESELECT_T_DEFINED
// ORIG init 0x0898d1fc：5 个 int 清零 + for(i<=4) 清 this+4+(i+8)*2 的 ushort
// （即 m_socketType[5]@+0x14）；[emblem socket default] 解析按同一寻址写回。
struct stAvatarTypeSelect_t
{
    int m_field0;                     // +0x00
    int m_field4;                     // +0x04
    int m_field8;                     // +0x08
    int m_fieldc;                     // +0x0c
    int m_field10;                    // +0x10（后续 emblem 数量）
    unsigned short m_socketType[5];   // +0x14..+0x1d
    void init();                      // ORIG T 0x0898d1fc
};
#endif


#ifndef DNF_ENUM_EQUIPMENTTYPE_DEFINED
#define DNF_ENUM_EQUIPMENTTYPE_DEFINED
enum ENUM_EQUIPMENTTYPE
{
    ENUM_EQUIPMENTTYPE_0 = 0
};
#endif

#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif

// ---- [equipment/OldEquipmentStatInfoList.dat] 旧装备统计条目。
// ORIG 本 TU 段符号：C1/C2 T 0x0898d25e（调 clear）、clear T 0x0898d272；
// clear 实证 38 个 int 全清 → sizeof 0x98：
//   +0x00..+0x24 直接清零（hpMax..equipMagDef 10 字段）
//   循环 i<=3  清 +0x28+4i（元素耐性 4）
//   +0x38 直接（refAll）
//   循环 j<=16 清 +0x3c+4j（状态耐性 17）
//   +0x80/+0x84 直接（refAllState + 隐藏字段）
//   循环 k<=3  清 +0x88+4k（隐藏字段 4）
// 消费方（InitScriptProducers_3.cpp importOldEquipmentStatInfo）只写 +0x00..+0x80，
// 尾部 5 字段仅 clear 触达；命名沿用 sync_script.h 旧镜像（该镜像已删，见批报）。
#ifndef DNF_STEQUIPMENTSTATINFO_DEFINED
#define DNF_STEQUIPMENTSTATINFO_DEFINED
class STEquipmentStatInfo
{
public:
    STEquipmentStatInfo();
    void clear();

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
    int m_84;               // +0x84（仅 clear 清零，语义未建模）
    int m_88;               // +0x88
    int m_8c;               // +0x8c
    int m_90;               // +0x90
    int m_94;               // +0x94
};
#endif

// ---- [change status on change hp] 条目（ORIG push_back 20 字节视图：
//      int appendage + 4×float rate；loadAppendageType 取首字段） ----
struct STChangeStatusRate
{
    // 2026-08-31：ORIG 元素 sizeof 24（new_allocator 24B；ImportEquipmentScript
    // 0x899a3bc push_back 栈对象 24B）——int+4×float=20 后仍有 4B（未初始化
    // 尾字段，ORIG 原样未写）——补 pad 保持 vector 步长。
    int m_appendage;   // +0x00（loadAppendageType(token)）
    float m_rate1;     // +0x04
    float m_rate2;     // +0x08
    float m_rate3;     // +0x0c
    float m_rate4;     // +0x10
    char m_pad14[4];   // +0x14..0x18（ORIG 未写尾字段）
};

// ---- [character item check] 条目（vector stride 实证：+0x7cc→+0x7d8 = 0xc；
//      第三个 int 由 getEquipmentTypeToEnumValue 写入） ----
struct STEnchantSystemData
{
    int m_a;               // +0x00
    unsigned char m_b;     // +0x04（ORIG byte：clear movb 清 0、遍历 movzbl 读；sizeof 12 不变）
    int m_c;               // +0x08（getEquipmentTypeToEnumValue(token)）

    // ORIG 全 W 头内联：C1 0x854b194（调 clear）、clear 0x854b1a8
    // （+0/+4/+8 依序清零）、D1 0x89d17a2（空体）。
    STEnchantSystemData() { clear(); }

    void clear()
    {
        m_a = 0;
        m_b = 0;
        m_c = 0;
    }
    // 无用户析构：ORIG STEnchantSystemDataD1Ev 仅被 allocator::destroy 引用 1 次；
    // 派生 STEnchantSystemMapKey 空析构（13 调用点）不调基类析构 ⇒ 基类析构
    // 必须 trivial。用户 ~STEnchantSystemData() {} 会令 MapKeyD1Ev 多调一次 D1。
};

// ---- [setback img] 条目（ORIG ctor 0x0898f?? 三连 int 扫描） ----
struct stSetItemBackImageInfo
{
    // ORIG C1/C2 0x089c3054（W）：三个 int 清零。
    stSetItemBackImageInfo()
    {
        m_a = 0;
        m_b = 0;
        m_c = 0;
    }
    int m_a;   // +0x00
    int m_b;   // +0x04
    int m_c;   // +0x08
};

// ---- 动画脚本帧（ORIG setEquipmentAniScriptLay 0x089c1cb9 / clear 0x0898ce62
//      实证：int@0 层号、string@+0x04..+0x5c 共 23 个路径槽（snprintf 逐槽
//      path+def 拼接）、string@+0x60 主路径、vector<string>@+0x64/+0x70 追加列；
//      sizeof = 0x7c） ----
class STEquipmentAniScript
{
public:
    STEquipmentAniScript();
    void clear();

    int m_field0;            // +0x00（层号；非 0 视为有效待提交）
    std::string m_str04;     // +0x04
    std::string m_str08;     // +0x08
    std::string m_str0c;     // +0x0c
    std::string m_str10;     // +0x10
    std::string m_str14;     // +0x14
    std::string m_str18;     // +0x18
    std::string m_str1c;     // +0x1c
    std::string m_str20;     // +0x20
    std::string m_str24;     // +0x24
    std::string m_str28;     // +0x28
    std::string m_str2c;     // +0x2c
    std::string m_str30;     // +0x30
    std::string m_str34;     // +0x34
    std::string m_str38;     // +0x38
    std::string m_str3c;     // +0x3c
    std::string m_str40;     // +0x40
    std::string m_str44;     // +0x44
    std::string m_str48;     // +0x48
    std::string m_str4c;     // +0x4c
    std::string m_str50;     // +0x50
    std::string m_str54;     // +0x54
    std::string m_str58;     // +0x58
    std::string m_str5c;     // +0x5c
    std::string m_str60;     // +0x60
    std::vector<std::string> m_vec64;  // +0x64（追加动画列）
    std::vector<std::string> m_vec70;  // +0x70（追加动画列）
};

// ---- 成长装备条目（ctor W 0x089c2fc4 / clear W 0x089c300c，sizeof 0x18） ----
class GrowthEquipmentInfo
{
public:
    GrowthEquipmentInfo();
    void clear();

    int m_level;        // +0x00（clear=0；[level section ani] 写入层级号）
    int m_type;         // +0x04（clear=-1）
    int m_8;            // +0x08（0）
    int m_c;            // +0x0c（0）
    int m_10;           // +0x10（0）
    std::string m_str14;  // +0x14
};


class EquipmentAniInfoScript
{
public:
    EquipmentAniInfoScript();
    void clear();

    std::vector<STEquipmentAniScript> m_ani0;   // +0x00（普通层动画序列）
    int m_c;                                    // +0x0c（-1）
    int m_10;                                   // +0x10（-1）
    char m_expand14;                            // +0x14（[expand ani] 置 1）
    std::vector<STEquipmentAniScript> m_ani18;  // +0x18（job 层动画序列）
    int m_24;                                   // +0x24（-1）
    int m_28;                                   // +0x28（1）
    int m_2c;                                   // +0x2c（0）
    int m_30;                                   // +0x30（-1）
    int m_34;                                   // +0x34（-1）
    int m_38;                                   // +0x38（0）
    int m_3c;                                   // +0x3c（sub type）
    std::vector<std::string> m_wav40;           // +0x40（[custom wav]）
    std::string m_path4c;                       // +0x4c（[expand path]）
    std::vector<GrowthEquipmentInfo> m_growth50;               // +0x50
    std::vector<std::vector<STEquipmentAniScript> > m_level5c; // +0x5c（level 层动画组）
};

// ---- [particle] "same position" 三坐标（ctor W 0x089c2c42：三 int 清零） ----
class Position
{
public:
    Position();
    int m_x;   // +0x00
    int m_y;   // +0x04
    int m_z;   // +0x08
};

namespace effect
{
// ---- [effect] 条目（clear W 0x089c2c64，sizeof 0x18） ----
struct STEffect
{
    STEffect();
    void clear();

    char m_a;          // +0x00
    char m_b;          // +0x01
    char m_c;          // +0x02
    int m_4;           // +0x04
    int m_8;           // +0x08
    char m_c_;         // +0x0c
    int m_10;          // +0x10（clear 写 0x14）
    std::string m_str14;  // +0x14
};
}

class STEquipmentScript : public STItemScript
{
public:
    STEquipmentScript();
    ~STEquipmentScript();
    void Clear();

    EquipmentAniInfoScript& addAniInfo(ENUM_CHARACTERJOB job);
    EquipmentAniInfoScript* getAniInfo(ENUM_CHARACTERJOB job);
    bool isUpgradableLevel(ItemUpgradeType::T type, int level);
    bool isExistUpgradableLevel();

    struct artifact_info
    {
        artifact_info();
        bool isUsable() const;
        int m_minLevel;        // +0x00（[creature minimum level]）
        int m_physAttack;      // +0x04（[creature physical attack]）
        int m_magAttack;       // +0x08（[creature magical attack]）
        float m_mpRate;        // +0x0c（[creature skill consume mp rate]）
        float m_chargeRate;    // +0x10（[creature skill charge time rate]）
        float m_overChargeRate;  // +0x14（[creature skill over charge time rate]）
        float m_expRate;       // +0x18（[creature experience amount rate]）
        float m_physCrit;      // +0x1c（[creature physical critical hit]）
        float m_magCrit;       // +0x20（[creature magical critical hit]）
        float m_stuck;         // +0x24（[creature stuck]）
        int m_piercing;        // +0x28（[creature piercing]）
    };

    // ---- 成员布局（+0x178 起，基类 STItemScript 0x178） ----
    int m_178;                                    // +0x178（-1；收尾由 value/price 补全）
    int m_17c;                                    // +0x17c（0x46；[maximum level]）
    int m_180;                                    // +0x180（-1；[control index]）
    int m_184;                                    // +0x184（-1；[required skill]）
    int m_188;                                    // +0x188（-1；[icon]）
    std::vector<std::string> m_vec18c;            // +0x18c（[tarotcard view]/[extra icon] 字符串列）
    std::vector<int> m_vec198;                    // +0x198（对应数值列）
    std::string m_str1a4;                         // +0x1a4（[field animation]）
    std::vector<std::string> m_vec1a8;            // +0x1a8（[custom animation]）
    int m_field1b4;                               // +0x1b4（0x1a；[equipment type]→GetEquipmentType）
    int m_1b8;                                    // +0x1b8（[sub type]）
    int m_1bc;                                    // +0x1bc（[durability]）
    EquipmentParameterInfo m_param1c0;            // +0x1c0（主参数区）
    std::map<ENUM_CHARACTERJOB, EquipmentAniInfoScript> m_aniInfo640;  // +0x640
    std::vector<std::string> m_vec658;            // +0x658（[custom wav]）
    std::string m_str664;                         // +0x664（脚本全路径）
    std::string m_str668;                         // +0x668（[set name]）
    std::vector<int> m_vec66c;                    // +0x66c（[set item]）
    std::map<int, EquipmentParameterInfo> m_paramMap678;  // +0x678（套装分档参数）
    std::vector<EquipmentParameterInfo> m_paramVec690;    // +0x690（[level section ability]）
    char m_69c;                                   // +0x69c（level section ani 标志）
    int m_6a0;                                    // +0x6a0（-1；[set item master]）
    std::string m_str6a4;                         // +0x6a4（[basic explain]）
    std::string m_str6a8;                         // +0x6a8（[detail explain]）
    std::string m_str6ac;                         // +0x6ac（[attack info]）
    std::string m_str6b0;                         // +0x6b0（[fullset explain]）
    std::string m_str6b4;                         // +0x6b4（[fullset detail explain]）
    std::string m_str6b8;                         // +0x6b8（[flavor text]）
    int m_6bc;                                    // +0x6bc（[creature species]）
    std::vector<stAvatarTypeSelect_t> m_avatar6c0;  // +0x6c0（[emblem socket default]/[avatar type select]）
    std::vector<unsigned short> m_vec6cc;         // +0x6cc（[emblem socket default] 首组 emblem）
    int m_6d8;                                    // +0x6d8（-1；[avatar type select]）
    int m_6dc;                                    // +0x6dc（-1；[change type ultimateSkillCurScene]）
    int m_6e0;                                    // +0x6e0（[fixed select type]）
    int m_6e4;                                    // +0x6e4（-1；[emancipate]）
    std::vector<STChangeStatusRate> m_rate6e8;    // +0x6e8（[change status on change hp]）
    char m_6f4;                                   // +0x6f4（impossible: upgrade）
    char m_6f5;                                   // +0x6f5（impossible: amplify upgrade）
    char m_6f6;                                   // +0x6f6（impossible: disjoint）
    int m_6f8;                                    // +0x6f8（[usable period after unsealing]）
    std::map<ItemUpgradeType::T, std::pair<int, int> > m_upgrade6fc;  // +0x6fc（[limit upgradable level]）
    std::set<int> m_set714;                       // +0x714（Clear 插入 20；[usable module] 重填）
    char m_72c;                                   // +0x72c（[no random]）
    char m_72d;                                   // +0x72d（[weapon full avatar]）
    int m_730;                                    // +0x730（[hp recovery add rate]）
    int m_734;                                    // +0x734（[mp recovery add rate]）
    artifact_info m_artifact738;                  // +0x738..+0x763（creature 数据区）
    int m_764;                                    // +0x764（[output index]）
    std::vector<ENUM_EQUIPMENTTYPE> m_equipTypes768;  // +0x768（[hide equipment]）
    std::vector<int> m_vec774;                    // +0x774（[hide layer]）
    std::vector<int> m_vec780;                    // +0x780（? 同 hide 族 ScanInt 列表）
    char m_78c;                                   // +0x78c（[usable even peace]）
    int m_790;                                    // +0x790（[spectrum] RGBA 合成）
    int m_794;                                    // +0x794（[spectrum] 第 5 值）
    int m_798;                                    // +0x798（-1；[part set index]）
    int m_79c;                                    // +0x79c（-1；[effect part set index]）
    std::vector<int> m_vec7a0;                    // +0x7a0（[reference effect part set index]）
    char m_7ac;                                   // +0x7ac（[clear avatar]）
    std::vector<std::string> m_vec7b0;            // +0x7b0（[image packs]）
    char m_7bc;                                   // +0x7bc（[epic routing]）
    int m_7c0;                                    // +0x7c0（-1；[special ability]）
    int m_7c4;                                    // +0x7c4（-1；[chat emoticon index]）
    char m_7c8;                                   // +0x7c8（[required full set for chatting emoticon]）
    std::vector<STEnchantSystemData> m_enchant7cc;  // +0x7cc（[character item check]）
    std::vector<std::pair<int, int> > m_vec7d8;   // +0x7d8（avatar func filter: input）
    std::vector<std::pair<int, int> > m_vec7e4;   // +0x7e4（avatar func filter: output）
    std::string m_str7f0;                         // +0x7f0（[emancipate explain]）
    std::string m_str7f4;                         // +0x7f4（[particle] same position 名称）
    Position m_particle7f8;                       // +0x7f8（same position 三坐标）
    STKeyCommand* m_keyCommand;                   // +0x804（[command] new STKeyCommand）
    int m_808;                                    // +0x808（3；processModuleTagSet 模块号）
    unsigned short m_80c;                         // +0x80c（[character name y revision] 后值）
    unsigned short m_80e;                         // +0x80e（同上 先值）
    int m_810;                                    // +0x810（[quest item drop rate bonus]）
    char m_814;                                   // +0x814（2；[Force Result Item Rule]）
    int m_818;                                    // +0x818（Force Result Item Rule 第二值）
    stBonusPointData_t m_wpBonus81c;              // +0x81c（[war room point bonus]）
    stBonusPointData_t m_expBonus824;             // +0x824（[war room monster exp bonus]）
    char m_82c;                                   // +0x82c（[Uncompoundable]）
    std::vector<int> m_vec830;                    // +0x830（[remove avatar] getModuleType 列）
    std::string m_str83c;                         // +0x83c（[setback img] 名称）
    std::vector<stSetItemBackImageInfo> m_setItem840;  // +0x840
    std::vector<effect::STEffect> m_effect84c;    // +0x84c（[effect]）
    std::vector<std::string> m_vec858;            // +0x858（[aurora graphic effects] 名称列）
    std::vector<bool> m_vec864;                   // +0x864（对应开关列）
    std::string m_str878;                         // +0x878（[msg balloon on mucu skill]）
    int m_87c;                                    // +0x87c
    char m_880;                                   // +0x880（-1）
    int m_884;                                    // +0x884
    char m_888;                                   // +0x888（[random option]）
    char m_889;                                   // +0x889（1；random option 参数）
    char m_88a;                                   // +0x88a（[enable dye] 第一开关）
    char m_88b;                                   // +0x88b（第二开关）
    int m_88c;                                    // +0x88c
    int m_890;                                    // +0x890（[not amplify]）
    // ORIG B 0x94f7c20 [0x2ee0]：pricetable.tbl 30×100 int 静态表
    // （InitEquipmentScript 0x08991601 清零回填；定义 InitScriptProducers_4.cpp）。
    static int priceTable_[30][100];
};

bool ImportEquipmentScript(STEquipmentScript* script, const char* path,
                           bool flag, int a, int idx);
bool EquipmentScriptSpecialMonsterDrop(STEquipmentScript* script,
                                       std::string str, const char* path);
bool importArtifactInfo(STEquipmentScript::artifact_info& info,
                        std::string str);

#endif  // GAME_STEQUIPMENTSCRIPT_H_
