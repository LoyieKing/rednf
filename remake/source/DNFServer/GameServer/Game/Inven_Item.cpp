// df_game_r Inven_Item 还原（2026-08-16）。
// 布局/函数形态以 ORIG 反汇编为准（AE 口径：调用目标归一）。

#include "Inven_Item.h"

// ORIG 全局（0x943ddc0）：空槽模板。定义于本 TU 顶部 —— ORIG 的
// _GLOBAL__I_g_emptySlot @ 0x0850d0cb 位于 Inven_Item 内联函数群首部，
// 供 CAccountCargo/CInventory 等 extern 引用。
Inven_Item g_emptySlot;

// ===================== stAmplifyOption_t（ORIG 0x80cb75c 族） =====================

void stAmplifyOption_t::reset()
{
    m_abilityType = 0;
    m_abilityValue = 0;
    m_lock = 0;
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

void RandomOptionSeed::reset()
{
    m_field = 0;
}

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

void ChronicleRandomOptionInfo::reset()
{
    m_skill0.reset();
    m_skill1.reset();
    m_field2 = 0;
    m_random.reset();
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
    // ORIG 0x84e906a：低 5 位写入 value & 0x1f，保留高 3 位（bit5=IsTradeRestriction）
    m_field = (unsigned char)((m_field & 0xe0) | (value & 0x1f));
}

// ORIG `shr $5;and $1` 直返 0/1（无 setne）；bool 返回致 `and $0x20;test;setne`。
// 改 int 返回类型虽可闭合但波及全部调用点头声明，风险>收益；按 P197④ setne 物化 N-cap 记。
bool UpgradeSeparateInfo::IsTradeRestriction() const
{
    return (m_field >> 5) & 1;
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
    m_attr = (unsigned char)((m_attr & 0xe0) | (upgrade & 0x1f));
}

void Inven_Item::IncUpgrade()
{
    m_attr = (unsigned char)((m_attr & 0xe0) | (((m_attr & 0x1f) + 1) & 0x1f));
}

unsigned char Inven_Item::GetItemAttr() const
{
    return m_attr;
}

void Inven_Item::SetItemAttr(unsigned char attr)
{
    m_attr = attr;
}

unsigned char Inven_Item::GetReSealCount() const
{
    return (m_attr & 0xe0) >> 5;
}

void Inven_Item::SetReSealCount(unsigned char count)
{
    m_attr = (unsigned char)((m_attr & 0x1f) | ((count & 0x7) << 5));
}

unsigned char Inven_Item::GetTradeLimitCount() const
{
    return (m_attr & 0xe0) >> 5;
}

void Inven_Item::SetTradeLimitCount(unsigned char count)
{
    m_attr = (unsigned char)((m_attr & 0x1f) | ((count & 0x7) << 5));
}

// ORIG _Z23DecreaseTradeLimitCountR10Inven_Item T 0x0812013c：
// cVar1 = GetTradeLimitCount()；非 0 时 SetTradeLimitCount(cVar1-1)，返回 cVar1!=0。
bool DecreaseTradeLimitCount(Inven_Item& item)
{
    unsigned char count = item.GetTradeLimitCount();
    if (count != 0)
    {
        item.SetTradeLimitCount(count - 1);
        return true;
    }
    return false;
}

bool Inven_Item::isAvatarItemType() const
{
    return m_field1 == 8;
}

bool Inven_Item::IsCreatureItemType() const
{
    return m_field1 == 5 || m_field1 == 7 || m_field1 == 6;
}

bool Inven_Item::isEquipableItemType() const
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
    int code = 0;
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
