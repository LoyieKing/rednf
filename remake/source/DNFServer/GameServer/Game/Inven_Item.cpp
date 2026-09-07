// df_game_r Inven_Item 还原（2026-08-16）。
// 布局/函数形态以 ORIG 反汇编为准（AE 口径：调用目标归一）。

#include "Inven_Item.h"

// ORIG 全局（0x943ddc0）：空槽模板。定义于本 TU 顶部 —— ORIG 的
// _GLOBAL__I_g_emptySlot @ 0x0850d0cb 位于 Inven_Item 内联函数群首部，
// 供 CAccountCargo/CInventory 等 extern 引用。
Inven_Item g_emptySlot;

// ===================== stAmplifyOption_t（ORIG 0x80cb75c 族） =====================

unsigned char stAmplifyOption_t::getAbilityType() const
{
    if (!hasAbility())
    {
        return 0;
    }
    if (!isIdentified())
    {
        return 0x80;
    }
    return m_abilityType;
}

unsigned short stAmplifyOption_t::getAbilityValue() const
{
    if (!hasAbility() || !isIdentified())
    {
        return 0;
    }
    return m_abilityValue;
}

void stAmplifyOption_t::reset()
{
    m_abilityType = 0;
    m_abilityValue = 0;
    m_lock = 0;
}

// ORIG W 0x0822b3d8：返回 bool = !isIdentified()（已识别→false 不改动）。
bool stAmplifyOption_t::identify()
{
    if (isIdentified())
    {
        return false;
    }
    m_abilityType &= 0x7f;
    return true;
}

bool stAmplifyOption_t::isIdentified() const
{
    // ORIG 0x081507bc：not 后 shr $0x7,%al（8 位逻辑右移，无 setne）。& 0x80 形式保持
    // 字节移位；原 (uchar)>>7 被 4.4 升格 sar$7+test+setne（N87 移位升格）。
    return ((~m_abilityType) & 0x80) != 0;
}

bool stAmplifyOption_t::hasAbility() const
{
    return m_abilityType != 0;
}

void stAmplifyOption_t::_setAbility(unsigned char type, unsigned short value,
                                    bool identified)
{
    m_abilityType = type;
    m_abilityValue = value;
    if (!identified)
    {
        m_abilityType |= 0x80;
    }
}

void stAmplifyOption_t::assign(unsigned char type, unsigned short value)
{
    m_abilityType = type;
    m_abilityValue = value;
}

void stAmplifyOption_t::SetLock(unsigned char lock)
{
    m_lock = lock;
}

unsigned char stAmplifyOption_t::GetLock() const
{
    return m_lock;
}

void stAmplifyOption_t::getValues(unsigned char& type,
                                  unsigned short& value) const
{
    type = m_abilityType;
    value = m_abilityValue;
}

// ===================== Inven_Item 内嵌辅助类 =====================

void STEnchantedSkillOption::reset()
{
    // ORIG 0x8500fe4：三次位清（&0xfffffffe, &0xfffffff9, &0x7）→ 全字节清零
    m_field = (unsigned char)(m_field & 0xfffffffe);
    m_field = (unsigned char)(m_field & 0xfffffff9);
    m_field = (unsigned char)(m_field & 0x7);
}

void RandomOptionField::reset()
{
    m_field0 = 0;
    m_field1 = 0;
    m_field2 = 0;
}

// RandomOptionSeed::reset 定义已迁 DnfItemInfo.cpp（unit-at-a-time outgoing
// 塌缩修复：本 TU 可见 callee 体使 RandomOption::reset outgoing 塌缩 0x4，
// ORIG sub $0x18；挪走后 4.4 恢复 0x18）。

void RandomOption::reset()
{
    m_field0.reset();
    m_field3.reset();
    m_field6.reset();
    m_seed9.reset();
    m_fielda.reset();
    m_seedd.reset();
}

// ORIG 0x822abd0 W：movzbl (this) → 首字节==0 返回 true。
bool RandomOption::empty()
{
    if (m_field0.m_field0 == 0)
        return true;
    return false;
}

