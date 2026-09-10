// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RandomRentalA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
//
// [Lane13 2026-09-10 归零记录] 四数 identical 295 / ae 334 / near 1 / diff 7
//   （diff 含 `_Z41__static_initialization_and_destruction_0ii` N123 撞名伪差）
//   本轮 AE 归零件（原 DIFF/NEAR）：_Select_Part、_Calculate_RegenCost、
//   reset_option、_get_option_quantity、_choose_option_group、
//   RegenVer_get_option_quantity、_RandomLevel、RegenVer_choose_item。
//   命中范式：① 具名 functor（CRandomList/CRandomListEx）→ 全表达式临时链
//   （P278/P221：ORIG 把 functor 构到浅槽、get_value 直作 for_each 返回值 sret）；
//   ② 迭代器比较写 `end() != it`（P74补：控实参装载序）；③ 具名 bound/常量局部
//   → randInt 内联实参（P0/P236，匿名临时落浅槽）；④ 删除误判的 P190 framePad；
//   ⑤ 共享头返回类型旋钮（P211/P292/P248档①）：CDataManager.h
//   `RegenVer_get_option_quantity` int→unsigned、CItemGenerator.h
//   `_random_application` int→bool。⑥ Main 已落 c6446r(446) 路由（N7 虚调寄存器角色）。
//   ⑦ [Main 视图 cast 反模式] give_option / RegenVer_give_option 的
//   `unsigned char* optB=(unsigned char*)opts` 字节视图写入 → 权威头具名字段
//   `opts->m_fields[slotI].m_field{0,1,2}` / `opts->m_seed9.m_field`（紧凑 3 字节组
//   视图，零布局变化）；l6 归一化差异 390→343 / 424→415、我方指令数更贴 ORIG。
//   ⑧ [Main 批准跨 TU] `CItemList.h` BaseItemKey 加匿名 union 双视角 `int m_key`
//   （sizeof 仍 4、偏移/mangled 零变化）⇒ choose_item / RegenVer_choose_item 的
//   `*(int*)&key` / `reinterpret_cast<const int&>(key)` 改 `key.m_key`
//   （RegenVer_choose_item 本件已 AE 归零）。
//   剩余真缺口 = 7（NEAR `_get_option_type` 4.1 两段迭代器直构族；DIFF
//   `choose_item`（4.4 cross-jumping 合并 `rarity=0` 失败块 + 槽位）、
//   `give_option` / `RegenVer_give_option` / `Regenerate` / `_get_option_value` /
//   `RegenVer_get_option_value`（槽基常数 N126 族））。
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
}

// ---------------------------------------------------------------------------
// RandomOption 访问器族（报告缺失，objdump 直转；W 符号实证）
// ---------------------------------------------------------------------------

#include "RandomRentalA2/get_option_index.inc"
}

#include "RandomRentalA2/set_first_value.inc"
}

#include "RandomRentalA2/set_second_value.inc"
}

// [b28d-solo-RandomRentalA2] P210：change_option 定义后置于三个调用者之后，
// 使 4.4 对仅声明的 callee 走保守帧（get 0x18 / set 0x28，v spill@-0xc），
// 与 ORIG 4.1 完全一致（探针 probe4.cpp c1 实证）。
#include "RandomRentalA2/change_option.inc"
}

// [b28d-solo-RandomRentalA2 P210] empty() 定义后置于 change_option 之后：
// 4.4 对「本 TU 已见定义的 callee」精确继承 outgoing（change_option 帧塌缩
// sub $0x4），ORIG 4.1 保守 sub $0x18；定义后置使 4.4 只见声明 ⇒ 恢复保守帧
// （探针 /tmp/rr_probe4.cpp f0 实证：未知 callee 时 4.4 恒 sub $0x18）。
#include "RandomRentalA2/empty.inc"
}

#include "RandomRentalA2/DifferentWeightValue.inc"
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

#include "RandomRentalA2/_get_level_limit.inc"
}

// ---- _get_part_type —— T 0x085f1088（_get_part_type.md 全文转写）----------
#include "RandomRentalA2/_get_part_type.inc"
}

// ---- _get_part_type_by_ItemGroup —— T 0x085f11a4（同名报告全文转写）-------
#include "RandomRentalA2/_get_part_type_by_ItemGroup.inc"
}

// ---- _get_base_item —— T 0x085f1426（同名报告全文转写）-------------------
#include "RandomRentalA2/_get_base_item.inc"
}

// ---- _get_option_type —— T 0x085f2338（同名报告全文转写）-----------------
// 经 script+0x148 provider 虚槽0 取 map<RAKey, map<int,RandomOptionGain>>；
// 内层先 find(-1)，未命中再 find(group)。
// 【运行期 blocked】provider 由 importRandomOptionScript 批次构造。
#include "RandomRentalA2/_get_option_type.inc"
}

// ---- _get_option_quantity —— T 0x085f25b0（同名报告全文转写）-------------
// 键 = int{b0=rarity, b1=ff|level, b2=grade, b3=optType}
#include "RandomRentalA2/_get_option_quantity.inc"
}

// ---- _choose_option_group —— T 0x085f2882（同名报告全文转写）-------------
// u64 键 = {rarity, ff|level, grade, optType} 低 4 字节 + dword(group) 高 4 字节
// （报告 asm +0x01f/+0x022：mov 0x1c(%ebp),%eax; mov %eax,-0x58(%ebp)，group 整体
// 写入键区 +4..+7）。经 provider 虚槽1 取表。
// 【运行期 blocked】同 _get_option_type 注。
#include "RandomRentalA2/_choose_option_group.inc"
}

