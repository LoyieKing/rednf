// ============================================================================
// RandomRentalA2.cpp — 随机选项 reset/RegenVer + RegenerationROI 再铸链（独占 TU）
//
// 全部函数经 df_game_r nm 核实；有报告者逐条对照
// docs/class_func_reports/<类>/<函数>.md 的 ## 汇编 + ## 反编译 C 完整转写；
// 无报告者（模板内部函数/键类型/访问器）以 objdump 直转并逐处标注"报告缺失"。
//
// 本 TU 独家提供（勿在他处重复定义）：
//   random_option::CRandomOptionItemHandle::
//     reset_option                 T 0x085f39d2（reset_option.md）
//     give_option                  T 0x085f2cc6（give_option.md）
//     RegenVer_give_option         T 0x085f1bb0（RegenVer_give_option.md）
//     RegenVer_choose_item         T 0x085f1802（RegenVer_choose_item.md）
//     RegenVer_get_option_quantity T 0x085f1a62（同名报告）
//     _get_level_limit             T 0x085f0f9a（同名报告）
//     _get_part_type               T 0x085f1088（同名报告）
//     _get_part_type_by_ItemGroup  T 0x085f11a4（同名报告）
//     _get_base_item               T 0x085f1426（同名报告）
//     _get_option_type             T 0x085f2338（同名报告）
//     _get_option_quantity         T 0x085f25b0（同名报告）
//     _choose_option_group         T 0x085f2882（同名报告）
//     _get_option_value            T 0x085f2b02（同名报告）
//   RegenerationROI::
//     Regenerate                   T 0x085f8646（Regenerate.md）
//     SendMessageToCUser           T 0x085f853c（报告缺失，objdump 直转）
//     _Select_Part                 T 0x085f8ec2（报告缺失，objdump 直转）
//     _Calculate_RegenCost         T 0x085f9094（同名报告）
//     _RandomLevel                 T 0x085f93cc（报告缺失，objdump 直转）
//     RegenVer_get_option_quantity T 0x085f8cb4（同名报告）
//     RegenVer_get_option_value    T 0x085f8da6（同名报告）
//   select_base_item C1            W 0x085f97ac（报告缺失，objdump 直转）
//   RandomOption::{change_option,get_option_index,set_first_value,
//                  set_second_value} + RandomOptionField::empty
//                                  W 0x85f4002/4040/4078/40be + 0x811ed22
//                                  （报告缺失，objdump 直转）
//   DifferentWeightValue::DifferentWeightValue W 0x085f419a（报告缺失，objdump）
//   CRandomList<unsigned> / CRandomListEx 模板（实例化符号与 ORIG W 对应；
//     成员体报告缺失，objdump 直转：C1 0x85f4904、get_value 0x85f49a0、
//     op()(uint) 0x85f5b6a、op()(ResetValue) 0x85f5e9e、op()(OptionGroup)
//     0x85f58d2；Ex: C1 0x85f4da6、op() 0x85f5d04、get_value 0x85f4e5a）
//
// 运行期依赖（链接不受影响，明确标注）：
//   - importRandomOptionScript（ORIG T 0x08a70a90，未实现）：脚本容器与
//     script+0x148 provider 对象为空 → handle 经 m_optionSet 的虚调用在脚本
//     装载批次完成前不可用（运行期 blocked 于该批次）。
// ============================================================================

#include <sstream>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "PvPTypes.h"            // CMTRand 完整定义（randInt(const unsigned long&)）
#include "CDataManager.h"        // RegenerationROI 权威布局 / G_CDataManager / find_item
#include "RandomOptionScript.h"  // GetRandomOption 宿主容器 + 全部嵌套类型
#include "CItemGenerator.h"      // random_option::CRandomOptionItemHandle 布局镜像（本批扩写）
#include "CItemAmplifier.h"      // ENUM_RARITY
#include "Inven_Item.h"          // RandomOption / ENUM_RANDOM_OPTION_NUMBER
#include "CUser.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "CCubeStatistic.h"      // getItemGroupNameTable()
#include "GameTypes.h"           // cMyTrace

namespace random_option
{
void* GetRandomOption();   // GlobalData.cpp 定义（T 0x085f0eb0：&G_CDataManager()->m_randomOption）
}  // namespace random_option

using random_option::GetRandomOption;

// ---------------------------------------------------------------------------
// select_base_item —— W ctor 0x085f97ac（报告缺失，objdump 直转）
// ctor 清零 +0/+4/+c/+10/+14；+0x08 不初始化（忠实保留）。
// ---------------------------------------------------------------------------
struct select_base_item
{
    select_base_item();

    int m_level0;    // +0x00（选定等级；Regenerate→RegenVer_choose_item 的 level 参）
    int m_field4;    // +0x04
    int m_field8;    // +0x08（ORIG ctor 不初始化）
    int m_fieldC;    // +0x0c
    int m_field10;   // +0x10
    int m_field14;   // +0x14
};  // sizeof 0x18

select_base_item::select_base_item()
{
    m_level0 = 0;
    m_field4 = 0;
    /* m_field8 ORIG 不清零 */
    m_fieldC = 0;
    m_field10 = 0;
    m_field14 = 0;
}

// ---------------------------------------------------------------------------
// RandomOption 访问器族（报告缺失，objdump 直转；W 符号实证）
// ---------------------------------------------------------------------------

bool RandomOptionField::empty() const
{
    return m_field0 == 0;  // W 0x811ed22：movzbl (%eax); test; sete
}

bool RandomOption::change_option(ENUM_RANDOM_OPTION_NUMBER num)
{
    // W 0x85f4002：!m_fielda.empty() && (m_seedd & 3) == num
    if (m_fielda.empty())
    {
        return false;
    }
    return (unsigned char)(m_seedd.m_field & 3) == (unsigned char)num;
}

unsigned char RandomOption::get_option_index(ENUM_RANDOM_OPTION_NUMBER num)
{
    // W 0x85f4040：change_option ? this[0xa] : this[num*3+0]
    if (change_option(num))
    {
        return m_fielda.m_field0;
    }
    const unsigned char* self = reinterpret_cast<const unsigned char*>(this);
    return self[(int)num * 3];
}

void RandomOption::set_first_value(ENUM_RANDOM_OPTION_NUMBER num, unsigned char v)
{
    // W 0x85f4078：change_option ? this[0xb]=v；恒写 this[num*3+1]=v
    if (change_option(num))
    {
        m_fielda.m_field1 = v;
    }
    unsigned char* self = reinterpret_cast<unsigned char*>(this);
    self[(int)num * 3 + 1] = v;
}

