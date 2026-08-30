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

    unsigned char m_abilityType;     // +0x00（bit7 = identified）
    unsigned short m_abilityValue;   // +0x01
    unsigned char m_lock;            // +0x03
    char m_pad4[0xa];                // +0x04（尺寸 0xe）
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
    int m_purifyMatItem;                          // +0x9c（[purify material] 第 1 值）
    int m_purifyOnlyMatItem;                      // +0xa0（[purify only material] 第 1 值）
    int m_purifyMatCount;                         // +0xa4（[purify material] 第 2 值）
    int m_purifyOnlyMatCount;                     // +0xa8（[purify only material] 第 2 值；
                                                  //      名称序「推断」，偏移实证）
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
    // ORIG C1/C2 0x0854b282（W）：+0x00..+0x68 全部清零
    // （前 8 字 edx 中转 movl、+0x20..+0x34 直 movl、两段循环）。
    upgrade_info_t()
    {
        memset(m_pad0, 0, sizeof(m_pad0));
        m_recipeItem = 0;
        m_meterialCount = 0;
        m_discountBase = 0;
        memset(m_pad, 0, sizeof(m_pad));
    }
    char m_pad0[0x2c];        // +0x00 概率/等级混合区（float 偏移访问）
    int m_recipeItem;         // +0x2c（材料 item idx / 强化商品）
    int m_meterialCount;      // +0x30（材料数量）
    int m_discountBase;       // +0x34（折扣基数；updateUpgradeTable... 亦写 +0x2c/+0x30）
    char m_pad[0x34];         // +0x38..+0x6b 尾部补齐至 0x6c
};

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
    char m_body[0x434 - 0x00c];                       // +0x00c 混合表（至 0x433）
    ST3rdChronicleDestructionResult m_destruction;    // +0x434（ORIG C1 0x089000ca 直构；
                                                      //   内部 m_vec @+0x490 = 旧 m_resultItems）
    std::map<int, float> m_floatMap;                  // +0x49c（[cost weight by upgrade level]）
    int m_zero4b4[4];                                 // +0x4b4..+0x4c0（machine frame base time ×4）
    std::unordered_map<int, MachineFrameInfo> m_machineFrames;  // +0x4c4
                                                       // （sizeof 0x20，与 ORIG 一致）

    // ORIG 直访视图（off 为对象内绝对偏移）。
    int& intAt(int off) { return *(int*)(m_body + off - 0xc); }
    float& floatAt(int off) { return *(float*)(m_body + off - 0xc); }
    float floatAt(int off) const { return *(const float*)(m_body + off - 0xc); }
    int intAtConst(int off) const { return *(const int*)(m_body + off - 0xc); }

    // ORIG 0x08900436：按 rarity/growthGrade/upgrade/equipType 计算 upgrade_info 到 out。
    // 返回 0 表示越界/失败，1 表示成功。
    int get_upgrade_info(upgrade_info_t* out, unsigned char rarity,
                         unsigned char growthGrade, unsigned char upgrade,
                         ENUM_EQUIPMENTTYPE equipType) const;
    // ORIG 0x089003ee：判断惩罚类型；返回 3 或 upgrade_info.m_discountBase 编码值。
    int GetPenaltyType(unsigned char upgrade, ENUM_RARITY rarity,
                       const upgrade_info_t* info) const;
};

class CItemAmplifier
{
public:
    CItemAmplifier();

    bool checkInvestableItem(const CItem* item, const Inven_Item& inven,
                             int flag) const;
    void generateAmplifyItem(const CItem* item, Inven_Item& inven) const;

    // ---- 强化投资（投资材料查询/启用/消费）----
    // 均 const；返回码语义见各实现（与 Dispatcher_InvestItemAmplifyOption 配套）。
    int getInvestMaterialInfo(ENUM_ITEM_AMPLIFY_INVEST_TYPE type,
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
