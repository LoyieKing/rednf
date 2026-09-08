#ifndef GAME_CEQUIPITEM_H_
#define GAME_CEQUIPITEM_H_

// ============================================================================
// df_game_r 还原 —— CEquipItem（0x23c 布局，单继承 CItem）
// 布局依据 ORIG 构造（0x851547e）/ 报告函数（CEquipItem.md）逐成员推导：
//   +0x148 RandomItemTable（0x2c）  +0x174 vector<stAvatarTypeSelect_t>
//   +0x180 vector<ushort>  +0x198/0x1a4 vector<pair<int,int>>
//   +0x1b0 vector<STEnchantSystemData>  +0x1dc auraOptionValue
//   +0x1e0 WpBonusPointData  +0x1e8 ExpBonusPointData
//   +0x1f0 vector<STAvatarAbilityVariation>  +0x1fc pollutionFlag
//   +0x200 map<ItemUpgradeType::T, pair<int,int>>  +0x218 avatarColor[2]
//   +0x21a growthFlag  +0x21c..+0x230 growth 参数
//   +0x234 itemType  +0x238 avatarSelectIdx
// ============================================================================

#include <map>
#include <utility>
#include <vector>

#include "CItem.h"

struct STAvatarAbilityVariation;  // 完整定义见 EquipmentParameterInfo.h / STEquipmentScript.h（1 int）
struct STEnchantSystemData;       // 完整定义见 STEquipmentScript.h（1 int）

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

// ORIG 布局（init 0x0898d1fc 实证）：5 int（+0x00..+0x13）+ 5 ushort（+0x14..+0x1d）
// = sizeof 0x20；与 STEquipmentScript.h 同 guard 同定义（Fix18_E 收敛，原 [4] 错误）。
#ifndef DNF_STAVATARTYPESELECT_T_DEFINED
#define DNF_STAVATARTYPESELECT_T_DEFINED
struct stAvatarTypeSelect_t
{
    int m_field0;       // +0x00
    int m_field4;       // +0x04
    int m_field8;       // +0x08
    int m_fieldc;       // +0x0c
    int m_field10;      // +0x10（ORIG：此选择项小孔数/数组长度）
    unsigned short m_socketType[5];  // +0x14..+0x1d

    void init();        // ORIG T 0x0898d1fc（定义 STEquipmentScript.cpp）
};
#endif

struct stAvatarEmblemInfo_t;  // 定义在 CInventory.h / AuctionItem.h（30B，pack(1)）

// ORIG checkBonusPoint 0x868ec5c：getWpBonusPoint()/getExpBonusPoint() 返回
// 8 字节视图 {int type; int value;}；type==0 → 加 CUserCharacInfo 基础值
// (m_base)、type==1 → 加百分比附加值 (m_add)，value 为加数。
// [b28d-solo-CEquipItem] WpBonusPointData/ExpBonusPointData 定义已按 §4.4 下沉到
// STItemScript.h（与 STEquipmentScript 的 m_wpBonus81c/m_expBonus824 统一为同一类型；
// 经 CItem.h → STItemScript.h 在本头可见），此处不再重复定义。

class STEquipmentScript;

class RandomItemTable
{
public:
    RandomItemTable();
    ~RandomItemTable();
    void Set(std::vector<int>* source);
    // ORIG 0x089bbb6c（_ZNK15RandomItemTable18GetRandomItemIndexEi，const）：
    // 在 m_vec4（+0x04，pair<int,int> 数组）中找 first > param 的首元素，返回其 first；
    // 未命中返回 0。SetEggItem 依赖此索引。
    int GetRandomItemIndex(int index) const;
    int m_field0;                          // +0x00
    std::vector<std::pair<int, int> > m_vec4;   // +0x04
    // [b28c-21] +0x10 位模式双视图：get_experience_amount 按 float 读（ORIG
    // 0x833eea6 单条 `mov 0x158(%eax),%eax`；引用转换形 4.4 恒 add+mov）。
    // sizeof/偏移不变（两成员同为 4 字节，union 尺寸 4）。
    union {
        int m_field10;                     // +0x10
        float m_experienceAmount;          // +0x10（float 位视图）
    };
    int m_field14;                         // +0x14
    int m_field18;                         // +0x18
    int m_field1c;                         // +0x1c
    int m_field20;                         // +0x20
    bool m_field24;                        // +0x24
    int m_endurance;                       // +0x28
};

