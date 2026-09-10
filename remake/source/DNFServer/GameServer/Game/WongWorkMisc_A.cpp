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
void CCalcCharacStat::ApplyElixerItem(_Additioal_info* out, int statIdx, int value)
{
    // [b27 封顶] 残差仅 case9 循环体寄存器角色（ORIG arr值→ebx/i基址重读→ecx，
    // 4.4 值→ecx/i→ebx+lea 直算——N7 族角色互换）+ jump table 基址 reloc。
    // switch/各 case/store 序全等。
    switch (statIdx)
    {
    case 4:
        out->m_field08 = static_cast<unsigned short>(
            out->m_field08 + value * 0x32);
        break;
    case 6:
        out->m_field0c = static_cast<unsigned short>(
            out->m_field0c + value * 0x32);
        break;
    case 5:
        out->m_field0a = static_cast<unsigned short>(
            out->m_field0a + value * 0x32);
        break;
    case 7:
        out->m_field0e = static_cast<unsigned short>(
            out->m_field0e + value * 0x32);
        break;
    case 2:
        out->m_field00 += value * 250;                       // imul $0xfa
        break;
    case 3:
        out->m_field04 += value * 250;
        break;
    case 8:
        out->m_field42 += value * 10;
        break;
    case 9:
        // [2026-09-10 D06 · Main 新轴（const/符号性）实测不适用] ORIG 0x0831e995 段
        // `movzwl …,%eax; mov %eax,%ebx`（元素值升格 callee-saved %ebx、i 留 %ecx +
        // `add $0x8,%ecx`）；去 cast 形与复合赋值形实测与 cast 形**同值**（119 vs 121，
        // 差异恒为寄存器角色互换）⇒ 循环变量符号性（两侧同为有符号 setle）与 cv 限定
        // 符均非本件变量，维持 N7/N78 封顶。
        for (int i = 0; i < 4; ++i)                          // 循环上界 cmp $0x3,setle
            out->m_field10[i] = static_cast<unsigned short>(
                out->m_field10[i] + value * 10);
        break;
    default:
        break;
    }
}

// ORIG T 0x0831e9ca _ZN8WongWork15CCalcCharacStat14InitializeStatEP15_Additioal_infoi
// 报告：docs/class_func_reports/WongWork__CCalcCharacStat/InitializeStat.md
// 清零后按职业写入初始属性表（job 只认 0..3，其余静默返回）。
void CCalcCharacStat::InitializeStat(_Additioal_info* out, int job)
{
    out->clear();
    switch (job)
    {
    case 0:                                   // +0x042 分支
        out->m_field00 = 0x5dc;
        out->m_field04 = 0x5dc;
        out->m_field08 = 0x32;
        out->m_field0a = 0x32;
        out->m_field0c = 0x3c;
        out->m_field0e = 0x32;
        out->m_field10[2] = 200;
        out->m_field10[3] = 0xff38;
        out->m_field3a = 400000;
        out->m_field40 = 0x96;
        out->m_field42 = 0x2134;
        out->m_field46 = 0x2134;
        out->m_field48 = 7000;
        out->m_field4a = 6000;
        out->m_field4c = 0x10cc;
        out->m_field4e = 680000;
        break;
    case 1:                                   // +0x0db 分支
        out->m_field00 = 0x708;
        out->m_field04 = 0x5dc;
        out->m_field08 = 0x3c;
        out->m_field0a = 0x3c;
        out->m_field0c = 0x32;
        out->m_field0e = 0x32;
        out->m_field10[2] = 0xff38;
        out->m_field10[3] = 200;
        out->m_field3a = 300000;
        out->m_field40 = 0x82;
        out->m_field42 = 0x238c;
        out->m_field46 = 0x251c;
        out->m_field48 = 10000;
        out->m_field4a = 6000;
        out->m_field4c = 0x125c;
        out->m_field4e = 500000;
        break;
    case 2:                                   // +0x174 分支
        out->m_field00 = 0x4e2;
        out->m_field04 = 0x708;
        out->m_field08 = 0x28;
        out->m_field0a = 0x28;
        out->m_field0c = 0x46;
        out->m_field0e = 0x3c;
        out->m_field3a = 330000;
        out->m_field40 = 0xaf;
        out->m_field42 = 0x2008;
        out->m_field46 = 0x251c;
        out->m_field48 = 8000;
        out->m_field4a = 6000;
        out->m_field4c = 0x1324;
        out->m_field4e = 600000;
        break;
    case 3:                                   // +0x1fb 分支
        out->m_field00 = 1000;
        out->m_field04 = 2000;
        out->m_field08 = 0x1e;
        out->m_field0a = 0x1e;
        out->m_field0c = 0x50;
        out->m_field0e = 0x46;
        out->m_field3a = 280000;
        out->m_field40 = 200;
        out->m_field42 = 8000;
        out->m_field46 = 10000;
        out->m_field48 = 10000;
        out->m_field4a = 5000;
        out->m_field4c = 0xdac;
        out->m_field4e = 400000;
        break;
    default:
        break;                                // 其余值：仅 clear 后返回
    }
}

