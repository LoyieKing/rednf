#ifndef GAME_CITEMAMPLIFIER_H_
#define GAME_CITEMAMPLIFIER_H_

// ============================================================================
// df_game_r 还原 —— CItemAmplifier（装备强化）+ stAmplifyOption_t。
// 布局依据 ORIG 构造（0x82aa65c）逐成员推导：
//   +0x00  stAmplifyItemScript_t（0x100，含 _generateOption 用的概率/选项表）
//   +0x100 upgrade_table_t（0x4e4，含 checkInvestableItem 用的 grade 表）
// 尺寸合计 0x5e4（GlobalData::Init 的 new 尺寸）。
// stAmplifyOption_t 为 Inven_Item::m_amp（+0x11）的类型，尺寸 0xe。
// ============================================================================

#include <ext/hash_map>
#include <map>
#include <vector>
#include <cstring>

class CItem;

#include "EquipmentParameterInfo.h"   // STResultItemCnt（upgrade_table_t +0x490 vector 元素）
#include "STEquipmentScript.h"    // ENUM_EQUIPMENTTYPE / ItemUpgradeType::T（ODR 守卫已由 FixEquipmentTypeGuard 统一）
#include "ST3rdChronicleDestructionResult.h"   // upgrade_table_t +0x434 成员（ORIG C1 0x089000ca 实证）
#include <unordered_map>   // upgrade_table_t +0x4c4（ORIG _ZNSt13unordered_map…C1EjRKS3_RKS5_RKS9_）

class Inven_Item;

// ---- 稀有度/装备类型枚举（ORIG mangled 名称所需；与 DNFShared/STEquipmentScript 取值一致）----
#ifndef DNF_ENUM_RARITY_DEFINED
#define DNF_ENUM_RARITY_DEFINED
enum ENUM_RARITY
{
    RARITY_COMMON = 0,
    RARITY_UNCOMMON = 1,
    RARITY_RARE = 2,
    RARITY_UNIQUE = 3,
    RARITY_EPIC = 4,
    RARITY_CHRONICLE = 5,
    RARITY_MAX = 6,
};
#endif
// ENUM_EQUIPMENTTYPE 归口 STEquipmentScript.h（CItem.h → STItemScript.h 链路可见）。

// ---- 强化选项（Inven_Item::m_amp @ +0x11；尺寸 0xe）----
#pragma pack(push, 1)
// amp 内嵌 guid 视图（恰 0xa）：ORIG makeInvenItem 0x833f418
// `mov %ecx,0x15(%eax)/0x19(%eax)`、`mov %dx,0x1d(%eax)` = item+0x15/0x19/0x1d
// = amp+0x04/0x08/0x0c（guid m_serverId/m_datetime/m_sequenceNo，SetXxx 实证）。
// 不引用全局 itemGloballyUniqueIdentifier_t（其 m_tailPad[7] 使 sizeof=0x11 ≠
// 0xa，撑爆 Inven_Item 链）。命名类型定义于 union 外（匿名 union 禁嵌套新类型）。
struct stAmplifyGuidView
{
    int m_serverId;                  // +0x00
    int m_datetime;                  // +0x04
    unsigned short m_sequenceNo;     // +0x08
};
static_assert(sizeof(stAmplifyGuidView) == 0xa, "amp guid view size drift");
// [b28d-solo-CCreatureMgr P247] +0x00 四字节头部视图：结构赋值形（4.4 对 4 字节
// POD 结构赋值发 LHS-first 双基址 `mov dst; mov src; mov (src),%edx; mov %edx,(dst)`，
// 与 ORIG setCopy 0x833f409/0x833f412、makeInvenItem 0x833f4a0 段逐条一致；
// 标量形恒 RHS-first）。sizeof 4、布局/mangled 零影响。
struct stAmplifyHeadView
{
    int m_value;
};
static_assert(sizeof(stAmplifyHeadView) == 4, "amp head view size drift");
class stAmplifyOption_t
{
public:
    void reset();
    bool isIdentified() const;
    bool hasAbility() const;
    bool identify();
    unsigned char getAbilityType() const;
    unsigned short getAbilityValue() const;
    void _setAbility(unsigned char type, unsigned short value, bool identified);
    void assign(unsigned char type, unsigned short value);
    void SetLock(unsigned char lock);
    unsigned char GetLock() const;
    void getValues(unsigned char& type, unsigned short& value) const;