// ORIG W 0x080cb332：构造后调 reset()（位域清零）。
UpgradeSeparateInfo::UpgradeSeparateInfo()
{
    reset();
}

void UpgradeSeparateInfo::reset()
{
    // ORIG 0x80cb346：位域 b0:5/b1:1/b2:2 清零 → 三次位清
    // （&0xffffffe0 → &0xffffffdf → &0x3f），整字节清零。
    struct Bits { unsigned char a:5, b:1, c:2; };
    reinterpret_cast<Bits*>(&m_field)->a = 0;
    reinterpret_cast<Bits*>(&m_field)->b = 0;
    reinterpret_cast<Bits*>(&m_field)->c = 0;
}

unsigned char UpgradeSeparateInfo::GetUpgradeSeparate() const
{
    return m_field & 0x1f;
}

void UpgradeSeparateInfo::SetUpgradeSeparate(unsigned char value)
{
    // ORIG 0x84e906a：位域 m_lo 装填（value 截断 and + 装填截断 and 双截断形）。
    m_bits.m_lo = value;
}

// ORIG 0x8110b0a：8 条形 = 位域 m_flag:1 读取 + unsigned char 返回（probe u1 全等）。
unsigned char UpgradeSeparateInfo::IsTradeRestriction() const
{
    return m_bits.m_flag;
}

ReservedCapacity::ReservedCapacity()
{
    reset();
}

void ReservedCapacity::reset()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}

// ===================== Inven_Item =====================
Inven_Item::Inven_Item()
{
    reset();
}

void Inven_Item::reset()
{
    m_field0 = 0;
    m_field1 = 0;
    m_attr = 0;
    m_addInfo = 0;
    m_addInfo2 = 0;
    m_fieldb = 0;
    m_fieldd = 0;
    m_amp.reset();
    m_random.reset();
    m_upgradeSep.reset();
    m_reserved.reset();
}

void Inven_Item::set_add_info(int value)
{
    m_addInfo2 = value;
}

int Inven_Item::get_add_info() const
{
    return m_addInfo2;
}

void Inven_Item::ResetItemAttr()
{
    m_attr = 0;
}

unsigned char Inven_Item::GetUpgrade() const
{
    return m_attr & 0x1f;
}

void Inven_Item::SetUpgrade(unsigned char upgrade)
{
    m_attrBits.m_attrLo = upgrade;
}

void Inven_Item::IncUpgrade()
{
    // ORIG 0x854b4be：读侧截断留在 eax、this 重载 edx（probe i4 ++ 形全等）。
    m_attrBits.m_attrLo++;
}

int Inven_Item::GetItemAttr() const
{
    return m_attrAll.m_attrAll;
}

void Inven_Item::SetItemAttr(unsigned char attr)
{
    m_attr = attr;
}

int Inven_Item::GetReSealCount() const
{
    // ORIG 0x822b456：movzbl 0x6 → shr $0x5,%al（位域 hi:3 读）
    return m_attrBits.m_attrHi;
}

void Inven_Item::SetReSealCount(unsigned char count)
{
    m_attrBits.m_attrHi = count;
}

// ORIG _Z23DecreaseTradeLimitCountR10Inven_Item T 0x0812013c：
// ORIG 块序 jne 进主流程/false 块直落 = 早退形（P16）。
// unit-at-a-time 规避（实证 /tmp/ii_half7.cpp）：本函数须置于
// GetTradeLimitCount/SetTradeLimitCount 外联定义之前，否则 4.4 删
// framePad 槽塌缩帧 0x28→0x18、count 槽 -0x9→-0x1。
bool DecreaseTradeLimitCount(Inven_Item& item)
{
    // ORIG 帧 0x28、count 槽 -0x9（count 前有 8 字节 dead 槽）；
    // 4.4 删 dead local，用被引用 double 撑回（探针 /tmp/dtlp/a.cpp 逐条同形）。
    double framePad;
    (void)&framePad;
    unsigned char count = item.GetTradeLimitCount();
    if (count == 0)
        return false;
    item.SetTradeLimitCount(count - 1);
    return true;
}

