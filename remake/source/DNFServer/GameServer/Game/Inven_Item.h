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
        // ORIG W 0x0811f1ae（PacketGuard 实例）逐指令对照：
        int count = 0;                                             // ORIG +0x07 movl $0x0,-0x10(%ebp)
        int index = packet.get_index();                            // ORIG +0x17 mov %eax,-0x14(%ebp)
        packet.put_byte(0);                                        // ORIG +0x29 call put_byte
        for (int i = 0; i <= 2; ++i)                               // ORIG +0xcd cmpl $0x2; setle（while 前置 jmp +0x35）
        {
            if (!((const RandomOptionField*)&((const unsigned char*)this)[i * 3])->empty())  // ORIG +0x43 add-add 3i + add this mem（&p[i*3] 形，探针 h1 实证）
            {
                packet.put_byte(((const RandomOptionField*)((int)this + i * 3))->m_field0);  // ORIG +0x61 movzbl (%eax,%ecx,1),%eax；4.4 无双寄存器 movzbl 寻址（N）
                packet.put_byte(((const RandomOptionField*)((int)this + i * 3 + 1))->m_field0);  // ORIG +0x83 lea; add $0x1
                packet.put_byte(((const RandomOptionField*)((int)this + i * 3 + 2))->m_field0);  // ORIG +0xaa lea; add $0x2
                ++count;                                           // ORIG +0xc5 addl $0x1,-0x10
            }
        }
        packet.put_byte(index, count);                             // ORIG +0xf0 call put_byteERii（index 槽取址+count 值传）
        if (count != 0)                                            // ORIG +0xf5 cmpl $0x0,-0x10; je end
        {
            packet.put_byte(m_seed9.m_field);                      // ORIG +0xff movzbl 0x9(%eax); movzbl %al,%edx
            if (!((const RandomOptionField*)((const char*)this + 0xa))->empty())  // ORIG +0x11e add $0xa,%eax; call empty; xor$1; je→put(-1)
            {
                packet.put_byte((unsigned char)(m_seedd.m_field & 3));  // ORIG +0x12d movzbl 0xd; and $0x3; movzbl %al,%edx
                packet.put_byte(m_seedd.m_field);                  // ORIG +0x14d
                packet.put_byte(m_fielda.m_field0);                // ORIG +0x166 movzbl 0xa
                packet.put_byte(m_fielda.m_field1);                // ORIG +0x17f
                packet.put_byte(m_fielda.m_field2);                // ORIG +0x198
            }
            else
            {
                packet.put_byte(-1);                               // ORIG +0x1b3 movl $0xffffffff
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
    unsigned char IsTradeRestriction() const;

    // +0x00（upgrade:bit0-4 / restriction:bit5 / 保留:bit6-7）。位域证据同上
    // （IsTradeRestriction 0x8110b0a、SetUpgradeSeparate 0x84e906a 位域装填形）。
    union
    {
        unsigned char m_field;
        struct { unsigned char m_lo:5, m_flag:1, m_hi:2; } m_bits;
    };
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
    bool isEquipableItemType() const;  // ORIG 0x85f77ca 调用点 test %al（0x868e07a xor$1），本体 0x08150812 mov$1/mov$0 bool 形
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

    bool m_field0;                 // +0x00（ORIG bool：movzbl;xor $0x1;test %al,%al 惯用式，WarRoom::HandleItemDrop 0x086bfad7）
    unsigned char m_field1;            // +0x01（ITEM_TYPE）
    unsigned int m_addInfo;            // +0x02（item idx；_CheckUsableExpertJobEquipment 0x084c4ba3 find_item(int) 两步 spill 需 unsigned 成员）
    // +0x06（upgrade:bit0-4 / reseal:bit5-7）。ORIG setter 汇编为位域装填形
    // （SetUpgrade 0x80f0ffc 双 and 截断、SetReSealCount and $0x7+shl $0x5、
    // IncUpgrade 读侧+装填侧双截断），标量写法不可复现（probe6/u2/r5/i3 实证
    // 真实位域成员与 ORIG 逐条全等）；union 标量别名保持外部 m_attr 使用点不变。
    union
    {
        unsigned char m_attr;
        struct { unsigned char m_attrLo:5, m_attrHi:3; } m_attrBits;
        struct { unsigned char m_attrAll:8; } m_attrAll;
    };
    int m_addInfo2;                    // +0x07（set_add_info / get_add_info）
    unsigned short m_fieldb;           // +0x0b（endurance）
    union
    {
        unsigned int m_fieldd;         // +0x0d（整体读写口径：SendUpdateItem 包字段、
                                       //      UseLimitCube 结果数、CCreatureMgr 拷贝）
                                       //      ORIG 三处 put_int(m_fieldd) 恒两段中转
                                       //      （0x08641b2d/0x0850bee2 段/0x084fd7b6 段）
                                       //      ⇒ 与 int 形参异型 = unsigned（P211；
                                       //      探针 /tmp/l6_p211d.cpp/e）
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

// Inven_Item 的 DB 兼容子集布局（setCopy 逐字段转写推导，自然对齐——
// ORIG 反汇编 m_attr@+0x08 后 m_addInfo2@+0x0c（+0x09..0x0b 填充），
// 与 pack(1) 的 Inven_Item 本体不同，故定义在 pack(1) 区外）：
#pragma pack(pop)
class Inven_Item_Compatible
{
public:
    void setCopy(const Inven_Item& item);        // ORIG _ZN21Inven_Item_Compatible7setCopyERK10Inven_Item W 0x833f38e
    void makeInvenItem(Inven_Item& item);        // ORIG _ZN21Inven_Item_Compatible13makeInvenItemER10Inven_Item W 0x833f418

    bool m_field0;              // +0x00（← item+0x00；ORIG setCopy/makeInvenItem
                                //   0x833f38e/0x833f418 首字段 movzbl+mov %dl 直拷
                                //   无 setne ⇒ bool→bool 赋值形，uchar 会生物化）
    unsigned char m_type;       // +0x01（ITEM_TYPE，← item+0x01）
    unsigned char m_field2[2];  // +0x02..0x03（填充：ORIG m_itemIdx 在 +0x04）
    // [b28c-30] m_itemIdx/m_fieldd 与目标字段同型化（Inven_Item::m_addInfo
    // +0x02 / m_fieldd +0x0d 均 unsigned int，Inven_Item.h:221/236）：ORIG
    // makeInvenItem 0x0833f418 对应两段为单段直落 `mov 0x4(%eax),%edx;
    // mov %edx,0x2(%eax)`；异型时 4.4 恒插 `mov %eax,%edx` 中转（P211 字段侧）。
    unsigned int m_itemIdx;     // +0x04（← item+0x02）
    unsigned char m_attr;       // +0x08（← Inven_Item::GetItemAttr）
    int m_addInfo2;             // +0x0c（← item+0x07）
    unsigned short m_endurance; // +0x10（← item+0x0b）
    unsigned int m_fieldd;      // +0x14（← item+0x0d，同型化同上）
    int m_amp0;                 // +0x18（← item+0x11）
    // [b28c-30] 原三标量 m_rand0/m_rand4/m_rand8 合并为具名 guid 视图（同偏移
    // +0x1c/0x20/0x24、同尺寸 0xa）：ORIG makeInvenItem 0x0833f418 尾段为单条
    // 结构赋值形（双基址各载一次 + 三连续 store），三独立字段赋值 4.4 恒逐条重载。
    stAmplifyGuidView m_guid;   // +0x1c（← item+0x15/0x19/0x1d）
};                              // 总 0x26

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
// [b28c-lane24] ORIG 尾 `leave; ret` 无 eax 设置（Ghidra C void）⇒ 返回类型 void
//（P159；返回类型不入 mangled，全部调用点均语句调用）。
void CheckItemTradeLimitCountToMoveByUser(CUser& user, int space, int slot, bool& ok);

// ORIG T 0x081201dc _Z26IsTradeLimitAttachTypeItemRK10Inven_Item：
// 交易限制/绑定类物品判定（exchange_interspace 0xc 域前置校验；定义由后续批次补）。
bool IsTradeLimitAttachTypeItem(const Inven_Item& item);

// ORIG T 0x08120242 _Z32IsTradeLimitAttachTypeItemByUserR5CUserii：
// getCurCharacR()==0 → false；GetInvenSlot(space,slot).m_addInfo==0 → false；
// 否则委托上一函数。（定义 CMailBoxHelper.cpp）
bool IsTradeLimitAttachTypeItemByUser(CUser& user, int space, int slot);
#endif  // GAME_INVEN_ITEM_H_
