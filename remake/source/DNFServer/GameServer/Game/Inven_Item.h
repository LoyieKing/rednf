#ifndef GAME_INVEN_ITEM_H_
#define GAME_INVEN_ITEM_H_

// ============================================================================
// df_game_r 还原 —— Inven_Item（背包槽物品实例）。
// 布局依据 ORIG 构造（0x80cb854）/ reset（0x80cb7d8）/ setCopy（0x814a62e）
// 逐成员推导（pack(1)，总尺寸 0x3d）：
//   +0x00 byte                  +0x11 stAmplifyOption_t（0xe）
//   +0x01 byte（ITEM_TYPE）     +0x1f ChronicleRandomOptionInfo（0x14）
//   +0x02 int（item idx）       +0x33 UpgradeSeparateInfo（1）
//   +0x06 byte（attr/upgrade）  +0x34 ReservedCapacity（9）
//   +0x07 int（add_info）
//   +0x0b ushort（endurance）
//   +0x0d int
// ============================================================================

#include "CItemAmplifier.h"

#pragma pack(push, 1)

class STEnchantedSkillOption
{
public:
    void reset();

    unsigned char m_field;   // +0x00
};

class RandomOptionField
{
public:
    void reset();

    unsigned char m_field0;  // +0x00
    unsigned char m_field1;  // +0x01
    unsigned char m_field2;  // +0x02
};

class RandomOptionSeed
{
public:
    void reset();

    unsigned char m_field;   // +0x00
};

class RandomOption
{
public:
    void reset();

    // ORIG 0x858e3ec（PacketBuf 实例）/ 0x811f1ae（PacketGuard 实例）：
    // 3 组 RandomOptionField（+0/+3/+6）+ seed(+9) + field(+0xa) + seed(+0xd)
    template <typename T>
    void put_packet_random_option(T& packet) const
    {
        int index = packet.get_index();
        packet.put_byte(0);
        int count = 0;
        const unsigned char* self = (const unsigned char*)this;
        for (int i = 0; i <= 2; ++i)
        {
            const unsigned char* f = self + i * 3;
            if (f[0] != 0 || f[1] != 0 || f[2] != 0)
            {
                packet.put_byte(f[0]);
                packet.put_byte(f[1]);
                packet.put_byte(f[2]);
                ++count;
            }
        }
        packet.put_byte(index, count);
        if (count != 0)
        {
            packet.put_byte(self[9]);
            const unsigned char* a = self + 0xa;
            if (a[0] != 0 || a[1] != 0 || a[2] != 0)
            {
                packet.put_byte(self[0xd] & 3);
                packet.put_byte(self[0xd]);
                packet.put_byte(a[0]);
                packet.put_byte(a[1]);
                packet.put_byte(a[2]);
            }
            else
            {
                packet.put_byte(-1);
            }
        }
    }

    RandomOptionField m_field0;  // +0x00
    RandomOptionField m_field3;  // +0x03
    RandomOptionField m_field6;  // +0x06
    RandomOptionSeed m_seed9;    // +0x09
    RandomOptionField m_fielda;  // +0x0a
    RandomOptionSeed m_seedd;    // +0x0d
};

class ChronicleRandomOptionInfo
{
public:
    void reset();

    STEnchantedSkillOption m_skill0;  // +0x00
    STEnchantedSkillOption m_skill1;  // +0x01
    int m_field2;                     // +0x02
    RandomOption m_random;            // +0x06
};

class UpgradeSeparateInfo
{
public:
    UpgradeSeparateInfo();
    void reset();
    unsigned char GetUpgradeSeparate() const;
    void SetUpgradeSeparate(unsigned char value);
    bool IsTradeRestriction() const;

    unsigned char m_field;   // +0x00
};

class ReservedCapacity
{
public:
    ReservedCapacity();
    void reset();

    int m_field0;        // +0x00
    int m_field4;        // +0x04
    unsigned char m_field8;  // +0x08
};

class Inven_Item
{
public:
    Inven_Item() __attribute__((nothrow));  // ORIG header 内联定义（W），nothrow 抑制 ctor 清理块
    void reset();

    void set_add_info(int value);
    int get_add_info() const;
    void ResetItemAttr();

    unsigned char GetUpgrade() const;
    void SetUpgrade(unsigned char upgrade);
    void IncUpgrade();
    unsigned char GetItemAttr() const;
    void SetItemAttr(unsigned char attr);
    unsigned char GetReSealCount() const;
    void SetReSealCount(unsigned char count);
    unsigned char GetTradeLimitCount() const;
    void SetTradeLimitCount(unsigned char count);
    bool isAvatarItemType() const;
    bool IsCreatureItemType() const;
    bool isEquipableItemType() const;
    bool isEmpty() const;
    void setCopy(const Inven_Item& other);
    int getPackedCode1() const;
    int getKey() const;

    int GetInventoryType();
    int GetItemSpace() const;

    // ORIG 嵌套枚举（mangled _ZN10Inven_Item9ITEM_TYPEE，值取自
    // CInventory::GetItemType / Inven_Item.cpp 的 m_field1 比较）
    enum ITEM_TYPE
    {
        ITEM_TYPE_0 = 0,
        ITEM_TYPE_EQUIP = 1,
        ITEM_TYPE_2 = 2,
        ITEM_TYPE_3 = 3,
        ITEM_TYPE_4 = 4,
        ITEM_TYPE_CREATURE_EQUIP = 5,
        ITEM_TYPE_ARTIFACT = 6,
        ITEM_TYPE_CREATURE_STACK = 7,
        ITEM_TYPE_AVATAR = 8,
        ITEM_TYPE_9 = 9,
        ITEM_TYPE_10 = 10,
        ITEM_TYPE_INVALID = 0xb
    };

    char m_field0;                     // +0x00
    unsigned char m_field1;            // +0x01（ITEM_TYPE）
    int m_addInfo;                     // +0x02（item idx）
    unsigned char m_attr;              // +0x06（upgrade:bit0-4 / reseal:bit5-7）
    int m_addInfo2;                    // +0x07（set_add_info / get_add_info）
    unsigned short m_fieldb;           // +0x0b（endurance）
    int m_fieldd;                      // +0x0d
    stAmplifyOption_t m_amp;           // +0x11
    ChronicleRandomOptionInfo m_random; // +0x1f
    UpgradeSeparateInfo m_upgradeSep;  // +0x33
    ReservedCapacity m_reserved;       // +0x34
};

#pragma pack(pop)

#endif  // GAME_INVEN_ITEM_H_