class CEquipItem : public CItem
{
public:
    int GetUsableMaxLevel() const;         // ORIG W 0x084e9700（返回 +0x1c0）
    int GetUsableEquipmentType() const;    // ORIG W 0x0832e036（返回 +0x1bc；定义见 UserCoreDepsB2.cpp）
public:
    CEquipItem();
    virtual ~CEquipItem();
    virtual void make_item(Inven_Item& item) const;
    virtual int GetItemType() const;
    virtual bool IsAvatarItem() const;
    virtual bool IsCreatureItem() const;
    virtual bool IsClearAvatar() const;
    virtual bool IsEnchantalbe() const;
    virtual bool getUsableItemType(int type) const;
    virtual bool isEnableAvatarFunc(ENUM_AVATAR_FUNC_FILTER filter) const;
    virtual int GetGuildPowerWarPoint() const;
    virtual int GetUserPowerWarPoint() const;
    virtual bool IsImpossibleUpgrade() const;
    virtual bool IsImpossibleAmplify() const;
    virtual bool IsImpossibleDisjoint() const;
    virtual bool IsImpossibleGift() const;
    virtual bool IsRandomOption() const;
    virtual int GetRandomOptionGrade() const;
    virtual bool isAmplifyPollutionItem() const;
    virtual bool checkItemUpgradePossible() const;
    virtual void set_parent_booster_item_id(unsigned long id);
    virtual unsigned long get_parent_booster_item_id() const;
    void set_item(STEquipmentScript& script);  // ORIG T 0x0850fd84

    void setAvatarOptionSize(int size);
    int getAvatarOptionSize() const;     // ORIG W 0x81803bc：返回 +0x1c4
    void* getAvatarTypeSelect() const;    // ORIG W 0x08150f28，返回 m_avatarSelect 视图
    unsigned char IsAvatarColorVariation(int idx) const;
    bool IsAvatarOptionValid(char idx) const;
    int check_able_To_equip(char eType, int job, int level);
    bool check_eType(char eType) const;
    ExpBonusPointData* getExpBonusPoint() const;
    int getGrowthGrade(int level) const;
    int getGrowthRepairCost(int level) const;
    // ORIG 0x822ca80：返回 +0x1cc 的 float 折扣率（强化成本折扣）。
    float getUpgradeCostDiscount() const;
    // ORIG _ZNK10CEquipItem22getUpgradeProbIncreaseEv W 0x0854b552：返回 +0x1d0 int
    //（强化成功率提升值；_DoProcUpgrade 扣减概率用）。
    int getUpgradeProbIncrease() const;
    WpBonusPointData* getWpBonusPoint() const;
    int get_aura_avatar_option_value();
    bool isExistUpgradableLevelTag();
    unsigned char isGrowthType() const;
    bool isUpgradableLevel(ItemUpgradeType::T type, int level);
    bool is_aura_avatar_option(char option);
    int get_endurance() const;
    int get_sub_type() const;  // ORIG W 0x0833eecc
    // ORIG W 0x833eea6（flds [this+0x158]）：经验量，位模式存于 m_randomTable.m_field10。
    float get_experience_amount() const;
    // ORIG W 0x833eee8：返回 this+0x168（m_randomTable.m_field20）。
    int get_creature_minimum_level() const;
    // ORIG W 0x0833eebe：返回 m_randomTable.m_field14（+0x15c）。
    int get_creature_species() const;
    // ORIG _ZNK10CEquipItem17get_hatch_item_idEv W 0x0833eeda size 0xe（nm -S 核验），
    // 体内仅 mov 0x164(%eax),%eax → m_randomTable.m_field1c（蛋孵化物品 id；
    // set_item case 1 写入）。调用方 CEgg::Hatch @0x833912a 是外联 call（0x8339181），
    // 并非头内联展开。
    int get_hatch_item_id() const;
    // ORIG _ZNK10CEquipItem20getEnchantSystemDataEv W 0x0854b544：返回 +0x1b0（m_enchantVec）
    const std::vector<STEnchantSystemData>& getEnchantSystemData() const;
    void getAvatarSocket(int slot, stAvatarEmblemInfo_t& emblem) const;  // ORIG W 0x08150f36
    // ORIG _ZN10CEquipItem12IsEmancipateEv W 0x0822ca70：movzbl +0x194 直返（P9）
    unsigned char IsEmancipate();
    // ORIG _ZN10CEquipItem25GetEmancipateMaterialListEv W 0x0854b528：
    //   push ebp; mov esp,ebp; mov 0x8(%ebp),%eax; add $0x198,%eax; pop; ret（直返 &m_vec198）
    std::vector<std::pair<int, int> >& GetEmancipateMaterialList();
    // ORIG _ZN10CEquipItem23GetEmancipateResultListEv W 0x0854b536：
    //   同形 add $0x1a4（直返 &m_vec1a4）
    std::vector<std::pair<int, int> >& GetEmancipateResultList();
    bool verify();
    int getAvatarPeriod(unsigned char a) const;  // ORIG W 0x08151120