void RandomOption::set_second_value(ENUM_RANDOM_OPTION_NUMBER num, unsigned char v)
{
    // W 0x85f40be：change_option ? this[0xc]=v；恒写 this[num*3+2]=v
    if (change_option(num))
    {
        m_fielda.m_field2 = v;
    }
    unsigned char* self = reinterpret_cast<unsigned char*>(this);
    self[(int)num * 3 + 2] = v;
}

DifferentWeightValue::DifferentWeightValue()
{
    // W 0x085f419a：双字清零（报告缺失，objdump 直转）
    m_field0 = 0;
    m_field4 = 0;
}

// ---------------------------------------------------------------------------
// CRandomList<unsigned int> —— 16 字节 functor（报告缺失，objdump 直转）
// 语义：ctor(n) 设随机目标 n；operator()(w) 在累计和未越过 n 时，
//   若 n-累计和 < w 则记下当前序号；随后累计和 += w、计数 ++。
// ---------------------------------------------------------------------------
template <typename T>
class CRandomList
{
public:
    explicit CRandomList(unsigned int n)
        : m_value(0), m_n(n), m_sum(0), m_count(0)
    {
    }

    // ORIG _ZN11CRandomListIjEclEj W @0x085f5b6a（36 条，报告缺失 objdump 直转）：
    //   m_sum > m_n 直接结束；否则 (m_n-m_sum)<w 时记 m_value=m_count，
    //   随后 m_sum+=w、++m_count。
    void operator()(unsigned int weight)
    {
        if (m_sum > m_n)
        {
            return;
        }
        if (m_n - m_sum < weight)
        {
            m_value = m_count;
        }
        m_sum += weight;
        ++m_count;
    }
    // ORIG _ZN11CRandomListIjEclE10ResetValue（W 0x085f5e9e）：
    //   参数按值（12 字节 ResetValue，mangled 无 RK 前缀）；取 m_weight8 作权重。
    // ORIG _ZN11CRandomListIjEclE11OptionGroup（W 0x085f58d2）：
    //   参数按值（8 字节 OptionGroup）；取 m_weight4 作权重。
    void operator()(ResetValue rv)
    {
        (*this)(static_cast<unsigned int>(rv.m_weight8));
    }
    void operator()(OptionGroup og)
    {
        (*this)(static_cast<unsigned int>(og.m_weight4));
    }

    // ORIG _ZN11CRandomListIjE9get_valueEv W @0x085f49a0（非 const）
    unsigned int get_value() { return m_value; }

private:
    unsigned int m_value;   // +0x00
    unsigned int m_n;       // +0x04
    unsigned int m_sum;     // +0x08
    unsigned int m_count;   // +0x0c
};


template class CRandomList<unsigned int>;

// ORIG W 符号（objdump 0x85f4da6..0x85f4e7a 直转）：
//   C1 0x85f4da6：先 call OptionValueRatio::C1（三字清零），再写 +0xc/+0x10/+0x14
//     （m_n=n、m_sum=0、m_count=0；非 memset）。
//   op()(OptionValueRatio) 0x85f5d04：参数按值（EclES0_）；m_sum>m_n 即返回；
//     否则 (m_n-m_sum)<o.m_weight8 时整拷 o 三字入 m_sel；随后仅 m_sum+=o.m_weight8，
//     无 ++m_count。
//   get_value 0x85f4e5a：非 const（E9get_valueEv），直拷 12 字节，无局部帧。
// ---------------------------------------------------------------------------
template <typename T>
class CRandomListEx
{
public:
    explicit CRandomListEx(unsigned int n) : m_sel(), m_n(n), m_sum(0), m_count(0)
    {
    }

    void operator()(T o)
    {
        if (m_sum > m_n)
        {
            return;
        }
        if (m_n - m_sum < o.m_weight8)
        {
            m_sel = o;
        }
        m_sum += o.m_weight8;
    }

    T get_value() { return m_sel; }

private:
    T m_sel;              // +0x00
    unsigned int m_n;     // +0x0c
    unsigned int m_sum;   // +0x10
    unsigned int m_count; // +0x14
};

template class CRandomListEx<OptionValueRatio>;

// ===========================================================================
// random_option::CRandomOptionItemHandle 成员
// （m_rand 为 new CMTRand[13]；ORIG 以 this+4+K*0x9c8 定址第 K 个池：
//   [1]_get_level_limit [2]part_type [3]base_item [4]option_type [5]quantity
//   [6]group [7]/[8]option_value [9]give 组选 [10]give seed
//   [11]reset 下标 [12]reset 数值）
// ===========================================================================

namespace random_option
{

// ---- _get_level_limit —— T 0x085f0f9a（_get_level_limit.md 全文转写）------
// ORIG 域常量（nm：_ZN13random_optionL13kMaxItemLevelE / L15KItemStartLevelE /
// L18KItemLevelIntervalE / L15MAX_RANDOM_SEEDE / L16MAX_OPTION_VALUEE，r 段）
static const int kMaxItemLevel = 80;
static const int KItemStartLevel = 10;
static const int KItemLevelInterval = 10;
static const int MAX_RANDOM_SEED = 64;
static const int MAX_OPTION_VALUE = 100;

int CRandomOptionItemHandle::_get_level_limit(ENUM_RARITY rarity, int level)
{
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)level);
    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());

    std::map<RandomApplicationKey, LevelLimitDecisionValue>::iterator it =
        script->m_map18.find(key);
    if (it != script->m_map18.end())
    {
        unsigned long bound =
            (unsigned long)(it->second.m_field0 + it->second.m_field4);
        int r = m_rand[1].randInt(bound);
        if ((unsigned int)it->second.m_field0 < (unsigned int)r)
        {
            level = (level == 1) ? 5 : level + 5;
        }
    }
    return level;
}

// ---- _get_part_type —— T 0x085f1088（_get_part_type.md 全文转写）----------
int CRandomOptionItemHandle::_get_part_type(int limit)
{
    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());

    std::map<int, PartSelectonValue>::iterator it = script->m_map30.find(limit);
    if (it == script->m_map30.end())
    {
        return 0;
    }

    unsigned long bound = (unsigned long)it->second.m_fieldC;
    unsigned int r = (unsigned int)m_rand[2].randInt(bound);

    std::vector<PartSelectonField>& vec = it->second.m_vec0;
    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        if (r < (unsigned int)vec[i].m_weight)
        {
            return vec[i].m_partType;
        }
    }
    return 0;
}

