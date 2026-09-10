// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WongWorkMisc_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// WongWorkMisc_A.cpp —— WongWork 域 A 段 G6 杂项组独占 TU（WongWorkA 批次）。
//
// 承载 WongWork::CCalcCharacStat 7 方法（全 T）+ 本 TU 的 GI/static_init
// （C++98 / 32位 / O0）。每函数按 docs/class_func_reports/<类>/<func>.md 的
// ## 汇编 + ## 反编译 C 逐指令转写；报告缺失者以 objdump ORIG 为准并已标注地址。
//
// [SweepWongWorkEvents 2026-09-10 两处真缺口修复]
// ① P338/P327：ORIG 本 TU 有 `t _GLOBAL__I_…ApplyElixerItem…`（0x083201f4，体
//    `sub $0x18`）与 `t _Z41__static_initialization_and_destruction_0ii`（0x083201b4，
//    体只含 `ios_base::Init::C1` + `__cxa_atexit`，即 `_ZStL8__ioinit`）。我方原缺
//    这两个符号 ⇒ 补 `#include <iostream>`（发射 ioinit/GI），并把 ApplyElixerItem
//    定义移到 TU 最前（它是 ORIG 该 TU 地址最小的强符号，P327 决定 GI 键名）。
//    修后 .o 的 GI 体 8/8 与 ORIG identical、static_init 16/16 逐条同构
//    （check_tu 的 static_init DIFF 系 N123 每 TU 同名 local 撞名假差）。
// ② 判据㉓/㉔：CMCAPManager 7 方法在 ORIG 全为 W（头内 inline，由消费 TU 发射；
//    ORIG 本 TU 区间 0x0831e85c..0x08320210 内无这些符号）⇒ 定义已迁回
//    CMCAPManager.h 头内，本 TU 不再持有（原 .cpp 强定义使绑定为 T）。
//    ORIG 本 TU 区间符号差集 ORIG-only=0；sizeof/成员偏移/mangled/虚槽零变化。
// 本 TU 现有 dual_rank 候选 5 件（gap' 10/8/7/6/2）全为 N 封顶（见各函数判词）；
// `call *` 0 处（ORIG 本 TU 亦 0）；jcc 全 TU 逐位核对：助记符序列 0 差，
// 目标偏移差仅由函数体长度差（+6/+5B）引起。
// ============================================================================

#include <algorithm>
#include <iostream>            // P338：ORIG 本 TU 有 _ZStL8__ioinit + _GLOBAL__I_ 体 sub $0x18
                               // （static_init 只含 ios_base::Init::C1 + __cxa_atexit），
                               // 缺此 include 则 4.4 不发射 GI/static_init 两个 ORIG 实有符号。

#include "CCharacter.h"        // _Additioal_info（全局 ns，0x52 pack(1)）/ CCharacter
#include "CMCAPManager.h"      // WongWork::CMCAPManager
#include "CDataManager.h"      // G_CDataManager() / m_characList（+0x14）
#include "CCharacList.h"       // CCharacList::m_characs[11]（+0x000，步长 0x7dc）

namespace WongWork
{

// ============================================================================
// CCalcCharacStat —— 角色基础属性计算器
// ============================================================================

// ORIG T 0x0831e85c _ZN8WongWork15CCalcCharacStat15ApplyElixerItemEP15_Additioal_infoii
// 报告：ApplyElixerItem.md。灵药道具按 statIdx 选择属性槽做加法（u16 回绕）；
// statIdx>9 静默忽略；case9 对 m_field10[0..3] 各加 value*10。
// ORIG 汇编不物化 (short)value：逐 case 以 32 位 value 乘 50 后 movw 截断
// （mod 2^16 与 s16 乘加等价）；case 物理布局序 = 4,6,5,7,2,3,8,9
// （ORIG 块序 +0x1f/+0x4c/+0x79/+0xa6/+0xd3/+0xea/+0x103/+0x120）。
// [SweepWongWorkEvents P327] 本函数是 ORIG 该 TU 内地址最小的强符号（0x0831e85c <
//   InitializeStat 0x0831e9ca），ORIG 的 GI 键名即它；定义置于 TU 最前使 4.4 发射的
//   _GLOBAL__I__ZN8WongWork15CCalcCharacStat15ApplyElixerItemEP15_Additioal_infoii 键名一致。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：case0..8 与 ORIG 段级逐条全等
//   （仅 jmp/jump-table 重定位伪差）；唯一残差 = case9 循环体寄存器角色互换
//   （ORIG: i→%ecx、元素值→%ebx、`lea (%ebx,%eax,1)`；4.4: i→%ebx、值→%ecx、
//   `add %eax,%ecx`+`lea 0x8(%ebx),%edx`）= N7+N18a。已试 7 形（`=`/`+=`/交换加数/
//   语句表达式/register 索引/register 元素/long 索引）+ 4.4.6 后端复测，同形 ⇒ 封顶。
//   p167: 0 missing / 0 extra；语义与 Ghidra C 逐 case 一致（mod 2^16 等价）。
#include "WongWorkMisc_A/ApplyElixerItem.inc"
}