    // [b28d-23 P235] 头部 4 字节改匿名 union 双视角：m_amp0Int 供
    // Inven_Item_Compatible::setCopy/makeInvenItem 以具名 int 成员直访
    // （ORIG 0x833f38e `mov 0x11(%edx),%edx` / 0x833f418 `mov %edx,0x11(%eax)`
    // 单条 disp；取址 reinterpret 形恒多 add $0x11）。布局/sizeof 0xe 不变。
    union {
        struct {
            unsigned char m_abilityType;     // +0x00（bit7 = identified）
            unsigned short m_abilityValue;   // +0x01
            unsigned char m_lock;            // +0x03
        };
        int m_amp0Int;                       // +0x00 四字节视角
        stAmplifyHeadView m_amp0Head;        // +0x00（结构赋值形视角，P247）
    };
    union {
        char m_pad4[0xa];            // +0x04（尺寸 0xe，历史视图保留——AraC9
                                     //   cUserHistoryLog 等消费点不动）
        stAmplifyGuidView m_guid;    // +0x04（恰 0xa，Main 批准的具名字段化）
    };
};
#pragma pack(pop)

// ---- 强化选项信息（stAmplifyItemScript_t::m_options 元素）----
struct stAmplifyOptionInfo_t
{
    int m_type;               // +0x00
    unsigned short m_prob;    // +0x04
    unsigned short m_value;   // +0x06
};

enum ENUM_AMPLIFY_TYPE
{
    ENUM_AMPLIFY_TYPE_NONE = 0
};

// ---- 强化投资类型（getInvestMaterialInfo/enableInvestAmplifyOption 的 enum）----
// 取值由 ORIG 反汇编约束：dispatch_sig 读入的 byte 校验 <<=0x4；enable 分支
// cmp 0 / 1 / 2；getInvestMaterialInfo 用 e = enum*0x14 索引 invest 表。
enum ENUM_ITEM_AMPLIFY_INVEST_TYPE
{
    ENUM_ITEM_AMPLIFY_INVEST_NONE  = 0,
    ENUM_ITEM_AMPLIFY_INVEST_TYPE_1 = 1,
    ENUM_ITEM_AMPLIFY_INVEST_TYPE_2 = 2,
};

// ---- 强化投资选项（stAmplifyItemScript_t::m_investMap 的 value；尺寸 8）----
// 布局依据 ORIG ctor（0x0822aea6）：+0x00 m_amount / +0x04 m_value，均清 0。
struct stInvestAmplifyOption_t
{
    int m_amount;   // +0x00
    int m_value;    // +0x04

    stInvestAmplifyOption_t();
};

// ---- 强化脚本表（CItemAmplifier +0x00，0x100）----
class stAmplifyItemScript_t
{
public:
    stAmplifyItemScript_t();

    std::map<std::string, ENUM_AMPLIFY_TYPE> m_amplifyTypeMap;    // +0x00（ORIG ctor 0x82aa510 构造）
    std::vector<stAmplifyOptionInfo_t> m_options; // +0x18
    float m_rarityRate[6];                        // +0x24
    int m_field3c;                                // +0x3c
    float m_weightByLevel;                        // +0x40（[amplification weight by level]）
    char m_pad44[0x6c - 0x44];
    float m_bonusWeightByEquipType[12];           // +0x6c..0x97（[bonus weight by equipment
                                                  //      type]，equip type 10..21）
    // ORIG getter 以类内 int 数组寻址（arr[i] ⇒ idx+0x24 + disp8 形），
    // 故按行各声明为 int[2]：item 行 [0]=[purify material] item、[1]=only。
    // [b28d-34] unsigned：ORIG getPurifyMaterialType 0x08234d20 比较段两段物化
    // （P274/P211 字段侧旋钮），探针 /tmp/p34e.cpp f4 逐条命中；布局不变。
    unsigned int m_purifyMatItem[2];              // +0x9c..0xa0
    int m_purifyMatCount[2];                      // +0xa4..0xa8（名称序「推断」，偏移实证）
    // 强化投资表（ORIG ctor 0x082aa510 依次构造 3 个；尺寸 0x14 各）
    __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>
        m_investMap[3];                           // +0xac,+0xc0,+0xd4
    int m_rarityProb[6];                          // +0xe8
};