// ---- _get_part_type_by_ItemGroup —— T 0x085f11a4（同名报告全文转写）-------
int CRandomOptionItemHandle::_get_part_type_by_ItemGroup(
    int limit, const std::vector<int>& itemGroups) const
{
    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());

    std::map<int, PartSelectonValue>::iterator it = script->m_map30.find(limit);
    if (it == script->m_map30.end())
    {
        return 0;
    }

    std::vector<int> weights(itemGroups.size(), 0);
    unsigned int total = 0;

    std::vector<PartSelectonField>& vec = it->second.m_vec10;
    for (std::size_t v = 0; v < vec.size(); ++v)
    {
        for (std::size_t i = 0; i < itemGroups.size(); ++i)
        {
            if (vec[v].m_partType == itemGroups[i])
            {
                weights[i] = (unsigned int)vec[v].m_weight;
                total += (unsigned int)vec[v].m_weight;
                break;
            }
        }
    }

    unsigned long bound = total;
    unsigned int r = (unsigned int)m_rand[2].randInt(bound);

    unsigned int acc = 0;
    for (std::size_t i = 0; i < weights.size(); ++i)
    {
        acc += weights[i];
        if (r < acc)
        {
            return itemGroups[i];
        }
    }
    return 0;
}

// ---- _get_base_item —— T 0x085f1426（同名报告全文转写）-------------------
int CRandomOptionItemHandle::_get_base_item(ENUM_RARITY rarity, int limit)
{
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)limit);
    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());

    std::map<RandomApplicationKey, BaseItemSelectionValue>::iterator it =
        script->m_map48.find(key);
    if (it == script->m_map48.end())
    {
        return 0;
    }

    unsigned long bound = (unsigned long)it->second.m_fieldC;
    unsigned int r = (unsigned int)m_rand[3].randInt(bound);

    std::vector<unsigned int>& vec = it->second.m_vec0;
    unsigned int idx = 0;
    for (std::size_t i = 0; i < vec.size(); ++i, ++idx)
    {
        if (r < vec[i])
        {
            return (int)idx;
        }
    }
    return 0;
}

// ---- _get_option_type —— T 0x085f2338（同名报告全文转写）-----------------
// 经 script+0x148 provider 虚槽0 取 map<RAKey, map<int,RandomOptionGain>>；
// 内层先 find(-1)，未命中再 find(group)。
// 【运行期 blocked】provider 由 importRandomOptionScript 批次构造。
int CRandomOptionItemHandle::_get_option_type(unsigned long /*itemIdx*/,
                                              ENUM_RARITY rarity, int level,
                                              int group, unsigned char& outType)
{
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)level);

    CRandomOptionGive* provider = static_cast<CRandomOptionGive*>(m_optionSet);
    std::map<RandomApplicationKey, std::map<int, RandomOptionGive> >& giveMap =
        provider->get_option_type();  // CRandomOptionGive 纯虚 ORIG 名（UserMiscLink 批改名）

    std::map<RandomApplicationKey,
             std::map<int, RandomOptionGive> >::const_iterator outer =
        giveMap.find(key);
    if (outer == giveMap.end())
    {
        return 0x23f;
    }

    const std::map<int, RandomOptionGive>& inner = outer->second;
    std::map<int, RandomOptionGive>::const_iterator g = inner.find(-1);
    if (g == inner.end())
    {
        g = inner.find(group);
        if (g == inner.end())
        {
            return 0x255;
        }
    }
    const RandomOptionGive& give = g->second;

    unsigned long bound = (unsigned long)give.m_c;
    int r = m_rand[4].randInt(bound);

    CRandomList<unsigned int> chooser((unsigned int)r);
    std::for_each(give.m_vec0.begin(), give.m_vec0.end(), chooser);

    outType = (unsigned char)(chooser.get_value() + 1);
    return 0;
}

// ---- _get_option_quantity —— T 0x085f25b0（同名报告全文转写）-------------
// 键 = int{b0=rarity, b1=ff|level, b2=grade, b3=optType}
int CRandomOptionItemHandle::_get_option_quantity(ENUM_RARITY rarity, int level,
                                                  int grade, int optType,
                                                  int& outQuantity)
{
    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());

    unsigned char keyBytes[4];
    keyBytes[0] = (unsigned char)rarity;
    keyBytes[1] = 0xff;
    keyBytes[2] = (unsigned char)grade;
    keyBytes[3] = optType;

    int mapKey;
    std::memcpy(&mapKey, keyBytes, 4);
    std::map<int, OptionQuantityValue>::iterator it =
        script->m_mapb4.find(mapKey);
    if (it == script->m_mapb4.end())
    {
        keyBytes[1] = (unsigned char)level;
        std::memcpy(&mapKey, keyBytes, 4);
        it = script->m_mapb4.find(mapKey);
        if (it == script->m_mapb4.end())
        {
            return 0x26c;
        }
    }

    if (it->second.m_maxB == 0)
    {
        return 0x270;
    }

    std::vector<unsigned int> weights;
    unsigned int total = 0;
    for (short i = (short)(it->second.m_minA - 1); i < it->second.m_maxB; ++i)
    {
        QuantityRatio& qr = script->m_veca8[i];
        weights.push_back(qr.m_weight4);
        total += qr.m_weight4;
    }

    if (weights.empty())
    {
        return 0x27d;
    }

    if (weights.size() == 1)
    {
        outQuantity = it->second.m_minA;
        return 0;
    }

    unsigned long bound = total;
    int r = m_rand[5].randInt(bound);
    CRandomList<unsigned int> chooser((unsigned int)r);
    std::for_each(weights.begin(), weights.end(), chooser);

    outQuantity = (int)chooser.get_value() + it->second.m_minA;
    return 0;
}

