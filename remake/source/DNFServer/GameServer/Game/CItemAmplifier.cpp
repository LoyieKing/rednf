#include "GameRand.h"
// df_game_r CItemAmplifier 还原（2026-08-16）。
// 构造/checkInvestableItem/generateAmplifyItem 依 ORIG 反汇编；
// stAmplifyItemScript_t / upgrade_table_t 构造为数据表占位（后续批次细化）。

#include "CItemAmplifier.h"

#include <algorithm>
#include <string.h>

#include "CItemList.h"

// ===================== 数据表占位构造（ORIG 0x82aa510 / 0x89000ca） =====================

// ORIG 0x0822aea6：+0x00 m_amount / +0x04 m_value 均清 0。
stInvestAmplifyOption_t::stInvestAmplifyOption_t()
{
    m_amount = 0;
    m_value = 0;
}

stAmplifyItemScript_t::stAmplifyItemScript_t()
{
    // ORIG 0x82aa510：仅构造 map/vector/hash_map 容器，数据区不初始化
}

// ORIG 0x089000ca：按偏移初始化概率/等级混合表。
upgrade_table_t::upgrade_table_t()
{
    int i;
    for (i = 0; i <= 3; ++i)          // +0x0c..：1.0f
        floatAt(i * 4 + 0xc) = 1.0f;
    for (i = 0; i <= 0x15; ++i)       // +0x1c..：1.0f
        floatAt((i + 4) * 4 + 0xc) = 1.0f;
    for (i = 0; i <= 5; ++i)
    {
        floatAt((i + 7) * 4 + 4) = 1.0f;    // +0x20..
        floatAt((i + 8) * 4 + 0xc) = 1.0f;  // +0x28..
    }
    for (i = 0; i <= 0x63; ++i)       // +0xa4..：整数 1
        intAt((i + 10) * 4 + 4) = 1;
    intAt(0x234) = 0;
    for (i = 0; i <= 0x63; ++i)       // +0x274..：1.0f
        floatAt((i + 0x9c) * 4 + 4) = 1.0f;
    for (i = 0; i <= 5; ++i)
    {
        intAt((i + 0x100) * 4 + 4) = -1;    // +0x404..
        intAt((i + 0x104) * 4 + 0xc) = -1;  // +0x41c..
    }
    intAt(0x238) = -1;
    intAt(0x23c) = 0;
    for (i = 0; i <= 5; ++i)
    {
        intAt((i + 0x90) * 4) = 0;          // +0x240..
        intAt((i + 0x94) * 4 + 0xc) = 0;    // +0x25c..
    }
    intAt(0x258) = 0;

    m_floatMap.clear();
    m_machineFrames.clear();
    m_machineFrames.resize(0xa);   // ORIG _Hashtable 桶数 0xa（C1 反汇编）
    memset(m_zero4b4, 0, sizeof(m_zero4b4));
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
    int base = (int)m_upgrade.floatAt(rarity * 4 + 0x25c);
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


// ===================== 净化材料查询（ORIG W 0x08234d20 / 0x08234cd4 / 0x08234cfa） =====================
// 两行净化材料在 stAmplifyItemScript_t 中连续布局：
//   type 0 = m_purifyMatItem(+0x9c)/m_purifyMatCount(+0xa4)（[purify material]）
//   type 1 = m_purifyOnlyMatItem(+0xa0)/m_purifyOnlyMatCount(+0xa8)（[purify only material]）
// 以具名字段首地址的连续视图访问，等价于 ORIG 的 *(int*)(amp + 0x9c + i*4) 表查找。

// ORIG 0x08234d20：i∈[0,1]，表项 == itemIdx 则返回 i；否则 -1。
int CItemAmplifier::getPurifyMaterialType(int itemIdx) const
{
    for (int i = 0; i <= 1; ++i)
    {
        if ((&m_script.m_purifyMatItem)[i] == itemIdx)  // +0x9c，与 m_purifyOnlyMatItem 连续
        {
            return i;
        }
    }
    return -1;
}

// ORIG 0x08234cd4：type∈[0,1] → *(int*)(amp + 0x9c + type*4)；越界返回 0。
int CItemAmplifier::getPurifyMaterialItemIdx(int type) const
{
    if (type >= 0 && type <= 1)
    {
        return (&m_script.m_purifyMatItem)[type];
    }
    return 0;
}

// ORIG 0x08234cfa：type∈[0,1] → *(int*)(amp + 0xa4 + type*4)；越界返回 0。
int CItemAmplifier::getPurifyMaterialItemCount(int type) const
{
    if (type >= 0 && type <= 1)
    {
        return (&m_script.m_purifyMatCount)[type];
    }
    return 0;
}

// ===================== 强化投资（ORIG 0x08234d5e / 0x08234c3c / 0x08234b30） =====================

// ORIG 0x08234d5e：按 type 索引 m_script.m_investMap[type]，查找 level。
// 命中则把 value.{m_amount,m_value} 写入 info，返回 1；未命中返回 0。
int CItemAmplifier::getInvestMaterialInfo(
    ENUM_ITEM_AMPLIFY_INVEST_TYPE type,
    unsigned long level,
    stInvestAmplifyOption_t& info) const
{
    if (type < ENUM_ITEM_AMPLIFY_INVEST_NONE ||
        type > ENUM_ITEM_AMPLIFY_INVEST_TYPE_2)
    {
        return 0;
    }

    const __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>&
        table = m_script.m_investMap[(int)type];
    __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>::const_iterator it =
        table.find(level);
    if (it == table.end())
    {
        return 0;
    }

    info = it->second;
    return 1;
}

// ORIG 0x08234c3c：返回 ENUM_ERROR 码（0 = 允许）。
//   0:  item 无能力且未强化 → 0；有强化 → 0x12；有(已识别)能力 → 0x14
//   1:  无能力 → 0x15；有强化 → 0x12；否则 → 0
//   2:  恒 0
//   其他（>2）: 恒 1
int CItemAmplifier::enableInvestAmplifyOption(
    const Inven_Item& inven,
    ENUM_ITEM_AMPLIFY_INVEST_TYPE type) const
{
    const stAmplifyOption_t& amp = inven.m_amp;

    if (type == ENUM_ITEM_AMPLIFY_INVEST_TYPE_1)
    {
        if (!amp.hasAbility())
        {
            return 0x15;
        }
        if (inven.GetUpgrade())
        {
            return 0x12;
        }
        return 0;
    }
    if (type == ENUM_ITEM_AMPLIFY_INVEST_TYPE_2)
    {
        return 0;
    }
    if (type != ENUM_ITEM_AMPLIFY_INVEST_NONE)
    {
        return 1;
    }

    if (amp.hasAbility())
    {
        return 0x14;
    }
    if (inven.GetUpgrade())
    {
        return 0x12;
    }
    return 0;
}

// ORIG 0x08234b30：在 m_script.m_options 中匹配 option.m_amount 类型的选项
// （stAmplifyOptionInfo_t::m_type == option.m_amount；option.m_value 不参与，
// 命中项自身的 +0x06 m_value 才是基数），命中则按 rarity 缩放并写
// inven.m_amp._setAbility(type, value, identified=true)。
void CItemAmplifier::investAmplifyOption(
    const CItem* item,
    Inven_Item& inven,
    const stInvestAmplifyOption_t& option) const
{
    if (option.m_amount == 0)
    {
        return;
    }

    std::vector<stAmplifyOptionInfo_t>::const_iterator it =
        m_script.m_options.begin();
    std::vector<stAmplifyOptionInfo_t>::const_iterator end =
        m_script.m_options.end();
    for (; it != end; ++it)
    {
        if (it->m_type != option.m_amount)
        {
            continue;
        }

        unsigned short value =
            (unsigned short)((float)it->m_value *
                             m_script.m_rarityRate[item->get_rarity()]);
        unsigned char type = (unsigned char)option.m_amount;
        inven.m_amp._setAbility(type, value, true);
        return;
    }
}

// ============================================================================
// upgrade_table_t::get_upgrade_info @ 0x8900436
//   按 rarity/growthGrade/upgrade/equipType 计算 upgrade_info 到 out。
//   返回 0=越界/失败, 1=成功。
// ============================================================================

int upgrade_table_t::get_upgrade_info(upgrade_info_t* out,
                                      unsigned char rarity,
                                      unsigned char growthGrade,
                                      unsigned char upgrade,
                                      ENUM_EQUIPMENTTYPE equipType) const
{
    if (upgrade >= m_infos.size())
        return 0;
    if (rarity >= 6)
        return 0;
    if (equipType >= 0x16 || equipType < 10)
        return 0;
    if (growthGrade >= 100)
        return 0;

    float bias = 2.0f * 0.5f;  // 常量区：two(0x8c9d?) * pointFive
    // 拷贝 m_infos[upgrade] 到 out（0x6c = 27 dwords）。
    memcpy(out, &m_infos[upgrade], sizeof(upgrade_info_t));

    float gain = floatAt((rarity + 0x20) * 4 + 0xc);
    out->m_pad0[0] = 0;  // 占位：浮点区逐项缩放（见 floatAt 访问）
    // 前 0x4 字节概率/等级混合区按 float 偏移缩放。
    float* p = (float*)out->m_pad0;
    // param_1[0..7] 与 param_1[0x13..0x1a] 各有系数缩放，这里按 ORIG 逐项到
    // 0x6c 内 float 数组（0/1/2/3 用 +0xc/0x10/0x14/0x18 分母，4..7 直接乘）。
    float* f = (float*)out;
    f[0] = gain * f[0] * ((float)growthGrade / floatAt(0xc) + bias);
    f[1] = gain * f[1] * ((float)growthGrade / floatAt(0x10) + bias);
    f[2] = gain * f[2] * ((float)growthGrade / floatAt(0x14) + bias);
    f[3] = gain * f[3] * ((float)growthGrade / floatAt(0x18) + bias);
    f[4] = gain * f[4];
    f[5] = gain * f[5];
    f[6] = gain * f[6];
    f[7] = gain * f[7];
    f[0x13] = gain * f[0x13] * ((float)growthGrade / floatAt(0xc) + bias);
    f[0x14] = gain * f[0x14] * ((float)growthGrade / floatAt(0x10) + bias);
    f[0x15] = gain * f[0x15] * ((float)growthGrade / floatAt(0x14) + bias);
    f[0x16] = gain * f[0x16] * ((float)growthGrade / floatAt(0x18) + bias);
    f[0x17] = gain * f[0x17];
    f[0x18] = gain * f[0x18];
    f[0x19] = gain * f[0x19];
    f[0x1a] = gain * f[0x1a];
    f[0xd] = (float)intAtConst((growthGrade + 0x28) * 4 + 4);
    // 后续常数 + 等级系数 + map 折扣调整。
    int disc = (int)f[0xd];
    disc = (int)(-100.0f + intAtConst((equipType + 4) * 4 + 0xc) * (float)disc);
    disc = (int)(-100.0f + intAtConst((rarity + 0x1c) * 4 + 4) * (float)disc);
    out->m_discountBase = disc;
    return 1;
}

// ============================================================================
// upgrade_table_t::GetPenaltyType @ 0x89003ee
//   返回 3 或 upgrade_info.m_discountBase 编码值。
// ============================================================================
int upgrade_table_t::GetPenaltyType(unsigned char upgrade, ENUM_RARITY rarity,
                                    const upgrade_info_t* info) const
{
    int th = intAtConst((rarity + 0x100) * 4 + 4);
    if (th == -1 || (int)upgrade < th)
        return info->m_discountBase;
    return 3;
}