// ---- 强化升级表（CItemAmplifier +0x100，0x4e4）----
// 布局依据 ORIG C1 0x089000ca 反汇编：+0x000 vector<upgrade_info_t>，
// +0x00c..+0x49b 概率/等级混合表（float/int 交叠访问，ctor 按偏移初始化），
// +0x49c map<int,float>，+0x4b4 四个标量，+0x4c4 std::unordered_map<int,
// MachineFrameInfo>（ORIG C1 0x089000ca 实证：构造传桶数 0xa）。
// ---- upgrade_info_t（强化信息行，尺寸 0x6c）----
// 权威尺寸来自 ORIG vector<upgrade_info_t>::operator[]（0x891e08e）步长 `imul $0x6c`，
// 及 Event_Upgrade_Jar::SettingRecipeNUpConunt / updateUpgradeTableFromEventUpgradeJarScript
// 的 +0x2c/+0x30/+0x34 字段访问。前 0x2c 字节为概率/等级混合区（upgrade_table_t ctor
// 与 get_upgrade_info 按 float 偏移访问，属内部镜像，不具名）。
struct upgrade_info_t
{
    // ORIG C1/C2 @0x0854b282（W，69 insn）写域逐段实证（二进制切片人工转写；
    // docs/class_func_reports 的 Ghidra C 段撞名，见 §6.3）：
    //   ① +0x00..+0x1c 八字（每字经 %edx 中转 movl）；
    //   ② +0x20..+0x34 六字直 movl（0x0854b2e2..0x0854b314）；
    //   ③ 循环 i=0..4：`add $0xc,%edx; movl $0,0x8(%eax,%edx,4)`
    //      ⇒ this + 4i + 0x38 ⇒ +0x38..+0x48 五字（0x0854b324）；
    //   ④ 同循环两 store：`lea 0x10(%edx)` ⇒ 4j+0x4c、`lea 0x14(%edx)`
    //      ⇒ 4j+0x5c ⇒ 各四字（0x0854b34d/0x0854b35f）。
    // 写域合计 +0x00..+0x6b ⇒ sizeof 恰 0x6c（与 ProcUpgrade 栈槽
    // -0x284→-0x215 跨度一致），故为**分段具名字段**而非扩容。
    upgrade_info_t()
    {
        // ①（逐条写：4.1 展开发形；写成循环会被 4.4 保留循环而丢 8×2 展开）
        // [b28d-solo-CItemUpgrade] 前 8 字 ORIG 0x0854b288..0x0854b2dc 为
        // `mov $0x0,%edx; mov %edx,N(%eax)` 两段（P238 常量 store edx 中转）⇒
        // 源字段必须是 float 视角；int 视角 m_prob00[k]=0 4.4 恒折单条 movl $0。
        m_f00[0] = 0.0f; m_f00[1] = 0.0f; m_f00[2] = 0.0f; m_f00[3] = 0.0f;
        m_f00[4] = 0.0f; m_f00[5] = 0.0f; m_f00[6] = 0.0f; m_f00[7] = 0.0f;
        // ②
        m_prob20[0] = 0;
        m_prob20[1] = 0;
        m_prob20[2] = 0;
        m_recipeItem = 0;
        m_meterialCount = 0;
        m_discountBase = 0;
        // ③
        for (int i = 0; i <= 4; ++i)
        {
            m_w38[i] = 0;
        }
        // ④（ORIG 两个四字数组共用一个循环计数器）
        // [b28d-solo-CItemUpgrade] ORIG 0x0854b34d/0x0854b35f 两 store 均带
        // `mov $0x0,%edx; mov %edx,0xc(%eax,%ecx,4)` 中转（P238）⇒ 源字段用
        // float 视角；int 视角 m_w4c[j]=0 4.4 恒折单条 movl $0。
        for (int j = 0; j <= 3; ++j)
        {
            m_f4c[j] = 0.0f;
            m_f5c[j] = 0.0f;
        }
    }
    // [b28d-solo-CItemAmplifier] +0x00..+0x1f 与 +0x20..+0x2b 加 float 视角：
    // get_upgrade_info 0x089004ed.. 段 ORIG 为 `mov 0xc(%ebp),%eax; flds
    // 0x4(%eax)` 的**常量 disp 折叠**形；`((float*)out)[i]` 写法 4.4 恒
    // `lea/add + flds (%reg)`（每点多 1-2 条）。匿名 union 双视角零布局改动
    // （sizeof/偏移/静态断言不变），消费方仍用 int 名。
    union {
        int m_prob00[8];    // +0x00..+0x1f 概率/等级混合区
        float m_f00[8];     // +0x00 同区 float 视角
    };
    union {
        int m_prob20[3];    // +0x20..+0x2b 同区尾字
        float m_f20[3];     // +0x20 同区 float 视角
    };
    int m_recipeItem;       // +0x2c（材料 item idx / 强化商品）
    int m_meterialCount;    // +0x30（材料数量）
    int m_discountBase;     // +0x34（折扣基数；updateUpgradeTable... 亦写 +0x2c/+0x30）
    int m_w38[5];           // +0x38..+0x48（ORIG C1 循环 ③ 清五字）
    union {
        int m_w4c[4];       // +0x4c..+0x58（循环 ④ 组一）
        float m_f4c[4];     // +0x4c 同区 float 视角（get_upgrade_info 0x13..0x16）
    };
    union {
        int m_w5c[4];       // +0x5c..+0x68（循环 ④ 组二）
        float m_f5c[4];     // +0x5c 同区 float 视角（get_upgrade_info 0x17..0x1a）
    };
};
static_assert(sizeof(upgrade_info_t) == 0x6c, "upgrade_info_t size drift");
static_assert(offsetof(upgrade_info_t, m_recipeItem) == 0x2c, "info +0x2c");
static_assert(offsetof(upgrade_info_t, m_w4c) == 0x4c, "info +0x4c");
static_assert(offsetof(upgrade_info_t, m_w5c) == 0x5c, "info +0x5c");