// ---- _choose_option_group —— T 0x085f2882（同名报告全文转写）-------------
// u64 键 = {rarity, ff|level, grade, optType} 低 4 字节 + dword(group) 高 4 字节
// （报告 asm +0x01f/+0x022：mov 0x1c(%ebp),%eax; mov %eax,-0x58(%ebp)，group 整体
// 写入键区 +4..+7）。经 provider 虚槽1 取表。
// 【运行期 blocked】同 _get_option_type 注。
int CRandomOptionItemHandle::_choose_option_group(ENUM_RARITY rarity, int level,
                                                  int grade, int optType,
                                                  int group, int& outGroup)
{
    unsigned char keyBytes[8];
    keyBytes[0] = (unsigned char)rarity;
    keyBytes[1] = 0xff;
    keyBytes[2] = (unsigned char)grade;
    keyBytes[3] = (unsigned char)optType;
    *(unsigned int*)(keyBytes + 4) = (unsigned int)group;   // ORIG mov 0x1c(%ebp)→键区+4

    CRandomOptionGive* provider = static_cast<CRandomOptionGive*>(m_optionSet);
    std::map<unsigned long long, OptionGroupChoiceValue>& choiceMap =
        provider->get_option_group_choice();

    // ORIG find 直传 &键区 8 字节引用（无中间 u64 拷贝）。
    std::map<unsigned long long, OptionGroupChoiceValue>::iterator it =
        choiceMap.find(*reinterpret_cast<const unsigned long long*>(keyBytes));
    if (it == choiceMap.end())
    {
        keyBytes[1] = (unsigned char)level;
        it = choiceMap.find(*reinterpret_cast<const unsigned long long*>(keyBytes));
        if (it == choiceMap.end())
        {
            return 0x29e;
        }
    }

    OptionGroupChoiceValue& val = it->second;

    unsigned long bound = (unsigned long)(val.m_field18 - 1);
    int r = m_rand[6].randInt(bound);

    CRandomList<unsigned int> chooser((unsigned int)r);
    std::for_each(val.m_vecC.begin(), val.m_vecC.end(), chooser);

    unsigned int sel = chooser.get_value();
    if (sel >= val.m_vec0.size())
    {
        return 0x2be;
    }
    outGroup = val.m_vec0.at(sel);
    return 0;
}

// ---- _get_option_value —— T 0x085f2b02（同名报告全文转写）----------------
int CRandomOptionItemHandle::_get_option_value(int minV, int maxV)
{
    unsigned long bound =
        (unsigned long)reinterpret_cast<RandomOptionScript*>(
            GetRandomOption())->m_ratioE4.m_c;
    int r1 = m_rand[7].randInt(bound);

    CRandomListEx<OptionValueRatio> chooser((unsigned int)r1);
    std::for_each(
        reinterpret_cast<RandomOptionScript*>(
            GetRandomOption())->m_ratioE4.m_vec0.begin(),
        reinterpret_cast<RandomOptionScript*>(
            GetRandomOption())->m_ratioE4.m_vec0.end(),
        chooser);

    OptionValueRatio sel = chooser.get_value();

    float spanf = sel.m_scaleRange0 * (float)(maxV - minV);
    int span = (int)spanf;      // ORIG fistpl 截断（fnstcw 0xc 实证，非四舍五入）

    float basef = sel.m_scaleBase4 * (float)minV;
    int base = (int)basef;      // ORIG fistpl 截断

    unsigned long bound2 = (unsigned long)span;
    int r2 = m_rand[8].randInt(bound2);
    return r2 + base;
}

// ---- reset_option —— T 0x085f39d2（reset_option.md 全文转写）-------------
unsigned int CRandomOptionItemHandle::reset_option(unsigned long /*itemIdx*/,
                                                   ENUM_RARITY rarity,
                                                   int optionType,
                                                   RandomOption* options)
{
    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());
    ENUM_RANDOM_OPTION_NUMBER slot = (ENUM_RANDOM_OPTION_NUMBER)optionType;

    if (options->get_option_index(slot) == 0)
    {
        return 0x17;
    }

    // set<unsigned long> @+0x200：禁重置道具黑名单；命中即拒绝。
    unsigned long idx = (unsigned long)options->get_option_index(slot);
    if (script->m_set200.find(idx) != script->m_set200.end())
    {
        return 0x17;
    }

    // map<unsigned char, ResetValues> @+0x1e8：按稀有度取重置区间。
    std::map<unsigned char, ResetValues>::iterator rv =
        script->m_map1e8.find((unsigned char)rarity);
    if (rv == script->m_map1e8.end())
    {
        return 0x13;
    }

    unsigned long boundIdx = (unsigned long)(rv->second.m_count - 1);
    int slotPick = m_rand[11].randInt(boundIdx);

    CRandomList<unsigned int> chooser((unsigned int)slotPick);
    std::for_each(rv->second.m_values.begin(), rv->second.m_values.end(),
                  chooser);

    unsigned int sel = chooser.get_value();
    if (sel >= rv->second.m_values.size())
    {
        return 0x13;
    }

    ResetValue& range = rv->second.m_values[sel];
    unsigned long valueBound = (unsigned long)(range.m_max - range.m_min - 1);
    int rolled = m_rand[12].randInt(valueBound);
    int value = rolled + range.m_min;
    if (MAX_OPTION_VALUE < value)
    {
        value = MAX_OPTION_VALUE;
    }

    options->set_first_value(slot, (unsigned char)(value & 0xff));
    options->set_second_value(slot, (unsigned char)(value & 0xff));
    return 0;
}

// ---- RegenVer_choose_item —— T 0x085f1802（同名报告全文转写）-------------
unsigned int CRandomOptionItemHandle::RegenVer_choose_item(
    ENUM_RARITY rarity, int level, const std::vector<int>& itemGroups)
{
    int limit = _get_level_limit(rarity, level);

    int partType = 0;
    if (itemGroups.empty())
    {
        partType = _get_part_type(limit);
    }
    else
    {
        partType = _get_part_type_by_ItemGroup(limit, itemGroups);
    }

    int count = _get_base_item(rarity, limit);

    BaseItemKey key((unsigned char)rarity, (unsigned char)limit,
                    (unsigned short)partType);

    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());
    // ORIG：外层 map<int,...> 键 = BaseItemKey 小端 int 打包
    std::map<int, std::map<int, BaseItemValue> >& table =
        script->m_baseItems60;
    std::map<int, std::map<int, BaseItemValue> >::const_iterator outer =
        table.find(*(int*)&key);

    if (outer == table.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "itemIndex_t random_option::CRandomOptionItemHandle::"
            "RegenVer_choose_item(ENUM_RARITY, int, const std::vector<int, "
            "std::allocator<int> >&)",
            0x19a, "choose_item (%d,%d,%d,%d)", (int)key.m_rarity,
            (int)key.m_level, (int)key.m_partType, count);
        return 0xffffffffu;
    }

    const std::map<int, BaseItemValue>& inner = outer->second;
    if (inner.empty())
    {
        return 0xffffffffu;
    }

    int steps = 0;
    std::map<int, BaseItemValue>::const_iterator walk = inner.begin();
    while (walk != inner.end() && count > steps)
    {
        ++walk;
        ++steps;
    }
    if (walk == inner.end())
    {
        --walk;
    }
    return (unsigned int)walk->second.m_field0;
}

