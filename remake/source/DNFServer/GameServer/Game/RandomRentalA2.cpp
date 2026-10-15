#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RandomRentalA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "RandomRentalA2/select_base_item.inc"


// ---------------------------------------------------------------------------
// RandomOption 访问器族（报告缺失，objdump 直转；W 符号实证）
// ---------------------------------------------------------------------------

#include "RandomRentalA2/get_option_index.inc"


#include "RandomRentalA2/set_first_value.inc"


#include "RandomRentalA2/set_second_value.inc"


#include "RandomRentalA2/change_option.inc"


#include "RandomRentalA2/empty.inc"
{
    return m_optionIndex == 0;  // W 0x811ed22：movzbl (%eax); test; sete
}

#include "RandomRentalA2/DifferentWeightValue.inc"
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
// [R25a06 2026-09-13 · §31.2 线索 #2 现场读数 · 数据宽度对齐（形态）]
//   ORIG nm -S + raw 双侧：`_ZN13random_optionL16MAX_OPTION_VALUEE` 1B=0x64、
//   `L15MAX_RANDOM_SEEDE` 1B=0x40、`L13kMaxItemLevelE` 2B=0x0050、
//   `L15KItemStartLevelE` 2B=0x000a、`L18KItemLevelIntervalE` 2B=0x000a
//   （ORIG .rodata 连续区 0x08af2ddc..0x08af2de3，地址升序 = 声明逆序）；
//   我方原 5×`int`（4B）⇒ 宽度偏大。改后值逐字节不变、恒等值不可观察
//   （无指令引用该宽度的可观察后果）⇒ **非语义 ④**，属形态对齐。
//   注：ORIG 该 `_ZL` 副本 315 份（每 TU 发射）vs 我方 1 份 = 4.1↔4.4
//   static-const 发射差，登记不修。
static const short kMaxItemLevel = 80;
static const short KItemStartLevel = 10;
static const short KItemLevelInterval = 10;
static const unsigned char MAX_RANDOM_SEED = 64;
static const unsigned char MAX_OPTION_VALUE = 100;

#include "RandomRentalA2/_get_level_limit.inc"


#include "RandomRentalA2/_get_part_type.inc"


#include "RandomRentalA2/_get_part_type_by_ItemGroup.inc"


#include "RandomRentalA2/_get_base_item.inc"


#include "RandomRentalA2/_get_option_type.inc"


