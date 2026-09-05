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
    : m_machineFrames(0xa)   // ORIG C1 0x089000ca：_Hashtable 桶数 0xa
{
    int i;
    for (i = 0; i <= 3; ++i)          // +0x0c..：1.0f
        floatAt(i * 4 + 0xc) = 1.0f;
    for (i = 0; i <= 0x15; ++i)       // +0x1c..：1.0f
        floatAt((i + 4) * 4 + 0xc) = 1.0f;
    for (i = 0; i <= 5; ++i)
    {
        floatAt((i + 0x1c) * 4 + 4) = 1.0f;    // ORIG 0x089000ca：+0x74..
        floatAt((i + 0x20) * 4 + 0xc) = 1.0f;  // +0x8c..
    }
    for (i = 0; i <= 0x63; ++i)       // ORIG +0xa4..：整数 1
        intAt((i + 0x28) * 4 + 4) = 1;
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
    memset(m_zero4b4, 0, sizeof(m_zero4b4));
}

// ===================== CItemAmplifier =====================

CItemAmplifier::CItemAmplifier()
{
}

// 【batch-11 残差标注】o=139 n=134，调用点差集已归零（direct 9/9、indirect 3/3）：
//   +0x25c 表项按证据建真实成员 m_baseGrade[6]（消除 1 个 floatAt call 型 EXTRA）。
//   余 5 insn = ORIG 对表址/结果地址的重复物化（非语义缺口）；实测把 std::max
//   改显式 if 反而更差（139/130），故保留模板形。
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
    // ORIG 直读 `mov 0x25c(%eax,%edx,4)`（+0x25c 具名视图，无 cvt 无 call）；
    // 走 inline 访问器 floatAt() 在 -O0 会额外发一次 call（+5 insn）。
    int base = m_upgrade.m_bodyView.m_named.m_baseGrade[rarity];
    int diff = grade - base;
    // 钳零保持 std::max 形：实测显式 if 版反而少 4 insn（139/130），
    // std::max 版 139/134 更接近（余差集中在 ORIG 的地址重复物化）。
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
    // ORIG 0x08514f76..0x08514fa3：范围守卫**两次独立 get_rarity**（0x08514f7c
    // 与 0x08514f8b）+ `mov$1/mov$0` 汇聚岛 + `test%al; je` ⇒ 用命名 bool 物化、
    // 且两臂各重调（P84/P48/P183）；写成 `int rarity` 缓存会让 4.4 只发 1 次
    // call 并直接 cmp/jcc（实测少 3 call / 15 insn）。
    const bool outOfRange = (item->get_rarity() < 0 || item->get_rarity() > 5);
    if (outOfRange)
    {
        type = ENUM_AMPLIFY_TYPE_NONE;
        value = 0;
        return;
    }

    int rand = get_rand_int(0x2710);
    if (m_script.m_rarityProb[item->get_rarity()] <= rand)
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
            // ORIG 0x0851508x：先 `fildl; fstps -0x24` 把 m_value 落成 float 槽，
            // 再**第三次** get_rarity → `flds m_rarityRate[r]`（被乘数在前）→
            // `fmuls -0x24` → 控制字 ROUND → `fistpl`。故 rarityRate 作左操作数、
            // m_value 走具名 float 局部（P176 ROUND 辨伪 + P183 重物化）。
            const float baseValue = (float)it->m_value;
            value = (unsigned short)(
                m_script.m_rarityRate[item->get_rarity()] * baseValue);
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
    // ORIG 0x08234d64..0x08234e1b 逐条转写（无 md 报告，二进制切片）：
    //   ① 表址 = this + ((type*4+type)*4 + 0xa0) + 0xc（无边界检查，P140）；
    //   ② hash_map::end() **两次**（-0x1c 先作 it 初值、-0x14 作 last），
    //      find 结果以 POD 双 dword 覆盖 -0x1c（4.1 迭代器赋值发形）；
    //   ③ it == last → 0；
    //   ④ 命中段**逐成员**赋值：两次 _Hashtable_const_iterator::operator->
    //      分别取 pair+4 / pair+8（⇒ `info = it->second` 整体拷贝会丢一次
    //      operator-> 并改走结构复制路径）。
    const __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>&
        table = m_script.m_investMap[(int)type];
    __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>::const_iterator it =
        table.end();
    const __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>::const_iterator
        last = table.end();
    it = table.find(level);
    if (it == last)
    {
        return 0;
    }
    info.m_amount = it->second.m_amount;
    info.m_value = it->second.m_value;
    return 1;
}