// ---- _get_option_value —— T 0x085f2b02（同名报告全文转写）----------------
#include "RandomRentalA2/_get_option_value.inc"
}

// ---- reset_option —— T 0x085f39d2（reset_option.md 全文转写）-------------
#include "RandomRentalA2/reset_option.inc"
}

// ---- RegenVer_choose_item —— T 0x085f1802（同名报告全文转写）-------------
#include "RandomRentalA2/RegenVer_choose_item.inc"
}

// ---- give_option —— T 0x085f2cc6（ORIG 独立完整实现，逐段转写）----------
// 注意：辅助调用参数顺序按 ORIG 反汇编实证——
//   _get_option_type 第4参=grade（内层 find 键，非 group）；
//   _get_option_quantity 键=(rarity,ff,outGrade,level)，回退 b1=grade；
//   _choose_option_group 键=(rarity,ff,group,level)+u32(outGrade)。
#include "RandomRentalA2/give_option.inc"
}

// ---- RegenVer_give_option —— T 0x085f1bb0（ORIG 独立完整实现，逐段转写）--
// 数量入口 optType==0x65 走 RegenVer_get_option_quantity（键构建同
// _get_option_quantity：(rarity,ff,outGrade,level)，回退 b1=grade）；
// 数值出口 optType==0xc9 走 roi->RegenVer_get_option_value，否则
// _get_option_value。logFormat 行号/PRETTY 与 give_option 不同。
#include "RandomRentalA2/RegenVer_give_option.inc"
}

// ---- RegenVer_get_option_quantity —— T 0x085f1a62（同名报告全文转写）------
#include "RandomRentalA2/RegenVer_get_option_quantity.inc"
}

}  // namespace random_option

// ===========================================================================
// RegenerationROI 再铸链
// ===========================================================================

// ---- SendMessageToCUser —— T 0x085f853c（报告缺失，objdump 直转）----------
#include "RandomRentalA2/SendMessageToCUser.inc"
}

// ---- _Select_Part —— T 0x085f8ec2（报告缺失，objdump 直转）----------------
#include "RandomRentalA2/_Select_Part.inc"
}

// ---- _Calculate_RegenCost —— T 0x085f9094（同名报告全文转写）--------------
#include "RandomRentalA2/_Calculate_RegenCost.inc"
}

// ---- _RandomLevel —— T 0x085f93cc（报告缺失，objdump 直转）----------------
#include "RandomRentalA2/_RandomLevel.inc"
}

// ---- RegenVer_get_option_quantity —— T 0x085f8cb4（同名报告全文转写）------
// [Lane13] 返回类型 int→unsigned int（CDataManager.h 同步改）：ORIG 调用点
// 0x085f8cbd 段为 `call; mov %eax,%edx; mov 0x1c(%ebp),%eax; mov %edx,(%eax)`
// = RHS-first + eax→edx 中转（P211 符号性异型），callee 自身尾部
// `call vector::ix; mov (%eax),%eax; leave; ret` 无扩展 ⇒ 原码返回 unsigned。
#include "RandomRentalA2/RegenVer_get_option_quantity_1.inc"
}

// ---- RegenVer_get_option_value —— T 0x085f8da6（同名报告全文转写）---------
#include "RandomRentalA2/RegenVer_get_option_value.inc"
}

// ---- Regenerate —— T 0x085f8646（Regenerate.md 全文转写）------------------
#include "RandomRentalA2/Regenerate.inc"
}

namespace random_option
{

// m_map0 查 (rarity,level)：命中时 roll=randInt(m_field4+m_field0)，
// m_field4 < roll → 1（应用），否则 0；未命中 0。
// （ORIG 反汇编实证：bound=pt()+0x8 + pt()+0x4（value.m_field4+m_field0），
//   阈值=pt()+0x8（m_field4）；2026-08-30 修正字段选择。）
// [Lane13] 返回类型 int→bool（CItemGenerator.h 同步改）：ORIG choose_item 0x085f163d
// 调用点 `call; xor $0x1,%eax; test %al,%al; je` = bool 直测（P292/P248档①）。
#include "RandomRentalA2/_random_application.inc"
}

// ---- choose_item —— T 0x085f1560（choose_item.md 全文转写）----
// m_disabled 非 0 → -1；level>9 时向下取整到十位（钳 0x50）；
// level<10 且 rarity==2 → rarity 清 0 返 -1；_random_application 通过后
// 按 (rarity,levelLimit,partType) 查 m_baseItems60，取第 baseItem 项
// （越界回退末项）的 BaseItemValue::m_field0；任何失配 rarity 清 0 返 -1。
#include "RandomRentalA2/choose_item.inc"
}

}  // namespace random_option

// ---- ORIG W 弱符号发射段（RandomOptionScript.h 头内联定义，本 TU 无 odr-use）----
//   ResetValues C1/C2 @0x08a73df6（vector ctor + clear）、C1ERKS/C2ERKS @0x08a74cda、
//   clear @0x08a73e38、D1/D2 @0x083cf4b4（仅 vector dtor）。
// 真实构造/拷贝/析构强制发射，不引入导出符号。
namespace
{
#include "RandomRentalA2/emitResetValuesWeakSymbols.inc"
}
}