// ---- give_option —— T 0x085f2cc6（ORIG 独立完整实现，逐段转写）----------
// 注意：辅助调用参数顺序按 ORIG 反汇编实证——
//   _get_option_type 第4参=grade（内层 find 键，非 group）；
//   _get_option_quantity 键=(rarity,ff,outGrade,level)，回退 b1=grade；
//   _choose_option_group 键=(rarity,ff,group,level)+u32(outGrade)。
bool CRandomOptionItemHandle::give_option(unsigned long itemIdx,
                                          ENUM_RARITY rarity, int level,
                                          int group, int grade,
                                          RandomOption* opts)
{
    unsigned char outGrade = 0;
    int err = 0;
    err = _get_option_type(itemIdx, rarity, level, grade, outGrade);
    if (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::give_option("
            "itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*)",
            0x2f9, "_get_option_type (%d,%d,%d,%d,%d) error_line(%d)",
            (int)itemIdx, (int)rarity, level, grade, (int)outGrade, err);
        return false;
    }

    int quantity = 0;
    err = _get_option_quantity(rarity, grade, outGrade, level, quantity);
    if (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::give_option("
            "itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*)",
            0x300, "_get_option_quantity (%d,%d,%d,%d,%d) error_line(%d)",
            (int)rarity, grade, (int)outGrade, level, quantity, err);
        return false;
    }

    // ORIG asm 85f2e27：m_field10(+0x10) 非零时覆盖数量（RegenVer 无此步）。
    if (m_field10 != 0)
        quantity = m_field10;

    int groupId = 0;
    err = _choose_option_group(rarity, grade, group, level, outGrade, groupId);
    if (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::give_option("
            "itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*)",
            0x30d, "_choose_option_group (%d,%d,%d,%d,%d,%d) error_line(%d)",
            (int)rarity, grade, group, level, (int)outGrade, groupId, err);
        return false;
    }

    // 不同权重表：map<short, DifferentWeightValue> @+0xf4；
    // 键 short = 低字节 rarity | 高字节 outGrade（asm -0x6e/-0x6d 实证）。
    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());
    DifferentWeightValue dwTmp;  // ORIG 未使用临时（O0 保留 ctor call）
    unsigned short dwRaw =
        (unsigned short)(((unsigned short)(unsigned char)rarity) |
                         ((unsigned short)outGrade << 8));
    std::map<short, DifferentWeightValue>::iterator dw =
        script->m_mapf4.find((short)dwRaw);
    if (dw == script->m_mapf4.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::give_option("
            "itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*)",
            0x319, "different_weight_ (%d,%d) ",
            (int)(unsigned char)rarity, (int)outGrade);
        return false;
    }
    int dwA = dw->second.m_field0;
    int dwB = dw->second.m_field4;

    // 组值表：map<int, OptionGroupValue> @+0x160。
    std::map<int, OptionGroupValue>::iterator ogIt =
        script->m_map160.find(groupId);
    if (ogIt == script->m_map160.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::give_option("
            "itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*)",
            0x321, "option_group_ (%d) ", groupId);
        return false;
    }

    int remaining = ogIt->second.m_fieldC;
    std::vector<OptionGroup> groupVec = ogIt->second.m_vec0;  // 值拷贝后 erase
    unsigned char* const optB = reinterpret_cast<unsigned char*>(opts);

    for (int slotI = 0; slotI < quantity && slotI <= 2; ++slotI)
    {
        unsigned long bound = (unsigned long)remaining;
        int pickN = m_rand[9].randInt(bound);      // m_rand+0x5808 → 池[9]

        CRandomList<unsigned int> chooser((unsigned int)pickN);
        std::for_each(groupVec.begin(), groupVec.end(), chooser);

        unsigned int sel = chooser.get_value();
        if (sel < groupVec.size())
        {
            OptionGroup og = groupVec[sel];
            optB[slotI * 3] = (unsigned char)og.m_type0;  // 槽首字节=组类型
            remaining -= og.m_weight4;

            std::vector<OptionGroup>::iterator victim = groupVec.begin();
            victim += sel;
            groupVec.erase(victim);

            optB[slotI * 3 + 1] =
                (unsigned char)_get_option_value(dwA, dwB);
            optB[slotI * 3 + 2] =
                (unsigned char)_get_option_value(dwA, dwB);
        }
    }

    // seed 字节(+9)：低 2 位 = outGrade；高位 = 池[10](+0x61d0) 掷 0x40 后 <<2
    unsigned char* optBytes = reinterpret_cast<unsigned char*>(opts);
    optBytes[9] = (unsigned char)((optBytes[9] & 0xfc) | (outGrade & 3));
    unsigned long seedBound = MAX_RANDOM_SEED;
    int seedRoll = m_rand[10].randInt(seedBound);
    optBytes[9] = (unsigned char)((optBytes[9] & 3) | ((seedRoll & 0x3f) << 2));

    return true;
}

