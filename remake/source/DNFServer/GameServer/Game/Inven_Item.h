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

    bool empty() const;      // ORIG _ZNK17RandomOptionField5emptyEv W 0x811ed22：
                             //   仅检查首字节==0（报告缺失，objdump 直转）

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

// ORIG enum（_ZN18randomOptionReason1TE；cUserHistoryLog::RandomOption 第 2 参）。
// 已实证实参：1=RegenerationROI 再铸、3=ResetRandomOption 重置。
namespace randomOptionReason
{
enum T
{
    T_REGENERATION = 1,
    T_RESET = 3
};
}

// ORIG mangled 参数枚举（reset_option / RandomOption 访问器；取值 0..2 选择
// 三槽之一。枚举成员名未从 DWARF 还原，按槽序命名 [推断]）。
enum ENUM_RANDOM_OPTION_NUMBER
{
    RANDOM_OPTION_NUMBER_0 = 0,
    RANDOM_OPTION_NUMBER_1 = 1,
    RANDOM_OPTION_NUMBER_2 = 2
};

class RandomOption
{
public:
    void reset();

    // ---- 槽选择/写入访问器（报告缺失，objdump 直转；W 符号实证）----
    bool change_option(ENUM_RANDOM_OPTION_NUMBER num);              // W 0x85f4002（ORIG _ZN 非 const）
    unsigned char get_option_index(ENUM_RANDOM_OPTION_NUMBER num);  // W 0x85f4040
    void set_first_value(ENUM_RANDOM_OPTION_NUMBER num, unsigned char v);   // W 0x85f4078
    void set_second_value(ENUM_RANDOM_OPTION_NUMBER num, unsigned char v);  // W 0x85f40be

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
    // ORIG _ZN12RandomOption5emptyEv W 0x822abd0：首字节为 0 视为未生成选项。
    bool empty();

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
    void IncUpgradeSeparate();   // ORIG W 0x0811ed32：低 5 位 +1（模 32），保留高位
    void SetUpgradeSeparate(unsigned char value);
    void SetUpgradeSeparateAndTradeRestriction(unsigned char value); // ORIG 0x085574bc (W)
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

    unsigned char m_field0;            // +0x00（ORIG 反汇编一律 movzbl 无符号读）
    unsigned char m_field1;            // +0x01（ITEM_TYPE）
    int m_addInfo;                     // +0x02（item idx）
    unsigned char m_attr;              // +0x06（upgrade:bit0-4 / reseal:bit5-7）
    int m_addInfo2;                    // +0x07（set_add_info / get_add_info）
    unsigned short m_fieldb;           // +0x0b（endurance）
    union
    {
        int m_fieldd;                  // +0x0d（整体读写口径：SendUpdateItem 包字段、
                                       //      UseLimitCube 结果数、CCreatureMgr 拷贝）
        struct
        {
            short m_fielddLo;          // +0x0d（低 16 位）
            unsigned short m_avatarEndur;  // +0x0f（头像/徽章耐久计数：
                                           //      AvatarRechargeServer::CalcDurabilityDetail
                                           //      0x081902e8 实证读减并经 UpdateEndurance 上报）
        } m_fieldSplit;
    };                                 // +0x0d..0x10
    stAmplifyOption_t m_amp;           // +0x11
    ChronicleRandomOptionInfo m_random; // +0x1f
    UpgradeSeparateInfo m_upgradeSep;  // +0x33
    ReservedCapacity m_reserved;       // +0x34
};

// ============================================================================
// Inven_Item_Compatible —— ORIG 0x833f38e/0x833f418（user_creature DB 域使用）。
// Inven_Item 的 DB 兼容子集布局（setCopy 逐字段转写推导，pack(1)）：
class Inven_Item_Compatible
{
public:
    void setCopy(const Inven_Item& item);        // ORIG _ZN21Inven_Item_Compatible7setCopyERK10Inven_Item W 0x833f38e
    void makeInvenItem(Inven_Item& item);        // ORIG _ZN21Inven_Item_Compatible13makeInvenItemER10Inven_Item W 0x833f418

    unsigned char m_field0;     // +0x00（← item+0x00）
    unsigned char m_type;       // +0x01（ITEM_TYPE，← item+0x01）
    int m_itemIdx;              // +0x04（← item+0x02）
    unsigned char m_attr;       // +0x08（← Inven_Item::GetItemAttr）
    int m_addInfo2;             // +0x0c（← item+0x07）
    unsigned short m_endurance; // +0x10（← item+0x0b）
    int m_fieldd;               // +0x14（← item+0x0d）
    int m_amp0;                 // +0x18（← item+0x11）
    int m_rand0;                // +0x1c（← item+0x15）
    int m_rand4;                // +0x20（← item+0x19）
    unsigned short m_rand8;     // +0x24（← item+0x1d）
};                              // 总 0x26

#pragma pack(pop)

class CUser;   // 前向：CheckItemTradeLimitCountToMoveByUser

// ORIG _Z23DecreaseTradeLimitCountR10Inven_Item T 0x0812013c：
// 交易次数限制递减。GetTradeLimitCount() 非 0 时 Set(count-1) 并返回 true，
// 否则返回 false（反编译见 docs/class_func_reports/_lt_global_gt_/DecreaseTradeLimitCount.md）。
bool DecreaseTradeLimitCount(Inven_Item& item);

// ORIG T 0x081202a9 _Z30CheckTradeLimitItemCountToMoveRK10Inven_ItemRb：
// proto 可 stackable/attachType!=7 时直接通过；attach==7 且交易限次非 0 → false。
bool CheckTradeLimitItemCountToMove(const Inven_Item& item, bool& ok);
// ORIG T 0x0812032e _Z36CheckItemTradeLimitCountToMoveByUserR5CUseriiRb：
// *ok=true；curCharac 缺失直接返回；GetInvenSlot(space,slot) 取实体后转上一函数。
bool CheckItemTradeLimitCountToMoveByUser(CUser& user, int space, int slot, bool& ok);

// ORIG T 0x081201dc _Z26IsTradeLimitAttachTypeItemRK10Inven_Item：
// 交易限制/绑定类物品判定（exchange_interspace 0xc 域前置校验；定义由后续批次补）。
bool IsTradeLimitAttachTypeItem(const Inven_Item& item);

// ORIG T 0x08120242 _Z32IsTradeLimitAttachTypeItemByUserR5CUserii：
// getCurCharacR()==0 → false；GetInvenSlot(space,slot).m_addInfo==0 → false；
// 否则委托上一函数。（定义 CMailBoxHelper.cpp）
bool IsTradeLimitAttachTypeItemByUser(CUser& user, int space, int slot);
#endif  // GAME_INVEN_ITEM_H_