#include "RandomRentalA2/_get_option_quantity.inc"
                                                  int grade, int optType,
                                                  int& outQuantity)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 212/220；ORIG 0x085f25b0）：P0/P48 五处
    // GetRandomOption 内联 + w 标量局部已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 5 处独立调 GetRandomOption
    // （两次 find + 两次 end + 循环内 m_veca8 基址），无具名 script 槽。
    // [SweepRandomRental-2 P82] ORIG 0x085f25b8..0x085f25cb 为四条 disp 写、
    // 键地址每次 `lea -0x54(%ebp),%edx` 重取 ⇒ 具名 4 字节 POD 视图
    // （数组 + `*reinterpret_cast<int*>` 使 4.4 把 &key 缓存进 %ebx）。
    union OptionQuantityKey
    {
        struct
        {
            unsigned char m_rarity;    // +0x0
            unsigned char m_level;     // +0x1（0xff 或 level）
            unsigned char m_grade;     // +0x2
            unsigned char m_optType;   // +0x3
        } m_parts;
        int m_key;                     // 4 字节键视图（find 直传左值）
    };
    OptionQuantityKey key;
    key.m_parts.m_rarity = (unsigned char)rarity;
    key.m_parts.m_level = 0xff;
    key.m_parts.m_grade = (unsigned char)grade;
    key.m_parts.m_optType = (unsigned char)optType;

    // [b28d-solo-RandomRentalA2 P0/P235] ORIG find 直传 &key（无 mapKey
    // 拷贝/无 memcpy 调用）；union 视图左值直绑 const&。
    // [b28d-solo-RandomRentalA2 P96/P270 反向] ORIG 走 const 版容器访问
    // （p167 MISSING `_ZNKSt3map...4find`/`_ZNKSt23_Rb_tree_const_iterator...eq`）
    // ⇒ 经 const 指针访问成员 + const_iterator。
    std::map<int, OptionQuantityValue>::const_iterator it =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.find(
            key.m_key);
    if (it == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
    {
        key.m_parts.m_level = (unsigned char)level;
        // [SweepRandomRental-2 P293 反向] ORIG 0x085f262b..0x085f2679 先
        // GetRandomOption+find（→-0x68）再 GetRandomOption+end（→-0x4c）；
        // 单表达式 `find()==end()` 令 4.4 反序成 end 先 ⇒ 具名 const_iterator。
        // [b28d-solo-RandomRentalA2 P171] 第二次 find 不回写 it（后续仍用首次）。
        std::map<int, OptionQuantityValue>::const_iterator retryIt =
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.find(
                key.m_key);
        if (retryIt ==
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
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
    // [SweepRandomRental-2] ORIG 0x085f26db/0x085f271d 为 32 位槽
    // （`mov %eax,-0xc`/`addl $0x1,-0xc`）⇒ 循环变量 int（short 形产 16 位槽）。
    // [SweepRandomRental-2] ORIG 0x085f26c1 `movl $0x0,-0x14`（w 在循环外
    // 声明并初始化 0，循环内仅赋值）⇒ 循环内声明会少一条初始化。
    unsigned int w = 0;
    for (int i = it->second.m_minA - 1; i < it->second.m_maxB; ++i)
    {
        // ORIG：每轮一次 operator[]，值存 -0x14 后 push_back / total 各读一次
        // （P0 有槽往返 ⇒ 具名标量局部 w）。
        w = reinterpret_cast<const RandomOptionScript*>(GetRandomOption())
                ->m_veca8[i]
                .m_weight4;
        // [b28d-solo-RandomRentalA2 P315/P319] ORIG push_back 选右值重载
        // `_ZNSt6vectorIjSaIjEE9push_backEOj` ⇒ 实参写 cast 成 prvalue。
        weights.push_back((unsigned int)w);
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

    // [b28d-solo-RandomRentalA2 P0] bound 匿名临时形。
    int r = m_rand[5].randInt((unsigned long)total);
    // [Lane13] P278/P221：ORIG 085f29?? 把 CRandomList 构到 -0x30（浅）、
    // for_each 返回值 sret -0x40 上直调 get_value（-0x40），非具名 chooser。
    // [Lane13] P211：ORIG 085f29f7 `lea (%ebx,%eax,1),%eax; mov %eax,%edx` ——
    // RHS 为 unsigned 表达式、目标 int ⇒ 异型触发 eax→edx 中转；突兀 `(int)` cast
    // 会把 RHS 归为 int（同型）而丢掉中转 ⇒ 删 cast 保留 unsigned 语义。
    outQuantity =
        std::for_each(weights.begin(), weights.end(),
                      CRandomList<unsigned int>((unsigned int)r))
            .get_value() +
        it->second.m_minA;
    return 0;
}

#include "RandomRentalA2/_choose_option_group.inc"
                                                  int grade, int optType,
                                                  int group, int& outGroup)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 193/172；ORIG 0x085f2882）：P0/P183
    // 4 处 provider 虚调内联已归位；残余槽序/物化次数。旧判词：
    // （P183 族）。语义等价。
    // [SweepRandomRental-2 P82] ORIG 0x085f28a1 `mov 0x1c(%ebp),%eax;
    // mov %eax,-0x58(%ebp)` 为单条 disp 写 ⇒ 键是具名 8 字节 POD 视图
    // （`*(unsigned int*)(keyBytes+4)` 生 lea+lea+mov (%edx) 三条）。
    union OptionGroupKey
    {
        struct
        {
            unsigned char m_rarity;    // +0x0
            unsigned char m_level;     // +0x1（0xff 或 level）
            unsigned char m_grade;     // +0x2
            unsigned char m_optType;   // +0x3
            unsigned int m_group;      // +0x4
        } m_parts;
        unsigned long long m_key;      // 8 字节键视图（find 直传左值）
    };
    OptionGroupKey key;
    key.m_parts.m_rarity = (unsigned char)rarity;
    key.m_parts.m_level = 0xff;
    key.m_parts.m_grade = (unsigned char)grade;
    key.m_parts.m_optType = (unsigned char)optType;
    key.m_parts.m_group = (unsigned int)group;

    // [b28d-solo-RandomRentalA2] P0/P183：ORIG 4 处独立重取 m_optionSet 并虚调
    // get_option_group_choice()（find/end ×2），无 provider/choiceMap 具名局部。
    // ORIG find 直传 &键区 8 字节引用（无中间 u64 拷贝）。
    // [b28d-solo-RandomRentalA2 P96] ORIG 把非 const `find()` 结果赋给
    // const_iterator（p167 MISSING `C1ERKSt17_Rb_tree_iterator` 转换构造 +
    // `_ZNKSt23_Rb_tree_const_iterator...ptEv/eq` + const vector begin/end/for_each/at）
    // ⇒ 保持非 const provider，仅把 it 声明为 const_iterator。
    std::map<unsigned long long, OptionGroupChoiceValue>::const_iterator it =
        static_cast<CRandomOptionGive*>(m_optionSet)
            ->get_option_group_choice()
            .find(key.m_key);
    if (it == static_cast<CRandomOptionGive*>(m_optionSet)
                  ->get_option_group_choice()
                  .end())
    {
        key.m_parts.m_level = (unsigned char)level;
        // [SweepRandomRental-2 P293 反向] ORIG 0x085f2940..0x085f29d2 为
        // 虚调#3+find（→-0x48，const_iterator ctor →-0x64）**再** 虚调#4+end
        // （→-0x40，ctor →-0x44），比较 (find,end)；单表达式 `find()==end()`
        // 令 4.4 反序成 end 先/find 后 ⇒ 具名 const_iterator 形。
        // [b28d-solo-RandomRentalA2 P171] 第二次 find 不回写外层 it（后续仍用
        // 首次迭代器）。
        std::map<unsigned long long, OptionGroupChoiceValue>::const_iterator retryIt =
            static_cast<CRandomOptionGive*>(m_optionSet)
                ->get_option_group_choice()
                .find(key.m_key);
        if (retryIt == static_cast<CRandomOptionGive*>(m_optionSet)
                           ->get_option_group_choice()
                           .end())
        {
            return 0x29e;
        }
    }

    // [SweepRandomRental-2 P183] ORIG 每处重取 it->second（ptEv + 偏移折叠：
    // m_field18→+0x20、m_vecC→+0x14、m_vec0→+0x8），无 OptionGroupChoiceValue&
    // 缓存（旧形 add $0x8 落 -0x14 槽并多次回读）。
    // [b28d-solo-RandomRentalA2 P0] bound 匿名临时形。
    int r = m_rand[6].randInt((unsigned long)(it->second.m_field18 - 1));

    // [Lane13] P278/P221：同族——chooser 匿名临时，get_value 作用 for_each 返回值。
    unsigned int sel =
        std::for_each(it->second.m_vecC.begin(), it->second.m_vecC.end(),
                      CRandomList<unsigned int>((unsigned int)r))
            .get_value();
    // [SweepRandomRental-2] ORIG 0x085f2ac5 `cmp -0xc(%ebp),%eax; seta %al`
    // ⇒ 源码正极性 `size() > sel`（`sel >= size()` 早返形产 setbe + 反块序）。
    if (it->second.m_vec0.size() > sel)
    {
        outGroup = it->second.m_vec0.at(sel);
        return 0;
    }
    return 0x2be;
}

#include "RandomRentalA2/_get_option_value.inc"


#include "RandomRentalA2/reset_option.inc"


#include "RandomRentalA2/RegenVer_choose_item.inc"


#include "RandomRentalA2/give_option.inc"


#include "RandomRentalA2/RegenVer_give_option.inc"


#include "RandomRentalA2/RegenVer_get_option_quantity.inc"
    ENUM_RARITY rarity, int level, int grade, int optType, int& outQuantity,
    RegenerationROI* roi)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 99/112；ORIG 0x085f1a62）：P0/P48 四处
    // GetRandomOption 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 4 处独立调 GetRandomOption
    // （两次 find + 两次 end），无具名 script 槽。
    // [SweepRandomRental-2 P82] 同 _get_option_quantity：具名 4 字节 POD 视图
    // （数组形使 4.4 把 &key 缓存进 %ebx，键地址装载序亦异）。
    union OptionQuantityKey
    {
        struct
        {
            unsigned char m_rarity;    // +0x0
            unsigned char m_level;     // +0x1（0xff 或 level）
            unsigned char m_grade;     // +0x2
            unsigned char m_optType;   // +0x3
        } m_parts;
        int m_key;                     // 4 字节键视图（find 直传左值）
    };
    OptionQuantityKey key;
    key.m_parts.m_rarity = (unsigned char)rarity;
    key.m_parts.m_level = 0xff;
    key.m_parts.m_grade = (unsigned char)grade;
    key.m_parts.m_optType = (unsigned char)optType;

    // [b28d-solo-RandomRentalA2 P0/P235] 同 _get_option_quantity：去 memcpy。
    std::map<int, OptionQuantityValue>::const_iterator it =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.find(
            key.m_key);
    if (it == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
    {
        key.m_parts.m_level = (unsigned char)level;
        // [SweepRandomRental-2 P293 反向] ORIG 先 find 再 end（同
        // _get_option_quantity）⇒ 具名 const_iterator。
        // [b28d-solo-RandomRentalA2 P171] 第二次 find 的结果只用于与 end
        // 比较、**不回写 it**（Ghidra: find(local_20) vs end(local_10)，后续
        // operator-> 仍取 local_1c=首次迭代器）⇒ 照抄原码遗留行为。
        std::map<int, OptionQuantityValue>::const_iterator retryIt =
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.find(
                key.m_key);
        if (retryIt ==
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
        {
            return 0x1ad;
        }
    }

    if (it->second.m_maxB == 0)
    {
        return 0x1b1;
    }

    // [Lane13] P211：ORIG 085f?? `call; mov %eax,%edx; mov 0x1c(%ebp),%eax;
    // mov %edx,(%eax)` —— RHS 与 int 目标符号性异型 ⇒ RHS-first + eax→edx 中转；
    // 显式 `(unsigned int)` cast 复现该异型（同族见 _get_option_quantity）。
    outQuantity = (unsigned int)roi->RegenVer_get_option_quantity(
        it->second.m_minA, it->second.m_maxB);
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

#include "RandomRentalA2/SendMessageToCUser.inc"


#include "RandomRentalA2/_Select_Part.inc"


#include "RandomRentalA2/_Calculate_RegenCost.inc"


#include "RandomRentalA2/_RandomLevel.inc"


#include "RandomRentalA2/RegenVer_get_option_quantity_1.inc"


#include "RandomRentalA2/RegenVer_get_option_value.inc"


#include "RandomRentalA2/Regenerate.inc"


namespace random_option
{

#include "RandomRentalA2/_random_application.inc"


#include "RandomRentalA2/choose_item.inc"


}  // namespace random_option

// ---- ORIG W 弱符号发射段（RandomOptionScript.h 头内联定义，本 TU 无 odr-use）----
//   ResetValues C1/C2 @0x08a73df6（vector ctor + clear）、C1ERKS/C2ERKS @0x08a74cda、
//   clear @0x08a73e38、D1/D2 @0x083cf4b4（仅 vector dtor）。
// 真实构造/拷贝/析构强制发射，不引入导出符号。
namespace
{
#include "RandomRentalA2/emitResetValuesWeakSymbols.inc"

}