// ---- RegenVer_give_option —— T 0x085f1bb0（ORIG 独立完整实现，逐段转写）--
// 数量入口 optType==0x65 走 RegenVer_get_option_quantity（键构建同
// _get_option_quantity：(rarity,ff,outGrade,level)，回退 b1=grade）；
// 数值出口 optType==0xc9 走 roi->RegenVer_get_option_value，否则
// _get_option_value。logFormat 行号/PRETTY 与 give_option 不同。
bool CRandomOptionItemHandle::RegenVer_give_option(
    unsigned long itemIdx, ENUM_RARITY rarity, int level, int group, int grade,
    RandomOption* opts, RegenerationROI* roi, int optType)
{
    unsigned char outGrade = 0;
    int err = 0;
    err = _get_option_type(itemIdx, rarity, level, grade, outGrade);
    if (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
            "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
            " RegenerationROI*, int)",
            0x1d8, "_get_option_type (%d,%d,%d,%d,%d) error_line(%d)",
            (int)itemIdx, (int)rarity, level, grade, (int)outGrade, err);
        return false;
    }

    int quantity = 0;
    if (optType == 0x65)
    {
        err = RegenVer_get_option_quantity(rarity, grade, outGrade, level,
                                           quantity, roi);
        if (err != 0)
        {
            LogManager::logFormat(
                1, "RandomOption.cpp",
                "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
                "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
                " RegenerationROI*, int)",
                0x1e2, "_get_option_quantity (%d,%d,%d,%d,%d) error_line(%d)",
                (int)rarity, grade, (int)outGrade, level, quantity, err);
            return false;
        }
    }
    else
    {
        err = _get_option_quantity(rarity, grade, outGrade, level, quantity);
        if (err != 0)
        {
            LogManager::logFormat(
                1, "RandomOption.cpp",
                "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
                "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
                " RegenerationROI*, int)",
                0x1ea, "_get_option_quantity (%d,%d,%d,%d,%d) error_line(%d)",
                (int)rarity, grade, (int)outGrade, level, quantity, err);
            return false;
        }
    }

    int groupId = 0;
    err = _choose_option_group(rarity, grade, group, level, outGrade, groupId);
    if (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
            "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
            " RegenerationROI*, int)",
            0x1f2, "_choose_option_group (%d,%d,%d,%d,%d,%d) error_line(%d)",
            (int)rarity, grade, group, level, (int)outGrade, groupId, err);
        return false;
    }

    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());
    DifferentWeightValue dwTmp;  // ORIG 未使用临时（O0 保留 ctor call）
    unsigned short dwRaw =
        (unsigned short)(((unsigned short)(unsigned char)rarity) |
                         ((unsigned short)outGrade << 8));
    std::map<short, DifferentWeightValue>::iterator dw =
        script->m_mapf4.find((short)dwRaw);
    if (dw == script->m_mapf4.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
            "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
            " RegenerationROI*, int)",
            0x1fe, "different_weight_ (%d,%d) ",
            (int)(unsigned char)rarity, (int)outGrade);
        return false;
    }
    int dwA = dw->second.m_field0;
    int dwB = dw->second.m_field4;

    std::map<int, OptionGroupValue>::iterator ogIt =
        script->m_map160.find(groupId);
    if (ogIt == script->m_map160.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
            "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
            " RegenerationROI*, int)",
            0x206, "option_group_ (%d) ", groupId);
        return false;
    }

    int remaining = ogIt->second.m_fieldC;
    std::vector<OptionGroup> groupVec = ogIt->second.m_vec0;
    unsigned char* const optB = reinterpret_cast<unsigned char*>(opts);

    for (int slotI = 0; slotI < quantity && slotI <= 2; ++slotI)
    {
        unsigned long bound = (unsigned long)remaining;
        int pickN = m_rand[9].randInt(bound);

        CRandomList<unsigned int> chooser((unsigned int)pickN);
        std::for_each(groupVec.begin(), groupVec.end(), chooser);

        unsigned int sel = chooser.get_value();
        if (sel < groupVec.size())
        {
            OptionGroup og = groupVec[sel];
            optB[slotI * 3] = (unsigned char)og.m_type0;
            remaining -= og.m_weight4;

            std::vector<OptionGroup>::iterator victim = groupVec.begin();
            victim += sel;
            groupVec.erase(victim);

            unsigned char v1;
            unsigned char v2;
            if (optType == 0xc9)
            {
                v1 = (unsigned char)roi->RegenVer_get_option_value(dwA, dwB);
                v2 = (unsigned char)roi->RegenVer_get_option_value(dwA, dwB);
            }
            else
            {
                v1 = (unsigned char)_get_option_value(dwA, dwB);
                v2 = (unsigned char)_get_option_value(dwA, dwB);
            }
            optB[slotI * 3 + 1] = v1;
            optB[slotI * 3 + 2] = v2;
        }
    }

    unsigned char* optBytes = reinterpret_cast<unsigned char*>(opts);
    optBytes[9] = (unsigned char)((optBytes[9] & 0xfc) | (outGrade & 3));
    unsigned long seedBound = MAX_RANDOM_SEED;
    int seedRoll = m_rand[10].randInt(seedBound);
    optBytes[9] = (unsigned char)((optBytes[9] & 3) | ((seedRoll & 0x3f) << 2));

    return true;
}

// ---- RegenVer_get_option_quantity —— T 0x085f1a62（同名报告全文转写）------
int CRandomOptionItemHandle::RegenVer_get_option_quantity(
    ENUM_RARITY rarity, int level, int grade, int optType, int& outQuantity,
    RegenerationROI* roi)
{
    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());

    unsigned char keyBytes[4];
    keyBytes[0] = (unsigned char)rarity;
    keyBytes[1] = 0xff;
    keyBytes[2] = (unsigned char)grade;
    keyBytes[3] = optType;

    int mapKey;
    std::memcpy(&mapKey, keyBytes, 4);
    std::map<int, OptionQuantityValue>::iterator it =
        script->m_mapb4.find(mapKey);
    if (it == script->m_mapb4.end())
    {
        keyBytes[1] = (unsigned char)level;
        std::memcpy(&mapKey, keyBytes, 4);
        it = script->m_mapb4.find(mapKey);
        if (it == script->m_mapb4.end())
        {
            return 0x1ad;
        }
    }

    if (it->second.m_maxB == 0)
    {
        return 0x1b1;
    }

    outQuantity =
        roi->RegenVer_get_option_quantity(it->second.m_minA, it->second.m_maxB);
    if (outQuantity == 0)
    {
        return 0x1b5;
    }
    return 0;
}

}  // namespace random_option

// ===========================================================================
// RegenerationROI 再铸链
// ===========================================================================

// ---- SendMessageToCUser —— T 0x085f853c（报告缺失，objdump 直转）----------
void RegenerationROI::SendMessageToCUser(CUser* user, const char* msg)
{
    if (!user->isGMUser())
    {
        return;
    }
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int((int)strlen(msg));
    packet.put_str(msg, (int)strlen(msg));
    packet.finalize(true);
    user->Send(packet);
}

