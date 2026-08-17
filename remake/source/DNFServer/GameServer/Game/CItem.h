#ifndef GAME_CITEM_H_
#define GAME_CITEM_H_

// ============================================================================
// df_game_r 还原 —— CItem（0x148 布局，I1 批次）
// 布局依据 ORIG 构造（0x85152d4）/ 访问器（0x80f12d6 族 / 0x8110c48 族）/
// 报告函数逐成员推导：
//   +0x00 vptr（vtable 0x8c96da8，24 槽：make_item 纯虚 @2、
//         GetItemType..GetUserPowerWarPoint 纯虚 @3..@11、
//         IsImpossible* @12..@21、isAmplifyPollutionItem @22、
//         checkItemUpgradePossible @23）
//   +0x04 index  +0x08 grade  +0x24 pair<int,int>（GetNeedMaterial 兜底）
//   +0x2c std::string name   +0xb0 std::string iconName
//   +0x90/0xc0 std::vector   其余 STItemScript 数据为 POD 区（0x148 止）
// CEquipItem/CStackableItem 单继承 CItem（偏移 0），派生尺寸 0x23c / 0x3f0。
// ============================================================================

#include <string>
#include <set>
#include <utility>
#include <vector>

#include "Inven_Item.h"
#include "STItemScript.h"

enum ENUM_AVATAR_FUNC_FILTER
{
    ENUM_AVATAR_FUNC_FILTER_0 = 0
};

// 世界/频道开关类型（CExpandEquipslot::Change_Equip 调用点 1；非虚成员）
enum ENUM_WORLD_TYPE
{
    ENUM_WORLD_TYPE_1 = 1
};

class CItem
{
public:
    CItem();
    virtual ~CItem();
    virtual void make_item(Inven_Item& item) const = 0;
    virtual int GetItemType() const = 0;
    virtual bool IsAvatarItem() const = 0;
    virtual bool IsCreatureItem() const = 0;
    virtual bool IsClearAvatar() const = 0;
    virtual bool IsEnchantalbe() const = 0;
    virtual bool getUsableItemType(int type) const = 0;
    virtual bool isEnableAvatarFunc(ENUM_AVATAR_FUNC_FILTER filter) const = 0;
    virtual int GetGuildPowerWarPoint() const = 0;
    virtual int GetUserPowerWarPoint() const = 0;
    virtual bool IsImpossibleUpgrade() const;
    virtual bool IsImpossibleAmplify() const;
    virtual bool IsImpossibleDisjoint() const;
    virtual bool IsImpossibleGift() const;
    virtual bool IsImpossibleCharacCargo() const;
    virtual bool IsPossibleKiriProtection() const;
    virtual int PeriodAfterUnsealing() const;
    virtual bool IsRandomOption() const;
    virtual int GetRandomOptionGrade() const;
    virtual bool IsRandomOptionUnseal() const;
    virtual bool isAmplifyPollutionItem() const;
    virtual bool checkItemUpgradePossible() const;
    virtual void set_parent_booster_item_id(unsigned long id) = 0;
    virtual unsigned long get_parent_booster_item_id() const = 0;

    // ---- 非虚访问器（ORIG weak 0x80f12d6 族 / 0x8110c48 族） ----
    const char* GetItemName() const;
    int GetAttachType() const;
    bool is_stackable() const;
    int get_index() const;
    int get_grade() const;
    int getItemGroupName() const;
    int getUsableLevel() const;
    int get_rarity() const;
    int getUsablePeriod() const;
    int getExpirationDate() const;
    unsigned long GetQuestItemDropBouns();
    const char* GetItemIconName() const;

    CItem* GetNeedMaterial() const;
    bool check_item_routing() const;
    unsigned char check_job_type(char job) const;
    bool isEpicRoutingItem() const;
    bool isPackagable() const;
    bool IsEnableWorld(ENUM_WORLD_TYPE type) const;  // 0x822c87c（链接桩由主 agent 提供）
    bool check_low_level(int level) const;
    bool IsHiddenOption() const;
    bool isExpertJobUsable(ENUM_EXPERT_JOB_TYPE type, int level) const;
    float GetExpertJobCompoundMaterialVariation() const;
    float GetExpertJobCompoundRateVariation() const;
    void GetExpertJobCompoundResultVariation(
        STItemScript::SEXPERTJOB::stExpertCompoundResultVariation& out) const;
    float GetExpertJobSelfDisjointBigWinRate() const;
    float GetExpertJobSelfDisjointResultVariation() const;
    void GetExpertJobAdditionalExp(
        STItemScript::SEXPERTJOB::stExpertJobAdditionalExp& out) const;
    int GetFinishPointPrice() const;

