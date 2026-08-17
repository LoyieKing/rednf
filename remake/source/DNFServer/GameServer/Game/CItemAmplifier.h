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

#include <vector>

class CItem;
class Inven_Item;

// ---- 强化选项（Inven_Item::m_amp @ +0x11；尺寸 0xe）----
#pragma pack(push, 1)
class stAmplifyOption_t
{
public:
    void reset();
    bool isIdentified() const;
    bool hasAbility() const;
    void identify();
    char getAbilityType() const;
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

// ---- 强化脚本表（CItemAmplifier +0x00，0x100）----
class stAmplifyItemScript_t
{
public:
    stAmplifyItemScript_t();

    char m_pad0[0x18];                            // +0x00
    std::vector<stAmplifyOptionInfo_t> m_options; // +0x18
    float m_rarityRate[6];                        // +0x24
    int m_field3c;                                // +0x3c
    char m_pad40[0xe8 - 0x40];                    // +0x40
    int m_rarityProb[6];                          // +0xe8
};

// ---- 强化升级表（CItemAmplifier +0x100，0x4e4）----
class upgrade_table_t
{
public:
    upgrade_table_t();

    char m_pad0[0x25c];      // +0x00
    int m_gradeLimit[6];     // +0x25c（ORIG 访问 this + 0x35c + rarity*4）
    char m_pad274[0x4e4 - 0x274];
};

class CItemAmplifier
{
public:
    CItemAmplifier();

    bool checkInvestableItem(const CItem* item, const Inven_Item& inven,
                             int flag) const;
    void generateAmplifyItem(const CItem* item, Inven_Item& inven) const;

private:
    void _generateOption(const CItem* item, ENUM_AMPLIFY_TYPE& type,
                         unsigned short& value) const;

    stAmplifyItemScript_t m_script;   // +0x00
    upgrade_table_t m_upgrade;        // +0x100
};

#endif  // GAME_CITEMAMPLIFIER_H_