class upgrade_table_t
{
public:
    // MachineFrameInfo（[upgrade machine frame rate] 行；importUpgradeTable
    // 0x08900c26 实证：4×ScanFloat + ScanInt，sizeof 0x14）。
    struct MachineFrameInfo
    {
        // ORIG T ctor 0x089003b4：4×float 1.0f + int 2。定义 OnlyOrigBatch4_Force.cpp。
        MachineFrameInfo();
        float m_f00;   // +0x00
        float m_f04;   // +0x04
        float m_f08;   // +0x08
        float m_f0c;   // +0x0c
        int m_i10;     // +0x10
    };

    upgrade_table_t();                                // ORIG 0x089000ca

    std::vector<upgrade_info_t> m_infos;              // +0x000
    // +0x00c 混合表（至 0x433）。ORIG 对该区既有「按脚本行/列号计算偏移」的
    // 通用访问（intAt/floatAt），也有编译期常量偏移的直接成员访存：
    //   ProcUpgrade @0x0854779c 0x08548ff2 `mov 0x234(%eax),%eax`（-O0 单条
    //   disp，非 helper call）—— 与 CItemUpgrade.cpp:188 的
    //   `newUpgrade >= table->…(0x234)` 同读点 ⇒ +0x234 建具名成员。
    // m_body 与视图并列（union 零改布局，sizeof/后续成员偏移不变；C++98 匿名
    // union 不得嵌套类型声明 ⇒ 用具名 union 成员 m_bodyView）。
    union UpgradeBodyView
    {
        char m_body[0x404 - 0x00c];   // 0x404 起切出为类成员 m_penalty（见下）
        // dword 下标视图（P30 关键形）：ORIG 对「基址 + 含常数下标」的表访问发
        //   `mov idx,%edx ; add $K,%edx ; movl $imm,(%eax,%edx,4)`
        // 两步（upgrade_table_t::C1 @0x089000ca 12 个初始化循环、importUpgradeTable
        // 写点、get_upgrade_info 16 行缩放全为该形）；用字节偏移表达式
        // （m_body + off）会被 4.4 折成 lea/shl/add 链（N81）。数组长度 0x428/4。
        float m_f32[(0x404 - 0x00c) / 4];
        int m_i32[(0x404 - 0x00c) / 4];
        struct UpgradeTableNamed
        {
            char m_pad00c[0x234 - 0x00c];   // +0x00c..+0x233（脚本行区，按
                                            //   intAt/floatAt 计算偏移访问）
            int m_noticeUpgradeLevel;       // +0x234 强化成功公告阈值
            // +0x238..+0x258 附加分解物四元组（ORIG _getAdditionalDisjointItem
            // @0x08546938 逐 disp 实证：0x085469b6 `mov 0x238`、0x08546993
            // `mov 0x23c`、0x085469d5 `flds (%eax,%edx,4)` 其中
            // edx=rarity+0x90 ⇒ +0x240+4·rarity、0x085469a4/0x085469de
            // `mov 0x258`）。写点 = importUpgradeTable 的
            // CItemAmplifier.cpp:51/52/55/58（权重区 i=0..5 ⇒ 6 项，0x258 独立）。
            int m_additionalItemId;         // +0x238 附加分解物 item idx
            int m_additionalGradeGap;       // +0x23c 触发所需的成长等级差下限
            float m_additionalWeight[6];    // +0x240..+0x257 按 rarity 的产量权重
            int m_additionalMinUpgrade;     // +0x258 触发所需的强化等级下限
            int m_baseGrade[6];             // +0x25c..+0x273 各稀有度基准成长
                                            //   等级。证据：checkInvestableItem
                                            //   @0x08234980 直读 `mov 0x25c(,%eax,4)`
                                            //   （inline 访问器在 -O0 会额外发
                                            //   call，故建真实成员）；写点
                                            //   importUpgradeTable [upgrade base
                                            //   grade] 行 CItemAmplifier.cpp:56
                                            //   `intAt((i+0x94)*4+0xc)` i=0..5。
            char m_pad274[0x404 - 0x274];   // +0x274..+0x403（其余脚本行区）
        } m_named;
    } m_bodyView;
    // GetPenaltyType 表：ORIG `add $0x100,%edx; mov 0x4(%eax,%edx,4)` 两步形
    // 需要**非 union** 的基 0x404 int 数组（union 成员访问 gcc 必拆 0xfc/0x14，
    // inv8~inv13 实证；inv7/inv9 普通成员发 0x100/4 精确复现）。
    // 原 union 0x404..0x433 区切出为独立成员，总尺寸/偏移不变（union 0x3f8 +
    // m_penalty 0x30 = 0x428）。UPG_TBL_I32 中 k>=0x100 的下标寻址不受影响。
    int m_penalty[12];                    // +0x404..+0x433 按 rarity 的惩罚表
    ST3rdChronicleDestructionResult m_destruction;    // +0x434（ORIG C1 0x089000ca 直构；
                                                      //   内部 m_vec @+0x490 = 旧 m_resultItems）
    std::map<int, float> m_floatMap;                  // +0x49c（[cost weight by upgrade level]）
    int m_zero4b4[4];                                 // +0x4b4..+0x4c0（machine frame base time ×4）
    std::unordered_map<int, MachineFrameInfo> m_machineFrames;  // +0x4c4
                                                       // （sizeof 0x20，与 ORIG 一致）