    int m_index;                          // +0x04
    int m_grade;                          // +0x08
    int m_field0c;                        // +0x0c
    int m_field10;                        // +0x10
    int m_field14;                        // +0x14
    char m_pad18[0xc];                    // +0x18..0x24
    std::pair<int, int> m_pair24;         // +0x24
    std::string m_str2c;                  // +0x2c（GetItemName）
    int m_field30;                        // +0x30
    int m_attachType;                     // +0x34
    int m_rarity;                         // +0x38
    bool m_jobUsable[0xb];                // +0x3c（check_job_type）
    char m_pad47[1];                      // +0x47
    int m_usableLevel;                    // +0x48
    int m_field4c;                        // +0x4c
    int m_field50;                        // +0x50
    bool m_stackable;                     // +0x54
    char m_pad55[3];                      // +0x55
    int m_int58;                          // +0x58
    int m_int5c;                          // +0x5c
    char m_char60;                        // +0x60
    char m_pad61[3];                      // +0x61
    int m_field64;                        // +0x64
    int m_field68;                        // +0x68
    int m_usablePeriod;                   // +0x6c
    int m_expirationDate;                 // +0x70
    int m_itemGroupName;                  // +0x74
    char m_char78;                        // +0x78
    bool m_bool79;                        // +0x79（isEpicRoutingItem）
    char m_pad7a[2];                      // +0x7a
    int m_int7c;                          // +0x7c
    int m_int80;                          // +0x80
    bool m_impossibleUpgrade;             // +0x84
    bool m_impossibleAmplify;             // +0x85
    bool m_impossibleDisjoint;            // +0x86
    bool m_impossibleGift;                // +0x87
    bool m_impossibleCharacCargo;         // +0x88
    bool m_possibleKiriProtection;        // +0x89
    char m_pad8a[6];                      // +0x8a..0x90
    std::vector<int> m_vec90;             // +0x90
    DyeInfo m_dyeInfo9c;                  // +0x9c（DyeInfo，16 字节）
    int m_periodAfterUnsealing;           // +0xac
    std::string m_strb0;                  // +0xb0（GetItemIconName）
    STSpecialMonsterDrop m_drop;          // +0xb4（STSpecialMonsterDrop，9 字节）
    char m_dropBD;                        // +0xbd
    char m_padBE[2];                      // +0xbe..0xc0
    std::vector<std::pair<int, int> > m_vecC0;  // +0xc0
    int m_questBonus;                     // +0xcc
    char m_padD0[4];                      // +0xd0
    std::set<int> m_setd4;                // +0xd4..0xec
    int m_intEC;                          // +0xec
    int m_intF0;                          // +0xf0
    int m_intF4;                          // +0xf4
    bool m_boolF8;                        // +0xf8（IsHiddenOption）
    char m_padF9[3];                      // +0xf9..0xfc
    float m_floatFC;                      // +0xfc（GetExpertJobCompoundMaterialVariation）
    float m_float100;                     // +0x100（GetExpertJobCompoundRateVariation）
    std::vector<STItemScript::SEXPERTJOB::SUSABLE> m_vec104;  // +0x104..0x110
    STItemScript::SEXPERTJOB::stExpertCompoundResultVariation
        m_compound110;                    // +0x110（8 字节）
    float m_float118;                     // +0x118（GetExpertJobSelfDisjointBigWinRate）
    float m_float11c;                     // +0x11c（GetExpertJobSelfDisjointResultVariation）
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp
        m_additionalExp120;               // +0x120（0x8 字节）
    std::set<STItemScript::ENUM_ITEM_CATEGORY> m_set128;      // +0x128..0x140
    unsigned long m_parentBoosterItemId;  // +0x140（派生类 set/get_parent_booster_item_id）
    char m_char144;                       // +0x144
    char m_pad145[3];                     // +0x145..0x148
};

#endif  // GAME_CITEM_H_
