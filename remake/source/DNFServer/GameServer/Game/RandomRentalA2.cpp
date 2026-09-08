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

unsigned char RandomOption::get_option_index(ENUM_RANDOM_OPTION_NUMBER num)
{
    // [b28d-solo-RandomRentalA2 判词] NEAR 21/21（ORIG 0x085f4040）：数组视图形
    // 已复现 ORIG 双寄存器寻址；旧判词：
    // `movzbl (%eax,%ecx,1)` vs 4.4 lea+movzbl 两段 + 帧 0x18 vs 0x8。语义等价。
    // [b28c-11] NEAR 封顶：ORIG `movzbl (%eax,%ecx,1),%eax`（双寄存器寻址）vs
    // 4.4 `lea (%ecx,%eax,1),%eax; movzbl (%eax),%eax`（N4/N30 族，不可折叠）。
    // W 0x85f4040：change_option ? this[0xa] : this[num*3+0]
    if (change_option(num))
    {
        return m_fielda.m_field0;
    }
    // [b28d-solo-RandomRentalA2] 数组视图形 `(*arr)[i]` 复现 ORIG 双寄存器寻址
    // `movzbl (%eax,%ecx,1),%eax`（指针算术形恒折 lea+movzbl；探针 probe1.cpp getC 实证）。
    // 残余仅 this/num 两条装载序（N4/N30 跨版本寄存器分配）。
    return (*reinterpret_cast<const RandomOptionField(*)[3]>(this))[num].m_field0;
}

void RandomOption::set_first_value(ENUM_RANDOM_OPTION_NUMBER num, unsigned char v)
{
    // [b28d-solo-RandomRentalA2 判词] NEAR 26/26（ORIG 0x085f4078）：P210 定义
    // 后置 + 数组元素视图已复现帧 0x28 与两步 lea；残余仅 this/num 装载序。旧判词：
    // v 槽 -0xc vs -0x4（4.1 槽基差）+ 两步 lea。语义等价。
    // [b28c-11] NEAR 封顶（26/26 条）：残余 = 帧 0x28/0xc、v 槽 -0xc/-0x4、
    // 地址两步 lea（ORIG `lea (%ecx,%eax,1),%eax; lea 0x1(%eax),%edx`）vs 4.4
    // `add $0x1,%eax; lea (%ecx,%eax,1),%edx`（N1 两步常量算术折叠）。
    // W 0x85f4078：change_option ? this[0xb]=v；恒写 this[num*3+1]=v
    if (change_option(num))
    {
        m_fielda.m_field1 = v;
    }
    // [b28d-solo-RandomRentalA2] 数组元素视图复现 ORIG 两步 lea
    // `lea (%ecx,%eax,1),%eax; lea 0x1(%eax),%edx`（探针 probe2.cpp s4 实证）。
    (*reinterpret_cast<RandomOptionField(*)[3]>(this))[num].m_field1 = v;
}

void RandomOption::set_second_value(ENUM_RANDOM_OPTION_NUMBER num, unsigned char v)
{
    // [b28d-solo-RandomRentalA2 判词] NEAR 26/26（ORIG 0x085f40be）：同 set_first_value。
    // [b28c-11] NEAR 封顶（26/26 条）：同 set_first_value（N1 两步 lea + 帧/槽差）。
    // W 0x85f40be：change_option ? this[0xc]=v；恒写 this[num*3+2]=v
    if (change_option(num))
    {
        m_fielda.m_field2 = v;
    }
    // [b28d-solo-RandomRentalA2] 同 set_first_value（探针 probe2.cpp s4 实证）。
    (*reinterpret_cast<RandomOptionField(*)[3]>(this))[num].m_field2 = v;
}