    //（+0x00c 混合表的按脚本行/列偏移访问已改为文件级宏 UPG_TBL_*，见下方：
    //  -O0 下成员函数访问器每个使用点额外发 1 条 call，而 ORIG 全部是
    //  `mov/flds off(%eax,%edx,4)` 帧内直访 —— batch-14 访问器专项。）

    // ORIG 0x08900436：按 rarity/growthGrade/upgrade/equipType 计算 upgrade_info 到 out。
    // 返回 0 表示越界/失败，1 表示成功。
    // [b28d-Main·检查点14 P292] int→bool：_GetUpgradeInfo 尾直返本函数结果
    //   （ORIG 0x08546816 `call; add; pop; ret` 无归一化）⇒ 本函数亦须 bool，
    //   否则 bool 化后的 _GetUpgradeInfo 会多出 setne 归一化。消费点仅
    //   CItemUpgrade.cpp:324（return 直传）。
    bool get_upgrade_info(upgrade_info_t* out, unsigned char rarity,
                          unsigned char growthGrade, unsigned char upgrade,
                          ENUM_EQUIPMENTTYPE equipType) const;
    // ORIG 0x089003ee：判断惩罚类型；返回 3 或 upgrade_info.m_discountBase 编码值。
    int GetPenaltyType(unsigned char upgrade, ENUM_RARITY rarity,
                       const upgrade_info_t* info) const;
};