// ORIG T 0x0831e9ca _ZN8WongWork15CCalcCharacStat14InitializeStatEP15_Additioal_infoi
// 报告：docs/class_func_reports/WongWork__CCalcCharacStat/InitializeStat.md
// 清零后按职业写入初始属性表（job 只认 0..3，其余静默返回）。
#include "WongWorkMisc_A/InitializeStat.inc"
}

// ORIG T 0x0831ec4a _ZN8WongWork15CCalcCharacStat8CalcStatEP15_Additioal_infoiiiii
// 报告：CalcStat.md。清零 → 累加角色表基行（&characTable[job]，即 m_baseInfo[0]）
// → 取三个成长块指针 (0,0)/(firstGrow,0)/(firstGrow,secondGrow) → 按 level
// 逐级累加：<15 用一转前系数块，<51 用一转块，否则二转块。
#include "WongWorkMisc_A/CalcStat.inc"
}

// —— 成长系转写辅助：ORIG 形如 ((short)(lv<<k)+t)*mul+cst 的有符号 16 位中间量
// 在等级/成长数据范围内不可能溢出 s16，先按 int 计算再截断存入 u16 字段，
// 与 mov %dx 存储语义一致（各级 _Calc* 报告反编译 C 同构）。——

// ORIG T 0x0831ed6c _ZN8WongWork15CCalcCharacStat13_CalcSwordmanEP15_Additioal_infoii
// 报告：_CalcSwordman.md。secondType 只认 0..4，越界不改写。
// [b28d-54 N] 4.1 内联 16 位算术 vs 4.4 cwtl/movswl 显式扩展 + `add imm,%eax;mov`
//   vs `lea imm(%eax)` 折叠（N1/N97 族）；语义等价。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：本棒修法——①全部 32 位字段段
//   （m_field00/04/3a/42）改用 unsigned 字面量（`+0xCxxxu`）：4.4 对 signed 常量的
//   `add imm,%eax;mov %eax,%edx` 恒折 `lea imm(%eax),%edx`，unsigned 即复现 ORIG
//   add+mov（探针 /tmp/probe_ty.cpp fC/fD、/tmp/probe_u.cpp fN/fP/fQ/fR）；②删除
//   `const short t` 具名局部与 `(short)(level<<K)+t` 形式，改写 `level*K + level`
//   （`*` 而非 `<<`）——消除帧 sub/movswl/cwtl/t 槽（ORIG 无帧 sub，P0 判无该具名
//   局部；探针 /tmp/probe_shift.cpp fB/fC）。段级 identical 12→34/47。
//   残余 N（不可翻）：16 位字段段（m_field08..0e/10/40/46/48/4a）ORIG 发 16 位
//   `add/sub $C,%ax` 或 32 位 `add $C,%eax;mov`，4.4 对 u16 目标恒折 `lea C(%eax)`
//   （8 形探针 /tmp/probe_w.cpp gA-gF、/tmp/probe_h.cpp hA-hG 全 lea）⇒ N1/N86b +
//   跨版本窄化。p167: 0 missing / 0 extra；语义与 Ghidra C 逐字段一致。
// [2026-09-10 D06 · DIFF→IDENTICAL 423=423] 旧判词"跨版本窄化不可翻"**作废**：真因是
//   **字段符号性**——ORIG 的 16 位窄化 `add $0x1770,%ax; mov %eax,%edx` 只在 **signed
//   short 视角**下由 4.4 复现（unsigned 视角恒产 `lea`，探针 /tmp/d06/p16c.cpp
//   f20..f27）；改用 CCharacter.h `_Additioal_info` 的 union 同址 signed 视角
//   （+0x40 `m_mpRegen` / +0x4a `m_hitRecovery`）+ `static_cast<short>` 后逐条全等；
//   +0x14 用新增 `m_field10s[4]`（探针 /tmp/d06/p14.cpp g0..g7 矩阵，含
//   reinterpret_cast<short&> 反例：多 `add $0x14,%eax` 基址提前物化）。P321-T1 + P235。
#include "WongWorkMisc_A/_CalcSwordman.inc"
}

