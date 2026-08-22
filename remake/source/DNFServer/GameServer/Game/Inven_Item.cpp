// df_game_r Inven_Item 还原（2026-08-16）。
// 布局/函数形态以 ORIG 反汇编为准（AE 口径：调用目标归一）。

#include "Inven_Item.h"

// ===================== stAmplifyOption_t（ORIG 0x80cb75c 族） =====================

void stAmplifyOption_t::reset()
{
    m_abilityType = 0;
    m_abilityValue = 0;
    m_lock = 0;
}

bool stAmplifyOption_t::isIdentified() const
{
    return (unsigned char)(~m_abilityType) >> 7;
}

bool stAmplifyOption_t::hasAbility() const
{
    return m_abilityType != 0;
}

void stAmplifyOption_t::identify()
{
    if (isIdentified())
    {
        return;
    }
    m_abilityType &= 0x7f;
}

char stAmplifyOption_t::getAbilityType() const
{
    if (!hasAbility())
    {
        return 0;
    }
    if (!isIdentified())
    {
        return (char)0x80;
    }
    return (char)m_abilityType;
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
    m_field = 0;
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

void ChronicleRandomOptionInfo::reset()
{
    m_skill0.reset();
    m_skill1.reset();
    m_field2 = 0;
    m_random.reset();
}

UpgradeSeparateInfo::UpgradeSeparateInfo()
{
    reset();
}

void UpgradeSeparateInfo::reset()
{
    m_field = 0;
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
    return m_attr >> 5;
}

void Inven_Item::SetReSealCount(unsigned char count)
{
    m_attr = (unsigned char)((m_attr & 0x1f) | ((count & 0x7) << 5));
}

unsigned char Inven_Item::GetTradeLimitCount() const
{
    return m_attr >> 5;
}

void Inven_Item::SetTradeLimitCount(unsigned char count)
{
    m_attr = (unsigned char)((m_attr & 0x1f) | ((count & 0x7) << 5));
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
    return m_addInfo == 0;
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