static_assert(sizeof(upgrade_table_t::UpgradeBodyView) == 0x404 - 0x00c,
              "upgrade_table_t::UpgradeBodyView ABI drift");
static_assert(sizeof(upgrade_table_t::MachineFrameInfo) == 0x14,
              "upgrade_table_t::MachineFrameInfo ABI drift");
static_assert(
    sizeof(upgrade_table_t::UpgradeBodyView::UpgradeTableNamed) ==
        0x404 - 0x00c,
    "upgrade_table_t::UpgradeBodyView::UpgradeTableNamed ABI drift");
static_assert(offsetof(upgrade_table_t::UpgradeBodyView::UpgradeTableNamed,
                       m_noticeUpgradeLevel) == 0x234 - 0x00c,
              "upgrade_table_t +0x234 view offset drift");
static_assert(offsetof(upgrade_table_t::UpgradeBodyView::UpgradeTableNamed,
                       m_baseGrade) == 0x25c - 0x00c,
              "upgrade_table_t +0x25c view offset drift");
static_assert(offsetof(upgrade_table_t::UpgradeBodyView::UpgradeTableNamed,
                       m_additionalItemId) == 0x238 - 0x00c,
              "upgrade_table_t +0x238 view offset drift");
static_assert(offsetof(upgrade_table_t::UpgradeBodyView::UpgradeTableNamed,
                       m_additionalGradeGap) == 0x23c - 0x00c,
              "upgrade_table_t +0x23c view offset drift");
static_assert(offsetof(upgrade_table_t::UpgradeBodyView::UpgradeTableNamed,
                       m_additionalWeight) == 0x240 - 0x00c,
              "upgrade_table_t +0x240 view offset drift");
static_assert(offsetof(upgrade_table_t::UpgradeBodyView::UpgradeTableNamed,
                       m_additionalMinUpgrade) == 0x258 - 0x00c,
              "upgrade_table_t +0x258 view offset drift");
static_assert(offsetof(upgrade_table_t, m_penalty) == 0x404,
              "upgrade_table_t m_penalty offset drift");
static_assert(offsetof(upgrade_table_t, m_destruction) == 0x434,
              "upgrade_table_t m_destruction offset drift");

// ---------------------------------------------------------------------------
// +0x00c 混合表的「脚本行/列号算偏移」访问宏（off = 对象内绝对偏移）。
//
// 为什么是宏而不是成员函数（batch-14 专项结论，同 N-A 手法）：工具链 c6root
// gcc 4.4.7 `-O0` 对**类内定义的 inline 成员函数照样发 call**（不做内联），
// 于是每个访问点从 ORIG 的单条直访变成「装载 off → 装载 this → call」3-5 条；
// upgrade_table_t::C1 12 个点、importUpgradeTable 16 个点、get_upgrade_info
// 11 个点各多一组，实测三函数合计偏差 ~190 insn。宏在预处理期展开为
// `mov/flds off(%reg,%reg,4)`，与 ORIG 同形且**不新增任何符号**。
//
// 锚点是已建真实成员 m_bodyView.m_body（union 具名视图），非 (char*)this+K 裸
// 偏移；写用途用非 const 版，只读用途用 C 版（可在 const 成员函数/const 指针
// 上求值）。tbl 表达式会被求值两次（地址 + 偏移），调用点必须传**无副作用**的
// 指针/引用（现有全部使用点均满足）。
// ---------------------------------------------------------------------------
#define UPG_TBL_INT(tbl, off) \
    (*reinterpret_cast<int*>((tbl)->m_bodyView.m_body + ((off) - 0xc)))