// ORIG T 0x0831f282 _ZN8WongWork15CCalcCharacStat12_CalcFighterEP15_Additioal_infoii
// 报告：_CalcFighter.md。
// [b28d-54 N] 同 _CalcSwordman（短整扩展/lea 折叠，N1/N97）；语义等价。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：同 _CalcSwordman 修法（32 位字段段
//   unsigned 字面量 + `*` 化去 t 槽），段级 identical 17→33/45；残余 7 段 = 16 位
//   字段 add/sub vs lea（N1/N86b+跨版本窄化，探针见 _CalcSwordman 注释）⇒ 封顶。
//   p167: 0 missing / 0 extra；语义与 Ghidra C 逐字段一致。
// [2026-09-10 D06 · DIFF→IDENTICAL 402=402] 同 _CalcSwordman 修法（signed 视角
//   m_mpRegen/m_hitRecovery + m_field10s[2]），旧"16 位字段 add/sub vs lea ⇒ 封顶"
//   判词作废。
#include "WongWorkMisc_A/_CalcFighter.inc"
}

// ORIG T 0x0831fc3a _ZN8WongWork15CCalcCharacStat9_CalcMageEP15_Additioal_infoii
// 报告：_CalcMage.md。
// 判例（ADeepY4[5]）：DIFF(N封)，gap -14→-4（orig 458 / our 454）。去 `const short t`
//   命名局部（原在栈槽 -0x2 每用回读 movzwl），全部内联 static_cast<short>(level)，
//   消除帧槽（ORIG 无帧 sub，P0 判原码无该具名局部）+ movswl 25→12。语义与 Ghidra C 逐条
//   对应（5 case × 字段算式全同）；残余为 4.1-vs-4.4 codegen 分解（N86b）：ORIG 用
//   `add imm,%eax; mov %eax,%edx`、我们用 `lea imm(%eax),%edx`（lea 折叠少 1 条）；
//   cwtl/movswl 短整符号扩展 4.1 内联 16 位算、4.4 显式扩宽。同 TU _CalcGunner/Fighter/
//   Swordman 沿用同型（非本棒）。
// [b28d-54 N 复核] ORIG 0x0831fc3a：既有判例（ADeepY4[5]）——短整扩展/lea 折叠族，
//   语义与 Ghidra C 逐条一致。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：同 _CalcSwordman 修法，段级
//   identical 21→35/50；残余 10 段 = 16 位字段 add/sub vs lea（N1/N86b+跨版本窄化）
//   ⇒ 封顶。p167: 0 missing / 0 extra；语义与 Ghidra C 逐字段一致。
#include "WongWorkMisc_A/_CalcMage.inc"
}

// ORIG T 0x0831f75c _ZN8WongWork15CCalcCharacStat11_CalcGunnerEP15_Additioal_infoii
// 报告：_CalcGunner.md。
// [b28d-54 N] 同 _CalcMage（短整扩展/lea 折叠，N1/N97）；语义等价。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：同 _CalcSwordman 修法，段级
//   identical 15→33/44；残余 6 段 = 16 位字段 add/sub vs lea（N1/N86b+跨版本窄化）
//   ⇒ 封顶。p167: 0 missing / 0 extra；语义与 Ghidra C 逐字段一致。
// [2026-09-10 D06 · DIFF→IDENTICAL 402=402] 同 _CalcSwordman 修法（signed 视角
//   m_hitRecovery/m_mpRegen），旧"16 位字段 add/sub vs lea ⇒ 封顶"判词作废。
#include "WongWorkMisc_A/_CalcGunner.inc"
}

// ============================================================================
// CMCAPManager —— CleanPad 点数管理。
// [SweepWongWorkEvents 判据㉓/㉔ 迁移] ORIG 该 7 方法全为 W 弱符号
// （getPoint W 0x0822eea4 / IsSaveCleanPadPoint W 0x0822eede / setPoint W 0x082870de /
//   incPoint W 0x084eb53e / resetIncRate W 0x08694fe0 / decIncRate W 0x08694ff0 /
//   getPointPerHackType W 0x08695025），且 ORIG 本 TU 区间 0x0831e85c..0x08320210
// 内**不存在**这些符号 ⇒ 原码为头内 inline 定义，由各消费 TU 发射。
// 故定义已迁回 CMCAPManager.h（头内 inline），本 TU 不再持有；原 .cpp 强定义
// 会使最终符号绑定为 T（与 ORIG W 不符）。成员体逐条照 ORIG 反汇编，语义零变化；
// sizeof/成员偏移/mangled/虚槽零变化。
// ============================================================================

}  // namespace WongWork