// ORIG T 0x0831ec4a _ZN8WongWork15CCalcCharacStat8CalcStatEP15_Additioal_infoiiiii
// 报告：CalcStat.md。清零 → 累加角色表基行（&characTable[job]，即 m_baseInfo[0]）
// → 取三个成长块指针 (0,0)/(firstGrow,0)/(firstGrow,secondGrow) → 按 level
// 逐级累加：<15 用一转前系数块，<51 用一转块，否则二转块。
_Additioal_info* CCalcCharacStat::CalcStat(_Additioal_info* out, int job,
                                           int firstGrow, int secondGrow,
                                           int level)
{
    out->clear();
    // ORIG 四次独立 call G_CDataManager + mov +0x14 + imul $0x7dc 重算行地址
    // （CalcStat.md +0x011/+0x034/+0x063/+0x090），故逐处表达式访问、不缓存行引用。
    out->add(G_CDataManager()->m_characList->m_characs[job].m_baseInfo0);

    const _Additioal_info* pFirst = G_CDataManager()->m_characList
                                        ->m_characs[job]
                                        .get_growtype_stat(0, 0);
    const _Additioal_info* pSecond = G_CDataManager()->m_characList
                                         ->m_characs[job]
                                         .get_growtype_stat(firstGrow, 0);
    const _Additioal_info* pThird = G_CDataManager()->m_characList
                                        ->m_characs[job]
                                        .get_growtype_stat(firstGrow, secondGrow);
    for (int lv = 1; lv < level; ++lv)
    {
        if (lv <= 0xe)                     // cmpl $0xe,jg→else-if（即 lv<15）
            out->add(*pFirst);
        else if (lv <= 0x31)               // cmpl $0x31（即 lv<50）
            out->add(*pSecond);
        else
            out->add(*pThird);
    }
    return out;
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
_Additioal_info* CCalcCharacStat::_CalcSwordman(_Additioal_info* out,
                                                int secondType, int level)
{
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 0x20d + 0x5dcu;  // (sVar1-1)*0x1e 写回 u16
        out->m_field04 = (level - 1) * 0xaf + 0x5dcu;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x1e + 0x32);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x1e + 0x32);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x23 + 0x3c);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x19 + 0x32);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_hitRecovery = static_cast<short>((level - 1) * 0x14 + 6000);
        break;
    case 1:
        out->m_field00 = level * 400 + 0xc99u;
        out->m_field04 = level * 300 - 0x39du;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0x46);
        out->m_field0a = static_cast<unsigned short>(level * 32 - level * 2 + 0x14);
        out->m_field0c = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field0e = static_cast<unsigned short>((level + 1) * 0x19);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x2080);
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    case 2:
        out->m_field00 = level * 0x15e + 0x101du;
        out->m_field04 = level * 0x15e - 0x721u;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 4 + 200);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 4 + 200);
        out->m_field0c = static_cast<unsigned short>((level * 4 + level) * 8 - 0x41);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 8 - 0xf5);
        // [2026-09-10 D06] P321-T1：ORIG 0x0831f? `shl $0x3,%eax; mov %eax,%edx`
        // ⇒ +0x14 源视角为 **signed short**（探针 /tmp/d06/p14.cpp g3：signed
        // short 字段产该形，unsigned short 恒产 `lea 0x0(,%eax,8),%edx`）。
        out->m_field10s[2] = static_cast<short>((level + 7) << 3);  // +0x14
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 - 0xd2);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x19f0);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x1810);
        break;
    case 3:
        out->m_field00 = level * 0x226 + 0x20du;
        out->m_field04 = level * 0x96 + 0x6efu;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 8 - 0xa0);
        out->m_field0a = static_cast<unsigned short>(level * 0x2d - 0xfa);
        out->m_field0c = static_cast<unsigned short>(level * 16 - level + 0x181);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 4 + 0x73);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_field42 = level * 10 + 0x2080u;
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 8 + 0x15f4);
        break;
    case 4:
        out->m_field00 = level * 0x20d + 0x3cfu;
        out->m_field04 = level * 0xaf + 0x52du;
        out->m_field08 = static_cast<unsigned short>(level * 32 - level * 2 + 0x14);
        out->m_field0a = static_cast<unsigned short>(level * 32 - level * 2 + 0x14);
        out->m_field0c = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field0e = static_cast<unsigned short>(level * 32 - level * 2 - 0x41);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_mpRegen = static_cast<short>(level * 4 + level + 0x3c);
        out->m_field48 = static_cast<unsigned short>(level * 10 + 0x1aa4);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 4 + 0x1608);
        break;
    default:
        break;
    }
    return out;
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
_Additioal_info* CCalcCharacStat::_CalcFighter(_Additioal_info* out,
                                               int secondType, int level)
{
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 0x1c2 + 0x708u;
        out->m_field04 = (level - 1) * 0xfa + 0x5dcu;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x20 + 0x3c);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x23 + 0x3c);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x1e + 0x32);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x17 + 0x32);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_hitRecovery = static_cast<short>((level - 1) * 0x14 + 6000);
        break;
    case 1:
        out->m_field00 = level * 400 + 0x8cau;
        out->m_field04 = level * 300 + 0x15eu;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 4 + 0xf4);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 4 + 0x127);
        out->m_field0c = static_cast<unsigned short>((level * 4 + level) * 8 - 0xa0);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 8 - 0x117);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 - 0xe6);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x25a8);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x1810);
        break;
    case 2:
        out->m_field00 = level * 500 + 0x1c2u;
        out->m_field04 = level * 200 + 0x866u;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0x1a);
        out->m_field0a = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 - 0x46);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 4 + 0x51);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x2468);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 8 + 0x15f4);
        break;
    case 3:
        out->m_field00 = level * 600 - 0x546u;
        out->m_field04 = level * 100 + 0xf6eu;
        out->m_field08 = static_cast<unsigned short>(level * 0x26 - 0x50);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 8 - 0x41);
        out->m_field0c = static_cast<unsigned short>(level * 0x1b + 0x4a);
        out->m_field0e = static_cast<unsigned short>(level * 16 - level + 0xab);
        // [2026-09-10 D06] 同上：ORIG `sub $0x134,%ax; mov %eax,%edx` ⇒ signed 视角
        // （探针 p14 g1/g5 命中；g0 unsigned 恒 lea）。
        out->m_field10s[2] = static_cast<short>(level * 6 - 0x134);  // +0x14
        out->m_field3a = level * 3000 + 0x466f4;
        out->m_field42 = level * 10 + 0x22d8u;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    case 4:
        out->m_field00 = level * 0x1c2 + 0x546u;
        out->m_field04 = level * 0xfa + 0x4e2u;
        out->m_field08 = static_cast<unsigned short>((level - 0x12) * 0x20 + 0x25c);
        out->m_field0a = static_cast<unsigned short>(level * 0x23 + 0x19);
        out->m_field0c = static_cast<unsigned short>(level * 32 - level * 2 + 0x14);
        out->m_field0e = static_cast<unsigned short>(level * 0x17 + 0x1b);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    default:
        break;
    }
    return out;
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
_Additioal_info* CCalcCharacStat::_CalcMage(_Additioal_info* out,
                                            int secondType, int level)
{
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 0x145 + 1000u;
        out->m_field04 = (level - 1) * 0x177 + 2000u;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x14 + 0x1e);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x14 + 0x1e);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x28 + 0x50);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x28 + 0x46);
        out->m_field3a = (level - 1) * 0x9c4 + 280000;
        // [2026-09-10 D06] P321-T1/P196 类型直证：ORIG 0x0831fd1f `add $0xc8,%ax`
        // / 0x0831fd3d `add $0x1388,%ax`（16 位窄化 add）⇒ 源类型为 **signed
        // short**（探针 /tmp/d06/p16.cpp f0/f14/f16：unsigned short 恒产
        // `lea 0xc8(%eax),%edx`，signed short 才产 `add $imm,%ax; mov %eax,%edx`）。
        // 用 m_mpRegen/m_hitRecovery（CCharacter.h 同址 signed 视角，+0x40/+0x4a）。
        out->m_mpRegen = static_cast<short>((level - 1) * 10 + 200);
        out->m_hitRecovery = static_cast<short>((level - 1) * 10 + 5000);
        break;
    case 1:
        out->m_field00 = level * 0x113 + 0x627u;
        out->m_field04 = level * 0x1a9 + 0x2d5u;
        out->m_field08 = static_cast<unsigned short>(level * 16 - level + 100);
        out->m_field0a = static_cast<unsigned short>(level * 16 - level + 100);
        out->m_field0c = static_cast<unsigned short>(level * 0x2d - 0x32);
        out->m_field0e = static_cast<unsigned short>(level * 0x2d - 0x3c);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 + 10);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x25a8);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x137e);
        break;
    case 2:
        out->m_field00 = level * 300 + 0x465u;
        out->m_field04 = level * 400 + 0x497u;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 4 + 10);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 4 + 10);
        out->m_field0c = static_cast<unsigned short>((level + 1) * 0x28);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 8 + 0x1e);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 + 10);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x25a8);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x137e);
        break;
    case 3:
        out->m_field00 = level * 400 - 0x2a3u;
        out->m_field04 = level * 300 + 0xb9fu;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0x104);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x50);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 + 0x82);
        out->m_field0e = static_cast<unsigned short>(level * 0x19 + 300);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x265c);
        out->m_field42 = level * 10 + 0x265cu;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x1216);
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 2 + 0xbe);
        break;
    case 4:
        out->m_field00 = level * 0x15e + 0xe1u;
        out->m_field04 = level * 0x15e + 0x81bu;
        out->m_field08 = static_cast<unsigned short>(level * 0x19 - 0x50);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x50);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 + 0x82);
        out->m_field0e = static_cast<unsigned short>(level * 0x23 + 0x78);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 + 10);
        out->m_field48 = static_cast<unsigned short>(level * 10 + 0x265c);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 4 + 0x12ca);
        break;
    default:
        break;
    }
    return out;
}

