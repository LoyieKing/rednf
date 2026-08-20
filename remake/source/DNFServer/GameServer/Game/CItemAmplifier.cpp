#include "GameRand.h"
// df_game_r CItemAmplifier 还原（2026-08-16）。
// 构造/checkInvestableItem/generateAmplifyItem 依 ORIG 反汇编；
// stAmplifyItemScript_t / upgrade_table_t 构造为数据表占位（后续批次细化）。

#include "CItemAmplifier.h"

#include <algorithm>
#include <string.h>

#include "CItemList.h"

// ===================== 数据表占位构造（ORIG 0x82aa510 / 0x89000ca） =====================

stAmplifyItemScript_t::stAmplifyItemScript_t()
{
    memset(m_rarityRate, 0, sizeof(m_rarityRate));
    m_field3c = 0;
    memset(m_rarityProb, 0, sizeof(m_rarityProb));
}

upgrade_table_t::upgrade_table_t()
{
    memset(m_gradeLimit, 0, sizeof(m_gradeLimit));
}

// ===================== CItemAmplifier =====================

CItemAmplifier::CItemAmplifier()
{
}

bool CItemAmplifier::checkInvestableItem(const CItem* item,
                                         const Inven_Item& inven,
                                         int flag) const
{
    if (item->is_stackable())
    {
        return false;
    }

    typedef int (*GetItemTypeFn)(const CItem*);
    GetItemTypeFn getType = *(GetItemTypeFn*)(*(const char**)item + 0xc);
    if (getType(item) <= 9 || getType(item) > 0x15)
    {
        return false;
    }
    if (getType(item) == 0xb)
    {
        return false;
    }

    if (item->get_rarity() != 0 && item->get_rarity() != 1)
    {
        return false;
    }

    CEquipItem* equip = (CEquipItem*)item;
    if (!equip)
    {
        return false;
    }
    if ((int)inven.m_fieldb < equip->get_endurance())
    {
        return false;
    }

    if (flag != 0)
    {
        if (equip->isExistUpgradableLevelTag())
        {
            if (flag <= 0x36)
            {
                return false;
            }
        }
        else if (item->getUsableLevel() <= 0x36)
        {
            return false;
        }
    }

    int grade = item->get_grade();
    int rarity = item->get_rarity();
    int base = m_upgrade.m_gradeLimit[rarity];
    int diff = grade - base;
    return std::max(diff, 0) >= m_script.m_field3c;
}

void CItemAmplifier::generateAmplifyItem(const CItem* item,
                                         Inven_Item& inven) const
{
    ENUM_AMPLIFY_TYPE type = ENUM_AMPLIFY_TYPE_NONE;
    unsigned short value = 0;

    _generateOption(item, type, value);
    if (type == ENUM_AMPLIFY_TYPE_NONE || value == 0)
    {
        return;
    }

    typedef bool (*CheckFn)(const CItem*);
    CheckFn check = *(CheckFn*)(*(const char**)item + 0x58);
    if (check(item))
    {
        inven.m_amp._setAbility((unsigned char)type, value, true);
    }
    else
    {
        inven.m_amp._setAbility((unsigned char)type, value, false);
    }
}

void CItemAmplifier::_generateOption(const CItem* item,
                                     ENUM_AMPLIFY_TYPE& type,
                                     unsigned short& value) const
{
    int rarity = item->get_rarity();
    if (rarity < 0 || rarity > 5)
    {
        type = ENUM_AMPLIFY_TYPE_NONE;
        value = 0;
        return;
    }

    int rand = get_rand_int(0x2710);
    if (m_script.m_rarityProb[rarity] <= rand)
    {
        type = ENUM_AMPLIFY_TYPE_NONE;
        value = 0;
        return;
    }

    int rand2 = get_rand_int(0x2710);
    std::vector<stAmplifyOptionInfo_t>::const_iterator it =
        m_script.m_options.begin();
    std::vector<stAmplifyOptionInfo_t>::const_iterator end =
        m_script.m_options.end();
    while (it != end)
    {
        if (it->m_prob > rand2)
        {
            type = (ENUM_AMPLIFY_TYPE)it->m_type;
            value = (unsigned short)((float)it->m_value *
                                     m_script.m_rarityRate[rarity]);
            return;
        }
        ++it;
    }
}