#define UPG_TBL_FLT(tbl, off) \
    (*reinterpret_cast<float*>((tbl)->m_bodyView.m_body + ((off) - 0xc)))
#define UPG_TBL_CINT(tbl, off) \
    (*reinterpret_cast<const int*>((tbl)->m_bodyView.m_body + ((off) - 0xc)))
#define UPG_TBL_CFLT(tbl, off) \
    (*reinterpret_cast<const float*>((tbl)->m_bodyView.m_body + ((off) - 0xc)))

// dword 下标版（k = (off - 0xc)/4，即“元素序号”）：用于 ORIG 的
// `add $K,%edx; movl/flds disp(%eax,%edx,4)` 两步形（P30/N81）。
#define UPG_TBL_I32(tbl, k)  ((tbl)->m_bodyView.m_i32[(k)])
#define UPG_TBL_F32(tbl, k)  ((tbl)->m_bodyView.m_f32[(k)])

class CItemAmplifier
{
public:
    CItemAmplifier();

    bool checkInvestableItem(const CItem* item, const Inven_Item& inven,
                             int flag) const;
    void generateAmplifyItem(const CItem* item, Inven_Item& inven) const;

    // ---- 强化投资（投资材料查询/启用/消费）----
    // 均 const；返回码语义见各实现（与 Dispatcher_InvestItemAmplifyOption 配套）。
    bool getInvestMaterialInfo(ENUM_ITEM_AMPLIFY_INVEST_TYPE type,
                               unsigned long level,
                               stInvestAmplifyOption_t& info) const;
    int enableInvestAmplifyOption(const Inven_Item& inven,
                                  ENUM_ITEM_AMPLIFY_INVEST_TYPE type) const;
    void investAmplifyOption(const CItem* item, Inven_Item& inven,
                             const stInvestAmplifyOption_t& option) const;
    // ---- 净化材料查询（ORIG W 0x08234d20 / 0x08234cd4 / 0x08234cfa）----
    // type∈[0,1] 对应 [purify material]/[purify only material] 两行；
    // 数据来自 stAmplifyItemScript_t（+0x9c..+0xa8，importAmplifyItem 填充）。
    // getPurifyMaterialType 返回 itemIdx 所在行的 type（无匹配 -1），
    // 其余两个按 type 返回 itemIdx/count，type 越界返回 0。
    int getPurifyMaterialType(int itemIdx) const;
    int getPurifyMaterialItemIdx(int type) const;
    int getPurifyMaterialItemCount(int type) const;

    // ORIG W 0x0854b382：返回内嵌 upgrade_table_t（+0x100）。
    const upgrade_table_t* getUpgradeInfoTable() const { return &m_upgrade; }

    // ORIG _ZN14CItemAmplifier10loadScriptEPKcS1_ W 0x08376060
    // （CDataManager::init 调用：Etc/AmplifyItem.etc + Etc/AmplifyUpgrade.etc）
    // 返回 int：ORIG 0x08376060 尾透传 importUpgradeTable 错误行号（0=成功）。
    int loadScript(const char* itemScript, const char* upgradeScript);

private:
    void _generateOption(const CItem* item, ENUM_AMPLIFY_TYPE& type,
                         unsigned short& value) const;

    stAmplifyItemScript_t m_script;   // +0x00
    upgrade_table_t m_upgrade;        // +0x100
};


// ---- 强化/升级脚本导入（定义待脚本导入域批次；loadScript 依赖）----
// ORIG T 0x0890a6b0 _Z17importAmplifyItemPKcR21stAmplifyItemScript_t：返回错误行，0=成功。
int importAmplifyItem(const char* path, stAmplifyItemScript_t& out);
// ORIG T 0x08900c26 _Z18importUpgradeTablePKcP15upgrade_table_t：返回错误行，0=成功。
int importUpgradeTable(const char* path, upgrade_table_t* table);
 
#endif  // GAME_CITEMAMPLIFIER_H_