// [b28d-solo-RandomRentalA2] P210：change_option 定义后置于三个调用者之后，
// 使 4.4 对仅声明的 callee 走保守帧（get 0x18 / set 0x28，v spill@-0xc），
// 与 ORIG 4.1 完全一致（探针 probe4.cpp c1 实证）。
bool RandomOption::change_option(ENUM_RANDOM_OPTION_NUMBER num)
{
    // [b28d-solo-RandomRentalA2 判词] DIFF 22/23（ORIG 0x085f4002）：旧判词：
    // 的先后跨版本不可择 + 帧 0x18 vs 0x4。语义等价。
    // [b28c-11] NEAR 封顶（22/22 条）：4.4 对 `(unsigned char)(x & 3)` 恒先窄化
    // （movzbl %al,%eax; mov %eax,%edx; and $3,%edx）而 ORIG 4.1 先 and 后窄化
    // （and $3,%eax; movzbl %al,%edx），且帧 sub $0x18(ORIG)/$0x4(4.4) 非 16k+4
    // 量化（P190 附注②不可凑）。N18/N22 族，语义等价。
    // W 0x85f4002：!m_fielda.empty() && (m_seedd & 3) == num
    if (!m_fielda.empty())
    {
        if ((m_seedd.m_field & 3) == (unsigned int)num)
            return true;
    }
    return false;
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
    // [b28d-59] ORIG _ZN11CRandomListIjEclE10ResetValue（W 0x085f5e9e）：34 条，
    //   无内部 call ⇒ 独立内联展开（旧转发形只 10 条，真语义/形态 bug 已修）；
    //   参数按值（12 字节 ResetValue，mangled 无 RK 前缀）；取 m_weight8 作权重，
    //   0x085f5ec2/0x085f5ed7 两处重读形参槽 0x14(%ebp)（P0 无具名局部）。
    // ORIG _ZN11CRandomListIjEclE11OptionGroup（W 0x085f58d2）：
    //   参数按值（8 字节 OptionGroup）；取 m_weight4 作权重。
    void operator()(ResetValue rv)
    {
        if (m_sum > m_n)
        {
            return;
        }
        if (m_n - m_sum < (unsigned int)rv.m_weight8)
        {
            m_value = m_count;
        }
        m_sum += (unsigned int)rv.m_weight8;
        ++m_count;
    }
    void operator()(OptionGroup og)
    {
        if (m_sum > m_n)
        {
            return;
        }
        if (m_n - m_sum < og.m_weight4)
        {
            m_value = m_count;
        }
        m_sum += og.m_weight4;
        ++m_count;
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 74/75（ORIG 0x085f0f9a）：P0/P48 script
    // 内联 + char framePad 撑帧已归位 key/it/end 槽；残余 bound(-0x20)/end(-0x10)
    // 与 ORIG (-0x10/-0x14) 互换（N93/N82 槽分配族）。旧判词：
    // 形（ORIG `lea 0x18(%eax),%ecx` 单条 vs 4.4 具名局部 + 槽序）。语义等价。
    // P190/P232：ORIG 槽基比 4.4 深 4 ⇒ 死局部撑帧。
    char framePad[4];
    (void)framePad;
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)level);
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 每处 map 访问独立调 GetRandomOption
    // （`call; lea 0x18(%eax),%ecx` 直传，无具名 script 槽）。
    std::map<RandomApplicationKey, LevelLimitDecisionValue>::iterator it =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map18.find(key);
    if (it != reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map18.end())
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
    // [b28d-solo-RandomRentalA2 判词] NEAR 84/84（ORIG 0x085f1088）：P0/P48 + P222
    // 正向包裹 + 迭代器循环已复现指令序列；残余 bound/end 槽互换（N93/N82）。旧判词：
    // 形（ORIG `lea 0x30(%eax),%ecx` 单条）+ 帧槽序。语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 每处 map 访问独立调 GetRandomOption。
    // P212：块内标量提前到函数头（4.4 块级槽方向与 4.1 相反）；残余仅
    // bound(-0x18)/end 临时(-0x14) 与 ORIG 互换（N93/N82 槽分配族）。
    unsigned long bound;
    unsigned int r;
    std::map<int, PartSelectonValue>::iterator it =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map30.find(limit);
    // P222：ORIG `je <fail 岛>` + body 直落 ⇒ 正向包裹（早返形多 mov$0/jmp）。
    if (it != reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map30.end())
    {
        bound = (unsigned long)it->second.m_fieldC;
        r = (unsigned int)m_rand[2].randInt(bound);

        // ORIG 为迭代器循环（begin 槽 -0x20 / 每轮重调 end 槽 -0x10 /
        // operator-> 取 m_weight、m_partType），非 size() 索引循环；
        // 且每次重取 it->second.m_vec0（无 vec 引用局部）。
        for (std::vector<PartSelectonField>::iterator i = it->second.m_vec0.begin();
             i != it->second.m_vec0.end(); ++i)
        {
            if (r < (unsigned int)i->m_weight)
            {
                return i->m_partType;
            }
        }
    }
    return 0;
}

