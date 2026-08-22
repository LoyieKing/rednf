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

namespace ItemUpgradeType
{
enum T
{
    T_0 = 0
};
}

struct stAvatarTypeSelect_t
{
    int m_field0;       // +0x00
    int m_field4;       // +0x04
    int m_field8;       // +0x08
    int m_fieldc;       // +0x0c
    int m_field10;      // +0x10（ORIG：此选择项小孔数/数组长度）
    unsigned short m_socketType[4];  // +0x14
};

struct stAvatarEmblemInfo_t;  // 定义在 CInventory.h / AuctionItem.h（30B，pack(1)）

struct WpBonusPointData
{
    char m_pad[8];
};

struct ExpBonusPointData
{
    char m_pad[8];
};

class STEquipmentScript;

class RandomItemTable
{
public:
    RandomItemTable();
    ~RandomItemTable();
    void Set(std::vector<int>* source);
    int m_field0;                          // +0x00
    std::vector<std::pair<int, int> > m_vec4;   // +0x04
    int m_field10;                         // +0x10
    int m_field14;                         // +0x14
    int m_field18;                         // +0x18
    int m_field1c;                         // +0x1c
    int m_field20;                         // +0x20
    bool m_field24;                        // +0x24
    char m_pad25[3];                       // +0x25
    int m_endurance;                       // +0x28
};

class CEquipItem : public CItem
{
public:
    int GetUsableMaxLevel() const;  // ORIG 084e9700（返回 +0x1c0）
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

    void setAvatarOptionSize(int size);
    unsigned char IsAvatarColorVariation(int idx) const;
    bool IsAvatarOptionValid(char idx) const;
    int check_able_To_equip(char eType, int job, int level);
    bool check_eType(char eType) const;
    ExpBonusPointData* getExpBonusPoint() const;
    int getGrowthGrade(int level) const;
    int getGrowthRepairCost(int level) const;
    WpBonusPointData* getWpBonusPoint() const;
    int get_aura_avatar_option_value();
    bool isExistUpgradableLevelTag();
    unsigned char isGrowthType() const;
    bool isUpgradableLevel(ItemUpgradeType::T type, int level);
    bool is_aura_avatar_option(char option);
    int get_endurance() const;
    int get_sub_type() const;  // ORIG W 0x0833eecc
    void getAvatarSocket(int slot, stAvatarEmblemInfo_t& emblem) const;  // ORIG W 0x08150f36
    void* getAvatarTypeSelect() const;
    void set_item(STEquipmentScript& script);
    bool verify();
    int getAvatarPeriod(unsigned char a) const;  // ORIG W 0x08151120

    RandomItemTable m_randomTable;         // +0x148
    std::vector<stAvatarTypeSelect_t> m_avatarSelect;  // +0x174
    std::vector<unsigned short> m_vec180;  // +0x180
    int m_field18c;                        // +0x18c
    int m_repairCost;                      // +0x190
    char m_char194;                        // +0x194
    char m_pad195[3];                      // +0x195
    std::vector<std::pair<int, int> > m_vec198;   // +0x198
    std::vector<std::pair<int, int> > m_vec1a4;   // +0x1a4
    std::vector<char> m_enchantVec;        // +0x1b0
    int m_field1bc;                        // +0x1bc
    int m_field1c0;                        // +0x1c0
    int m_avatarOptionSize;                // +0x1c4
    bool m_randomOption;                   // +0x1c8
    char m_randomGrade;                    // +0x1c9
    char m_pad1ca[2];                      // +0x1ca
    int m_field1cc;                        // +0x1cc
    int m_field1d0;                        // +0x1d0
    int m_field1d4;                        // +0x1d4
    char m_auraOptionType;                 // +0x1d8
    char m_pad1d9[3];                      // +0x1d9
    int m_auraOptionValue;                 // +0x1dc
    int m_field1e0;                        // +0x1e0
    int m_field1e4;                        // +0x1e4
    int m_field1e8;                        // +0x1e8
    int m_field1ec;                        // +0x1ec
    std::vector<char> m_avatarAbility;     // +0x1f0
    bool m_pollutionFlag;                  // +0x1fc
    char m_pad1fd[3];                      // +0x1fd
    std::map<ItemUpgradeType::T, std::pair<int, int> > m_upgradeLevel;  // +0x200
    char m_avatarColor[2];                 // +0x218
    char m_growthFlag;                     // +0x21a
    char m_pad21b[1];                      // +0x21b
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