// ORIG 0x08234c3c：返回 ENUM_ERROR 码（0 = 允许）。
//   0:  item 无能力且未强化 → 0；有强化 → 0x12；有(已识别)能力 → 0x14
//   1:  有能力 → (有强化 → 0x12；否则 0)；无能力 → 0x15
//   2:  恒 0
//   其他（>2）: 恒 1
int CItemAmplifier::enableInvestAmplifyOption(
    const Inven_Item& inven,
    ENUM_ITEM_AMPLIFY_INVEST_TYPE type) const
{
    const stAmplifyOption_t& amp = inven.m_amp;

    if (type == ENUM_ITEM_AMPLIFY_INVEST_TYPE_1)
    {
        if (amp.hasAbility())
        {
            if (inven.GetUpgrade())
            {
                return 0x12;
            }
            return 0;
        }
        return 0x15;
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

// 【batch-11 残差标注 · 待访问器专项批】o=371 n=365：本函数按**脚本列号变量**
//   偏移访问 +0x00c 混合表，重建侧走 inline 成员访问器 floatAt/intAt（-O0 每点
//   额外 1 call，实测 EXTRA 11 个 + 1 memcpy），ORIG 侧是 `flds off(%eax,%edx,4)`
//   直访。访问器全域宏化/字段化需同时改 ScriptLeafA2::importUpgradeTable 十余
//   写点且横跨 13 TU ⇒ Main 裁决延至 batch-12 单派专项，本批不改形。
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
    // 拷贝 m_infos.at(upgrade) 到 out（0x6c = 27 dwords；ORIG 用 at 带越界检查）。
    memcpy(out, &m_infos.at(upgrade), sizeof(upgrade_info_t));

    float gain = floatAt((rarity + 0x20) * 4 + 0xc);
    out->m_prob00[0] = 0;  // 占位：浮点区逐项缩放（见 floatAt 访问）
    // 前 0x4 字节概率/等级混合区按 float 偏移缩放。
    float* p = (float*)out->m_prob00;
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
    // ORIG 0x8900436：两段系数均按 float 解读（flds，floatAt），非 int。
    int disc = (int)f[0xd];
    disc = (int)(-100.0f + floatAt((equipType + 4) * 4 + 0xc) * (float)disc);
    disc = (int)(-100.0f + floatAt((rarity + 0x1c) * 4 + 4) * (float)disc);
    out->m_discountBase = disc;
    // ORIG：equipType 钳制到 0x12 后查 m_floatMap(+0x49c) 折扣，命中则覆盖。
    int et = equipType > 0x12 ? 0x12 : equipType;
    std::map<int, float>::const_iterator it = m_floatMap.find(et);
    if (it != m_floatMap.end())
        out->m_discountBase = static_cast<int>(it->second * (float)disc);
    return 1;
}

//   返回 3 或 upgrade_info +0x24 惩罚类型字段（importUpgradeTable 的
//   infoInt(info,0x24) 脚本导入值；ORIG 反汇编 mov 0x24(%eax),%eax 实证，
//   非 m_discountBase(+0x34)）。
// ============================================================================
int upgrade_table_t::GetPenaltyType(unsigned char upgrade, ENUM_RARITY rarity,
                                    const upgrade_info_t* info) const
{
    int th = intAtConst((rarity + 0x100) * 4 + 4);
    if (th == -1 || (int)upgrade < th)
        return *(const int*)((const char*)info + 0x24);
    return 3;
}