// ORIG T 0x0831f75c _ZN8WongWork15CCalcCharacStat11_CalcGunnerEP15_Additioal_infoii
// 报告：_CalcGunner.md。
// [b28d-54 N] 同 _CalcMage（短整扩展/lea 折叠，N1/N97）；语义等价。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：同 _CalcSwordman 修法，段级
//   identical 15→33/44；残余 6 段 = 16 位字段 add/sub vs lea（N1/N86b+跨版本窄化）
//   ⇒ 封顶。p167: 0 missing / 0 extra；语义与 Ghidra C 逐字段一致。
// [2026-09-10 D06 · DIFF→IDENTICAL 402=402] 同 _CalcSwordman 修法（signed 视角
//   m_hitRecovery/m_mpRegen），旧"16 位字段 add/sub vs lea ⇒ 封顶"判词作废。
_Additioal_info* CCalcCharacStat::_CalcGunner(_Additioal_info* out,
                                              int secondType, int level)
{
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 400 + 0x4e2u;
        out->m_field04 = (level - 1) * 300 + 0x708u;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x19 + 0x28);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x17 + 0x28);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x25 + 0x46);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x23 + 0x3c);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_hitRecovery = static_cast<short>((level - 1) * 0x14 + 6000);
        break;
    case 1:
        out->m_field00 = level * 0x15e + 0x6d6u;
        out->m_field04 = level * 0x15e + 600u;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0xa5);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x13);
        out->m_field0c = static_cast<unsigned short>(level * 32 - level * 2 + 0x9f);
        out->m_field0e = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x2468);
        out->m_field42 = level * 10 + 0x1f54u;
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 8 + 0x15f4);
        break;
    case 2:
        out->m_field00 = level * 0x1a9 + 400u;
        out->m_field04 = level * 0x113 + 0x79eu;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 8 - 0xff);
        out->m_field0a = static_cast<unsigned short>(level * 0x2d - 0x17b);
        out->m_field0c = static_cast<unsigned short>(level * 16 - level + 0x1ad);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 4 + 0x127);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    case 3:
        out->m_field00 = level * 300 + 0xa5au;
        out->m_field04 = level * 400 - 300u;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x69);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 4 + 0x47);
        out->m_field0c = static_cast<unsigned short>((level * 4 + level) * 8 - 0x15);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 8 - 0x41);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 - 0xb9);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x1dd8);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x1810);
        break;
    case 4:
        out->m_field00 = level * 400 + 0x352u;
        out->m_field04 = level * 300 + 0x5dcu;
        out->m_field08 = static_cast<unsigned short>(level * 32 - level * 2 - 0x4b);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x13);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 + 0x45);
        out->m_field0e = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    default:
        break;
    }
    return out;
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