    RandomItemTable m_randomTable;         // +0x148
    std::vector<stAvatarTypeSelect_t> m_avatarSelect;  // +0x174
    std::vector<unsigned short> m_vec180;  // +0x180
    int m_field18c;                        // +0x18c
    int m_repairCost;                      // +0x190
    char m_char194;                        // +0x194
    std::vector<std::pair<int, int> > m_vec198;   // +0x198
    std::vector<std::pair<int, int> > m_vec1a4;   // +0x1a4
    std::vector<STEnchantSystemData> m_enchantVec;  // +0x1b0
    int m_field1bc;                        // +0x1bc
    int m_field1c0;                        // +0x1c0
    int m_avatarOptionSize;                // +0x1c4
    bool m_randomOption;                   // +0x1c8
    char m_randomGrade;                    // +0x1c9
    float m_field1cc;                      // +0x1cc (ORIG flds 位读)
    int m_field1d0;                        // +0x1d0
    // [b28d-solo-CEquipItem] ORIG set_item 0x0850fe2e 段 `mov 0x60c(%eax),%edx` 为
    // float→int 位拷贝单条 disp；`*(int*)&float` 取址形 4.4 恒 add+mov（N107）⇒
    // 本字段包匿名 union 增 float 位视图（sizeof/偏移/mangled 不变，仅 +0x1d4 增一别名）。
    union {
        int m_field1d4;                    // +0x1d4
        float m_assaultCostDiscount;       // +0x1d4（float 位视图）
    };
    char m_auraOptionType;                 // +0x1d8
    int m_auraOptionValue;                 // +0x1dc
    WpBonusPointData m_wpBonus1e0;         // +0x1e0（8 字节：type@+0x1e0/value@+0x1e4；与 m_field1e0..1e4 同布局）
    ExpBonusPointData m_expBonus1e8;        // +0x1e8（8 字节：type@+0x1e8/value@+0x1ec）
    std::vector<STAvatarAbilityVariation> m_avatarAbility;  // +0x1f0
    bool m_pollutionFlag;                  // +0x1fc
    std::map<ItemUpgradeType::T, std::pair<int, int> > m_upgradeLevel;  // +0x200
    char m_avatarColor[2];                 // +0x218
    char m_growthFlag;                     // +0x21a
    int m_growthA;                         // +0x21c
    int m_growthB;                         // +0x220
    int m_growthC;                         // +0x224
    int m_growthD;                         // +0x228
    int m_growthE;                         // +0x22c
    int m_growthF;                         // +0x230
    int m_itemType;                        // +0x234
    int m_avatarSelectIdx;                 // +0x238
};

#endif  // GAME_CEQUIPITEM_H_