// ---- _Select_Part —— T 0x085f8ec2（报告缺失，objdump 直转）----------------
void RegenerationROI::_Select_Part(int partIdx, std::vector<int>& out)
{
    std::map<int, str_index_to_itemgroup>::iterator it =
        m_regenerator.m_map0.find(partIdx);

    std::string groupName;
    if (it != m_regenerator.m_map0.end())
    {
        std::vector<std::string>& names = it->second.m_vec8;
        for (std::size_t i = 0; i < names.size(); ++i)
        {
            groupName = names[i];
            std::map<std::string, int>& nameTable = getItemGroupNameTable();
            std::map<std::string, int>::const_iterator f =
                nameTable.find(groupName);
            if (f != nameTable.end())
            {
                out.push_back(f->second);
            }
        }
    }
    else
    {
        cMyTrace t("void RegenerationROI::_Select_Part(int, "
                   "std::vector<int, std::allocator<int> >&)",
                   0xd6, 5);
        t("packet:%d groupName:%s\n", partIdx, groupName.c_str());
    }
}

// ---- _Calculate_RegenCost —— T 0x085f9094（同名报告全文转写）--------------
int RegenerationROI::_Calculate_RegenCost(int partIdx, int optType, int lv0,
                                          int lv1)
{
    int cost1 = 0;
    int cost2 = 0;

    if (partIdx != 0 && partIdx <= 99)
    {
        std::map<int, std::map<int, _str_cost> >::iterator outer =
            m_regenerator.m_map18.find(partIdx);
        if (outer == m_regenerator.m_map18.end())
        {
            return -1;
        }
        std::map<int, _str_cost>& inner = outer->second;

        std::map<int, _str_cost>::iterator a = inner.find(lv0);
        if (a == inner.end())
        {
            return -1;
        }
        if (a->second.m_field0 == 0)
        {
            cost1 = a->second.m_field4;
        }

        std::map<int, _str_cost>::iterator b = inner.find(lv1);
        if (b == inner.end())
        {
            return -1;
        }
        if (b->second.m_field0 == 0)
        {
            cost1 = (cost1 + b->second.m_field4) / 2;
        }
    }

    if (optType != 0 && optType > 100)
    {
        std::map<int, std::map<int, _str_cost> >::iterator outer =
            m_regenerator.m_map18.find(optType);
        if (outer == m_regenerator.m_map18.end())
        {
            return -1;
        }
        std::map<int, _str_cost>& inner = outer->second;

        std::map<int, _str_cost>::iterator a = inner.find(lv0);
        if (a == inner.end())
        {
            return -1;
        }
        if (a->second.m_field0 == 0)
        {
            cost2 = a->second.m_field4;
        }

        std::map<int, _str_cost>::iterator b = inner.find(lv1);
        if (b == inner.end())
        {
            return -1;
        }
        if (b->second.m_field0 == 0)
        {
            cost2 = (cost2 + b->second.m_field4) / 2;
        }
    }

    return cost1 + cost2;
}

// ---- _RandomLevel —— T 0x085f93cc（报告缺失，objdump 直转）----------------
void RegenerationROI::_RandomLevel(int lv0, int lv1, select_base_item& sb,
                                   std::stringstream& ss)
{
    ss << "try regen:lv:" << lv0 << "," << lv1 << "\n";

    if (lv0 > lv1)
    {
        std::swap(lv0, lv1);
    }

    int lo = (lv0 % 5 == 0) ? lv0 : lv0 + (5 - lv0 % 5);   // 向上取整到 5 的倍数
    int hi = lv1 - (lv1 % 5);                               // 向下取整到 5 的倍数

    std::vector<int> levels;
    for (int v = lo; v <= hi; v += 5)
    {
        levels.push_back(v);
    }

    if (levels.empty())
    {
        ss << "level count : Empty\n";
        return;
    }

    unsigned long bound = (unsigned long)(levels.size() - 1);
    int pick = m_regeneratorPtr->randInt(bound);
    sb.m_level0 = levels[pick];

    if (sb.m_level0 % 10 != 0 && sb.m_level0 > 10)
    {
        sb.m_level0 += 5;
    }

    ss << "Level count : " << (unsigned int)levels.size()
       << " and select lv : " << sb.m_level0 << "\n";
}

// ---- RegenVer_get_option_quantity —— T 0x085f8cb4（同名报告全文转写）------
int RegenerationROI::RegenVer_get_option_quantity(int lo, int hi)
{
    if (lo < 0 || 3 < lo)
    {
        return 0;
    }
    if (hi < 0 || 3 < hi)
    {
        return 0;
    }

    int total = 0;
    for (int i = lo - 1; i < hi; ++i)
    {
        total += m_regenerator.m_vec30[i];
    }

    unsigned long bound = (unsigned long)total;
    int r = m_regeneratorPtr->randInt(bound);

    int acc = 0;
    for (int i = lo - 1; i < hi; ++i)
    {
        acc += m_regenerator.m_vec30[i];
        if (r <= acc)
        {
            return i + 1;
        }
    }
    return m_regenerator.m_vec30[lo];
}

// ---- RegenVer_get_option_value —— T 0x085f8da6（同名报告全文转写）---------
int RegenerationROI::RegenVer_get_option_value(int minV, int maxV)
{
    unsigned long bound = (unsigned long)m_regenerator.m_field48;
    int r = m_regeneratorPtr->randInt(bound);

    float scaleRange = 0.0f;   // 默认零值同 OptionValueRatio W ctor 0x85f4176
    float scaleBase = 0.0f;

    int acc = 0;
    std::vector<_str_option_value_ratio>& ratios = m_regenerator.m_vec3c;
    for (std::size_t i = 0; i < ratios.size(); ++i)
    {
        acc += (int)ratios[i].m_weight8;
        if ((unsigned int)r <= (unsigned int)acc)
        {
            scaleRange = ratios[i].m_scaleRange0;
            scaleBase = ratios[i].m_scaleBase4;
            break;
        }
    }

    float spanf = scaleRange * (float)(maxV - minV);
    int span = (int)(spanf + (spanf >= 0.0f ? 0.5f : -0.5f));   // ROUND(fistp)

    float basef = scaleBase * (float)minV;
    int base = (int)(basef + (basef >= 0.0f ? 0.5f : -0.5f));

    unsigned long bound2 = (unsigned long)span;
    int r2 = m_regeneratorPtr->randInt(bound2);
    return r2 + base;
}