unsigned char Inven_Item::GetTradeLimitCount() const
{
    return (m_attr & 0xe0) >> 5;
}

void Inven_Item::SetTradeLimitCount(unsigned char count)
{
    m_attrBits.m_attrHi = count;
}

bool Inven_Item::isAvatarItemType() const
{
    return m_field1 == 8;
}

bool Inven_Item::IsCreatureItemType() const
{
    return m_field1 == 5 || m_field1 == 7 || m_field1 == 6;
}

int Inven_Item::isEquipableItemType() const
{
    return m_field1 == 1 || m_field1 == 5 || m_field1 == 6 ||
           m_field1 == 8;
}

bool Inven_Item::isEmpty() const
{
    if (m_addInfo == 0)
        return true;
    return false;
}

void Inven_Item::setCopy(const Inven_Item& other)
{
    m_field0 = other.m_field0;
    m_field1 = other.m_field1;
    m_addInfo = other.m_addInfo;
    m_addInfo2 = other.m_addInfo2;
    m_fieldb = other.m_fieldb;
    m_attr = other.m_attr;
    m_fieldd = other.m_fieldd;
    *(int*)((char*)&m_amp + 0) = *(const int*)((const char*)&other.m_amp + 0);
    *(int*)((char*)&m_random + 0) = *(const int*)((const char*)&other.m_random + 0);
    *(int*)((char*)&m_random + 4) = *(const int*)((const char*)&other.m_random + 4);
    *(int*)((char*)&m_random + 8) = *(const int*)((const char*)&other.m_random + 8);
    *(int*)((char*)&m_random + 12) = *(const int*)((const char*)&other.m_random + 12);
    *(int*)((char*)&m_random + 16) = *(const int*)((const char*)&other.m_random + 16);
    m_upgradeSep.m_field = other.m_upgradeSep.m_field;
    m_reserved.m_field0 = other.m_reserved.m_field0;
    m_reserved.m_field4 = other.m_reserved.m_field4;
    m_reserved.m_field8 = other.m_reserved.m_field8;
}

int Inven_Item::getPackedCode1() const
{
    // ORIG 0x850d102：全程 or %eax,mem 形需 code 为 unsigned int（probe p2 全等）。
    unsigned int code = 0;
    if (m_field0 != 0)
    {
        code |= 1;
    }
    code |= (m_field1 & 0x7) << 1;
    code |= m_addInfo << 4;
    code |= m_addInfo2 & 0xfff00000;
    return code;
}

int Inven_Item::getKey() const
{
    return m_addInfo;
}

int Inven_Item::GetInventoryType()
{
    int result;
    unsigned int u;
    if ((unsigned char)m_field1 < 0xb)
    {
        u = 1u << m_field1;
        if (u & 0x61f)
        {
            result = 1;
        }
        else if (u & 0x100)
        {
            result = 2;
        }
        else if (u & 0xe0)
        {
            result = 3;
        }
        else
        {
            result = 1;
        }
    }
    else
    {
        result = 1;
    }
    return result;
}

int Inven_Item::GetItemSpace() const
{
    int result;
    unsigned int u;
    if ((unsigned char)m_field1 < 0xb)
    {
        u = 1u << m_field1;
        if (u & 0x61f)
        {
            result = 0;
        }
        else if (u & 0x100)
        {
            result = 1;
        }
        else if (u & 0xe0)
        {
            result = 7;
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        result = 0;
    }
    return result;
}

void UpgradeSeparateInfo::SetUpgradeSeparateAndTradeRestriction(unsigned char value)
{
    // ORIG 0x085574bc (W)：*(unsigned char*)this = value —— 整字节覆盖，
    // 同时写入 upgrade 槽位（bit0-4）与 trade-restriction 位（bit5）。
    m_field = value;
}