// ---- _get_part_type_by_ItemGroup —— T 0x085f11a4（同名报告全文转写）-------
int CRandomOptionItemHandle::_get_part_type_by_ItemGroup(
    int limit, const std::vector<int>& itemGroups) const
{
    // [b28d-solo-RandomRentalA2 判词] DIFF 192/180（ORIG 0x085f11a4）：P0/P48 script
    // 内联已消缓存局部；残余循环形态/槽序。旧判词：
    // （P183 族）。语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 每处 map 访问独立调 GetRandomOption。
    std::map<int, PartSelectonValue>::iterator it =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map30.find(limit);
    if (it == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map30.end())
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
    // [b28d-solo-RandomRentalA2 判词] NEAR 92/92（ORIG 0x085f1426）：P0/P48 + P222 +
    // 迭代器循环 + char framePad 已复现帧与指令序列；残余 bound/end 槽互换。旧判词：
    // + `lea 0x48(%eax),%ecx` 单条 vs 4.4 具名局部。语义等价。
    // P190/P232：ORIG 全帧槽基比 4.4 深 4 ⇒ 死局部撑帧。
    char framePad[4];
    (void)framePad;
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)limit);
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 每处 map 访问独立调 GetRandomOption。
    std::map<RandomApplicationKey, BaseItemSelectionValue>::iterator it =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map48.find(key);
    // P222：ORIG `je <fail 岛>` + body 直落 ⇒ 正向包裹。
    if (it != reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map48.end())
    {
        unsigned long bound = (unsigned long)it->second.m_fieldC;
        unsigned int r = (unsigned int)m_rand[3].randInt(bound);

        // ORIG 为迭代器循环（begin 槽 -0x28 / 每轮重调 end 槽 -0x14 /
        // operator* 取元素 / 独立 idx 槽 -0xc），非 size() 索引循环。
        unsigned int idx = 0;
        for (std::vector<unsigned int>::iterator i = it->second.m_vec0.begin();
             i != it->second.m_vec0.end(); ++i, ++idx)
        {
            if (r < *i)
            {
                return (int)idx;
            }
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 184/176（ORIG 0x085f2338）：P0/P183
    // provider 每次重取已归位；残余槽序/物化次数。旧判词：
    // （P183 族）。语义等价。
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)level);

    // [b28d-solo-RandomRentalA2] P0/P183：ORIG 每处 map 操作都重取 m_optionSet
    // 并虚调 get_option_type()（find/end 各一次，无 provider/giveMap 具名局部）。
    // CRandomOptionGive 纯虚 ORIG 名（UserMiscLink 批改名）
    std::map<RandomApplicationKey,
             std::map<int, RandomOptionGive> >::const_iterator outer =
        static_cast<CRandomOptionGive*>(m_optionSet)->get_option_type().find(key);
    if (outer ==
        static_cast<CRandomOptionGive*>(m_optionSet)->get_option_type().end())
    {
        return 0x23f;
    }

    // ORIG 具名常量局部（movl $0xffffffff,-0x48(%ebp)）。
    int defaultKey = -1;
    std::map<int, RandomOptionGive>::const_iterator g =
        outer->second.find(defaultKey);
    if (g == outer->second.end())
    {
        g = outer->second.find(group);
        if (g == outer->second.end())
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 212/220（ORIG 0x085f25b0）：P0/P48 五处
    // GetRandomOption 内联 + w 标量局部已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 5 处独立调 GetRandomOption
    // （两次 find + 两次 end + 循环内 m_veca8 基址），无具名 script 槽。
    unsigned char keyBytes[4];
    keyBytes[0] = (unsigned char)rarity;
    keyBytes[1] = 0xff;
    keyBytes[2] = (unsigned char)grade;
    keyBytes[3] = optType;

    int mapKey;
    std::memcpy(&mapKey, keyBytes, 4);
    std::map<int, OptionQuantityValue>::iterator it =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapb4.find(mapKey);
    if (it == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
    {
        keyBytes[1] = (unsigned char)level;
        std::memcpy(&mapKey, keyBytes, 4);
        it = reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapb4.find(mapKey);
        if (it == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
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
        // ORIG：每轮一次 operator[]，值存 -0x14 后 push_back / total 各读一次
        // （P0 有槽往返 ⇒ 具名标量局部 w）。
        unsigned int w = reinterpret_cast<RandomOptionScript*>(GetRandomOption())
                             ->m_veca8[i]
                             .m_weight4;
        weights.push_back(w);
        total += w;
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 193/172（ORIG 0x085f2882）：P0/P183
    // 4 处 provider 虚调内联已归位；残余槽序/物化次数。旧判词：
    // （P183 族）。语义等价。
    unsigned char keyBytes[8];
    keyBytes[0] = (unsigned char)rarity;
    keyBytes[1] = 0xff;
    keyBytes[2] = (unsigned char)grade;
    keyBytes[3] = (unsigned char)optType;
    *(unsigned int*)(keyBytes + 4) = (unsigned int)group;   // ORIG mov 0x1c(%ebp)→键区+4

    // [b28d-solo-RandomRentalA2] P0/P183：ORIG 4 处独立重取 m_optionSet 并虚调
    // get_option_group_choice()（find/end ×2），无 provider/choiceMap 具名局部。
    // ORIG find 直传 &键区 8 字节引用（无中间 u64 拷贝）。
    std::map<unsigned long long, OptionGroupChoiceValue>::iterator it =
        static_cast<CRandomOptionGive*>(m_optionSet)
            ->get_option_group_choice()
            .find(*reinterpret_cast<const unsigned long long*>(keyBytes));
    if (it == static_cast<CRandomOptionGive*>(m_optionSet)
                  ->get_option_group_choice()
                  .end())
    {
        keyBytes[1] = (unsigned char)level;
        it = static_cast<CRandomOptionGive*>(m_optionSet)
                 ->get_option_group_choice()
                 .find(*reinterpret_cast<const unsigned long long*>(keyBytes));
        if (it == static_cast<CRandomOptionGive*>(m_optionSet)
                      ->get_option_group_choice()
                      .end())
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 96/100（ORIG 0x085f2b02）：残差=帧
    // 0xa8 vs 0xb8 + 槽序（N82/P112）。旧判词：
    // + 槽序。语义等价。
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 210/196（ORIG 0x085f39d2）：P0/P48 四处
    // GetRandomOption 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 4 处独立调 GetRandomOption
    // （set200 find/end + map1e8 find/end），无具名 script 槽。
    ENUM_RANDOM_OPTION_NUMBER slot = (ENUM_RANDOM_OPTION_NUMBER)optionType;

    if (options->get_option_index(slot) == 0)
    {
        return 0x17;
    }

    // set<unsigned long> @+0x200：禁重置道具黑名单；命中即拒绝。
    unsigned long idx = (unsigned long)options->get_option_index(slot);
    if (reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_set200.find(idx) !=
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_set200.end())
    {
        return 0x17;
    }

    // map<unsigned char, ResetValues> @+0x1e8：按稀有度取重置区间。
    std::map<unsigned char, ResetValues>::iterator rv =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())
            ->m_map1e8.find((unsigned char)rarity);
    if (rv == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map1e8.end())
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 170/173（ORIG 0x085f1802）：P0/P48
    // table/script 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
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

    // ORIG：外层 map<int,...> 键 = BaseItemKey 小端 int 打包
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 2 处独立调 GetRandomOption
    // （find/end 各一），无 table/script 具名局部。
    std::map<int, std::map<int, BaseItemValue> >::const_iterator outer =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())
            ->m_baseItems60.find(*(int*)&key);

    if (outer == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_baseItems60.end())
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 434/430（ORIG 0x085f2cc6）：P0/P48 四处
    // GetRandomOption 内联已归位；残余槽序/物化次数。旧判词：
    // （P183 族）。语义等价。
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
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 4 处独立调 GetRandomOption
    // （mapf4 find/end + map160 find/end），无具名 script 槽。
    DifferentWeightValue dwTmp;  // ORIG 未使用临时（O0 保留 ctor call）
    unsigned short dwRaw =
        (unsigned short)(((unsigned short)(unsigned char)rarity) |
                         ((unsigned short)outGrade << 8));
    std::map<short, DifferentWeightValue>::iterator dw =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapf4.find((short)dwRaw);
    if (dw == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapf4.end())
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
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map160.find(groupId);
    if (ogIt == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map160.end())
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 506/486（ORIG 0x085f1bb0）：P0/P48 四处
    // GetRandomOption 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
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

    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 4 处独立调 GetRandomOption
    // （mapf4 find/end + map160 find/end），无具名 script 槽。
    DifferentWeightValue dwTmp;  // ORIG 未使用临时（O0 保留 ctor call）
    unsigned short dwRaw =
        (unsigned short)(((unsigned short)(unsigned char)rarity) |
                         ((unsigned short)outGrade << 8));
    std::map<short, DifferentWeightValue>::iterator dw =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapf4.find((short)dwRaw);
    if (dw == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapf4.end())
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
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map160.find(groupId);
    if (ogIt == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map160.end())
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 99/112（ORIG 0x085f1a62）：P0/P48 四处
    // GetRandomOption 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 4 处独立调 GetRandomOption
    // （两次 find + 两次 end），无具名 script 槽。
    unsigned char keyBytes[4];
    keyBytes[0] = (unsigned char)rarity;
    keyBytes[1] = 0xff;
    keyBytes[2] = (unsigned char)grade;
    keyBytes[3] = optType;

    int mapKey;
    std::memcpy(&mapKey, keyBytes, 4);
    std::map<int, OptionQuantityValue>::iterator it =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapb4.find(mapKey);
    if (it == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
    {
        keyBytes[1] = (unsigned char)level;
        std::memcpy(&mapKey, keyBytes, 4);
        it = reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapb4.find(mapKey);
        if (it == reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
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
    // ORIG 0x085f853c：直测 bool（test;je 早退），真路径 fall-through ⇒ 正向
    // if 包裹（P1/P19 块序，消 4.4 的 xor;jne + 尾 jmp）。
    if (user->isGMUser())
    {
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
}

// ---- _Select_Part —— T 0x085f8ec2（报告缺失，objdump 直转）----------------
void RegenerationROI::_Select_Part(int partIdx, std::vector<int>& out)
{
    // [b28d-solo-RandomRentalA2 判词] DIFF 132/131（ORIG 0x085f8ec2）：P0/P183
    // names/nameTable 内联已消引用缓存；残余帧/槽序。旧判词：
    // + 局部槽序。语义等价。
    // P190：ORIG 槽基比 4.4 深 0x10 ⇒ 取址 double 撑帧。
    double framePad1;
    (void)&framePad1;
    char framePad2[4];
    (void)framePad2;
    std::map<int, str_index_to_itemgroup>::iterator it =
        m_regenerator.m_map0.find(partIdx);

    std::string groupName;
    if (it != m_regenerator.m_map0.end())
    {
        // [b28d-solo-RandomRentalA2] P0/P183：ORIG 每轮重取 it->second.m_vec8
        // （`call it->second; add $0xc`）且 getItemGroupNameTable() 两次独立调用
        // （find/end 各一次），无 names/nameTable 引用缓存局部。
        for (std::size_t i = 0; i < it->second.m_vec8.size(); ++i)
        {
            groupName = it->second.m_vec8[i];
            std::map<std::string, int>::const_iterator f =
                getItemGroupNameTable().find(groupName);
            if (f != getItemGroupNameTable().end())
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 246/236（ORIG 0x085f9094）：残余槽序/
    // 物化次数（N82/P183 族）。旧判词：
    // 语义等价。
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 202/204（ORIG 0x085f93cc）：残余槽序/
    // 物化次数（N82 族）。旧判词：
    // 语义等价。
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

    /* ORIG 槽复用（/tmp/p26_reg.cpp probeV2 实证逐槽一致）：4.1 把 randInt
     * 返回值 r 写回 total 槽（lifetime 不重叠）；独立具名 r 会让 4.4 多占
     * 一槽、全帧槽号错位。 */
    unsigned long bound = (unsigned long)total;
    total = m_regeneratorPtr->randInt(bound);

    int acc = 0;
    for (int i = lo - 1; i < hi; ++i)
    {
        acc += m_regenerator.m_vec30[i];
        if (total <= acc)
        {
            return i + 1;
        }
    }
    return m_regenerator.m_vec30[lo];
}

// ---- RegenVer_get_option_value —— T 0x085f8da6（同名报告全文转写）---------
int RegenerationROI::RegenVer_get_option_value(int minV, int maxV)
{
    // [b28d-solo-RandomRentalA2 判词] DIFF 92/117（ORIG 0x085f8da6）：残余槽序/
    // 物化次数（N82 族；ratios 内联实测反劣 117→118 已回退）。旧判词：
    // 语义等价。
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 435/416（ORIG 0x085f8646）：P282/P48
    // 双 GetRandomOptionGrade 虚调已补齐（vptr+0x50 = _ZNK5CItem20GetRandomOptionGradeEv，
    // vtable word22 实证）；残余槽序/物化次数。旧判词：
    // （P183 族，大函数按 §6.2 三件套口径）。语义等价。
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
    // [b28d-solo-RandomRentalA2] P167：ORIG 日志重载 = _ZNSolsEm(ulong)/
    // _ZNSolsEi(int)/_ZNSolsEt(ushort)/_ZNSolsEi(int) —— 逐值对齐重载。
    ss << "base item index : " << (unsigned long)chosenIdx << "\n";

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

    ss << "rarity : " << rarity << " level : " << (unsigned short)usableLevel
       << " base item item : " << optionGrade << "\n";

    outItem.reset();
    outItem.m_addInfo = chosenIdx;
    G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, outItem,
                                              1);

    bool ok = handle.RegenVer_give_option(
        // [b28d-solo-RandomRentalA2] P48：ORIG 在此处重新调用 get_rarity/
        // getUsableLevel（不用上方日志用的缓存局部），实参求值序右→左。
        (unsigned long)chosenIdx, (ENUM_RARITY)base->get_rarity(),
        base->getUsableLevel(), base->getItemGroupName(),
        base->GetRandomOptionGrade(),
        &outItem.m_random.m_random, this, optType);

    if (ok)
    {
        for (int i = 0; i < 3; ++i)
        {
            const unsigned char* slot =
                reinterpret_cast<const unsigned char*>(&outItem) + 0x25 + i * 3;
            // ORIG 循环三值走 operator<<(int)（显式 (int) 提升，避免选到
            // char/signed char 重载）。
            ss << "[" << i << "] :" << (int)slot[0] << ":" << (int)slot[1]
               << ":" << (int)slot[2] << "\n";
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

// m_map0 查 (rarity,level)：命中时 roll=randInt(m_field4+m_field0)，
// m_field4 < roll → 1（应用），否则 0；未命中 0。
// （ORIG 反汇编实证：bound=pt()+0x8 + pt()+0x4（value.m_field4+m_field0），
//   阈值=pt()+0x8（m_field4）；2026-08-30 修正字段选择。）
int CRandomOptionItemHandle::_random_application(ENUM_RARITY rarity, int level)
{
    // [b28d-solo-RandomRentalA2 判词] NEAR 67/67（ORIG 0x085f0ec2）：P0/P48 双
    // GetRandomOption + unsigned long bound + char framePad 已复现指令序列；
    // 残余 bound/end 槽互换（N93/N82）。旧判词：
    // + 槽序。语义等价。
    // P190/P232：ORIG 槽基比 4.4 深 4 ⇒ 死局部撑帧对齐。
    char framePad[4];
    (void)framePad;
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)level);
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 两次独立调用 GetRandomOption
    // （find / end 各一次，`mov %eax,%edx` 寄存器直传无槽），去 table 局部。
    std::map<RandomApplicationKey, RandomApplicationValue>::iterator it =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map0.find(key);
    if (it != reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_map0.end())
    {
        // ORIG 只占一个 bound 槽（lea -0x10(%ebp),%edx 直取址传引用）⇒ 原码
        // bound 即 unsigned long（randInt(const unsigned long&)）。
        const unsigned long bound = it->second.m_field4 + it->second.m_field0;
        const unsigned long roll =
            static_cast<unsigned long>(m_rand->randInt(bound));
        if (static_cast<unsigned int>(it->second.m_field4) < roll)
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
    // [b28d-solo-RandomRentalA2 判词] DIFF 201/205（ORIG 0x085f1560）：P0/P48
    // table 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
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
            // ORIG：外层 map<int,...> 键 = BaseItemKey 小端 int 打包
            // [b28d-solo-RandomRentalA2] P0/P48：ORIG 2 处独立调 GetRandomOption。
            std::map<int, std::map<int, BaseItemValue> >::iterator it =
                reinterpret_cast<RandomOptionScript*>(GetRandomOption())
                    ->m_baseItems60.find(*(int*)&key);
            if (it == reinterpret_cast<RandomOptionScript*>(GetRandomOption())
                          ->m_baseItems60.end())
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