// ---- Regenerate —— T 0x085f8646（Regenerate.md 全文转写）------------------
int RegenerationROI::Regenerate(int baseLv0, int baseLv1, int partIdx,
                                int optType, int& outCost, Inven_Item& outItem,
                                CUser* user)
{
    std::stringstream ss(std::ios::out | std::ios::in);
    select_base_item sb;
    _RandomLevel(baseLv0, baseLv1, sb, ss);

    // 函数级 static handle（guard 0x09473100 / 对象 0x09473108）
    static random_option::CRandomOptionItemHandle handle;

    if (optType != 0 && optType != 0x65)
    {
        return 0x11;
    }

    outCost = _Calculate_RegenCost(partIdx, optType, baseLv0, baseLv1);
    if (outCost == -1)
    {
        cMyTrace t("int RegenerationROI::Regenerate(int, int, int, int, int&, "
                   "Inven_Item&, CUser*)",
                   0x47, 5);
        t("ROI regen Error : %d, %d, %d, %d", partIdx, optType, baseLv0,
          baseLv1);
        return 0x11;
    }

    std::vector<int> parts;
    if (partIdx != 0)
    {
        _Select_Part(partIdx, parts);
    }
    if (partIdx == -1)
    {
        return 0x11;
    }

    int chosenIdx = (int)handle.RegenVer_choose_item((ENUM_RARITY)2, sb.m_level0,
                                                     parts);
    ss << "base item index : " << chosenIdx << "\n";

    CItem* base = G_CDataManager()->find_item(chosenIdx);
    if (base == 0)
    {
        ss << "base item is Null\n";
        SendMessageToCUser(user, ss.str().c_str());
        return 0x11;
    }

    int rarity = base->get_rarity();
    int usableLevel = base->getUsableLevel();
    int optionGrade = base->GetRandomOptionGrade();   // vtable 槽20（+0x50）

    ss << "rarity : " << rarity << " level : " << usableLevel
       << " base item item : " << optionGrade << "\n";

    outItem.reset();
    outItem.m_addInfo = chosenIdx;
    G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, outItem,
                                              1);

    bool ok = handle.RegenVer_give_option(
        (unsigned long)chosenIdx, (ENUM_RARITY)rarity, usableLevel,
        base->getItemGroupName(), optionGrade, &outItem.m_random.m_random, this,
        optType);

    if (ok)
    {
        for (int i = 0; i < 3; ++i)
        {
            const unsigned char* slot =
                reinterpret_cast<const unsigned char*>(&outItem) + 0x25 + i * 3;
            ss << "[" << i << "] :" << (unsigned int)slot[0] << ":"
               << (unsigned int)slot[1] << ":" << (unsigned int)slot[2] << "\n";
        }
        SendMessageToCUser(user, ss.str().c_str());
        return 0;
    }

    ss << "give_option false\n";
    SendMessageToCUser(user, ss.str().c_str());
    return 0x13;
}

namespace random_option
{

// ---- _random_application —— T 0x085f0ec2（_random_application.md 全文转写）----
// m_map0 查 (rarity,level)：命中时 roll=randInt(m_field8+m_field4)，
// m_field8 < roll → 1（应用），否则 0；未命中 0。
int CRandomOptionItemHandle::_random_application(ENUM_RARITY rarity, int level)
{
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)level);
    RandomOptionScript* table =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());
    std::map<RandomApplicationKey, RandomApplicationValue>::iterator it =
        table->m_map0.find(key);
    if (it != table->m_map0.end())
    {
        const int bound = it->second.m_field8 + it->second.m_field4;
        const unsigned long roll =
            static_cast<unsigned long>(m_rand->randInt(bound));
        if (static_cast<unsigned int>(it->second.m_field8) < roll)
        {
            return 1;
        }
    }
    return 0;
}

// ---- choose_item —— T 0x085f1560（choose_item.md 全文转写）----
// m_disabled 非 0 → -1；level>9 时向下取整到十位（钳 0x50）；
// level<10 且 rarity==2 → rarity 清 0 返 -1；_random_application 通过后
// 按 (rarity,levelLimit,partType) 查 m_baseItems60，取第 baseItem 项
// （越界回退末项）的 BaseItemValue::m_field0；任何失配 rarity 清 0 返 -1。
unsigned int CRandomOptionItemHandle::choose_item(ENUM_RARITY& rarity,
                                                  int level)
{
    if (m_disabled == 0)
    {
        if (level >= KItemStartLevel)
        {
            if (level < kMaxItemLevel)
            {
                const int rem = level % KItemLevelInterval;
                level = level - rem;
            }
            else
            {
                level = kMaxItemLevel;
            }
        }
        if (level < KItemStartLevel && rarity == 2)
        {
            rarity = static_cast<ENUM_RARITY>(0);
            return 0xffffffff;
        }
        if (_random_application(rarity, level) == 1)
        {
            m_field08 = m_field08 + 1;
            const int levelLimit = _get_level_limit(rarity, level);
            const int partType = _get_part_type(levelLimit);
            const int baseItem = _get_base_item(rarity, levelLimit);
            BaseItemKey key((unsigned char)rarity,
                            (unsigned char)levelLimit,
                            (unsigned short)partType);
            RandomOptionScript* table =
                reinterpret_cast<RandomOptionScript*>(GetRandomOption());
            // ORIG：外层 map<int,...> 键 = BaseItemKey 小端 int 打包
            std::map<int, std::map<int, BaseItemValue> >::iterator it =
                table->m_baseItems60.find(*(int*)&key);
            if (it == table->m_baseItems60.end())
            {
                rarity = static_cast<ENUM_RARITY>(0);
                return 0xffffffff;
            }
            if (!it->second.empty())
            {
                int idx = 0;
                std::map<int, BaseItemValue>::iterator vit = it->second.begin();
                while (vit != it->second.end() && baseItem > idx)
                {
                    ++vit;
                    idx = idx + 1;
                }
                if (vit == it->second.end())
                {
                    --vit;
                }
                return static_cast<unsigned int>(vit->second.m_field0);
            }
            rarity = static_cast<ENUM_RARITY>(0);
            return 0xffffffff;
        }
        return 0xffffffff;
    }
    return 0xffffffff;
}

}  // namespace random_option

// ---- ORIG W 弱符号发射段（RandomOptionScript.h 头内联定义，本 TU 无 odr-use）----
//   ResetValues C1/C2 @0x08a73df6（vector ctor + clear）、C1ERKS/C2ERKS @0x08a74cda、
//   clear @0x08a73e38、D1/D2 @0x083cf4b4（仅 vector dtor）。
// 真实构造/拷贝/析构强制发射，不引入导出符号。
namespace
{
void emitResetValuesWeakSymbols()
{
    ResetValues a;             // C1/C2
    ResetValues b(a);          // C1ERKS/C2ERKS
    a.clear();                 // clear
    (void)b;
}
}
