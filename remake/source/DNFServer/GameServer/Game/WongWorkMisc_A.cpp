// ============================================================================
// WongWorkMisc_A.cpp —— WongWork 域 A 段 G6 杂项组独占 TU（WongWorkA 批次）。
//
// 承载两类 14 个 ORIG 真实符号（C++98 / 32位 / O0）：
//   WongWork::CCalcCharacStat   7 方法（全 T）
//   WongWork::CMCAPManager      7 方法（getPoint/setPoint/incPoint/decIncRate/
//                               resetIncRate/IsSaveCleanPadPoint/getPointPerHackType，
//                               nm -S 实证均 W 内联段）
// 每函数按 docs/class_func_reports/<类>/<func>.md 的 ## 汇编 + ## 反编译 C
// 逐指令转写；报告缺失者以 objdump ORIG 为准并已标注地址。
// 注意：CMCAPManager.h 中既有的 getPoint() const / isSaveCleanPadPoint() const
// 声明与实现落在 CMCAPManager.cpp，其 mangled 与 ORIG 不符（_ZNK… 且字段错位）；
// 本 TU 提供唯一 ORIG 匹配强符号，错误符号无调用方、留待宿主清理（见批次报告）。
// ============================================================================

#include <algorithm>

#include "CCharacter.h"        // _Additioal_info（全局 ns，0x52 pack(1)）/ CCharacter
#include "CMCAPManager.h"      // WongWork::CMCAPManager
#include "CDataManager.h"      // G_CDataManager() / m_characList（+0x14）
#include "CCharacList.h"       // CCharacList::m_characs[11]（+0x000，步长 0x7dc）

namespace WongWork
{

// ============================================================================
// CCalcCharacStat —— 角色基础属性计算器
// ============================================================================

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

// ORIG T 0x0831e85c _ZN8WongWork15CCalcCharacStat15ApplyElixerItemEP15_Additioal_infoii
// 报告：ApplyElixerItem.md。灵药道具按 statIdx 选择属性槽做加法（u16 回绕）；
// statIdx>9 静默忽略；case9 对 m_field10[0..3] 各加 value*10。
// ORIG 汇编不物化 (short)value：逐 case 以 32 位 value 乘 50 后 movw 截断
// （mod 2^16 与 s16 乘加等价）；case 物理布局序 = 4,6,5,7,2,3,8,9
// （ORIG 块序 +0x1f/+0x4c/+0x79/+0xa6/+0xd3/+0xea/+0x103/+0x120）。
// [b28d-54 N 复核] ORIG 0x0831e85c：case9 循环体寄存器角色互换（N7）+ jump table
//   基址 reloc（AE 归一）；switch/各 case/store 序全等，语义等价。
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
        for (int i = 0; i < 4; ++i)                          // 循环上界 cmp $0x3,setle
            out->m_field10[i] = static_cast<unsigned short>(
                out->m_field10[i] + value * 10);
        break;
    default:
        break;
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
_Additioal_info* CCalcCharacStat::_CalcSwordman(_Additioal_info* out,
                                                int secondType, int level)
{
    const short t = static_cast<short>(level);          // sVar1=(short)param_3
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 0x20d + 0x5dc;   // (sVar1-1)*0x1e 写回 u16
        out->m_field04 = (level - 1) * 0xaf + 0x5dc;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x1e + 0x32);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x1e + 0x32);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x23 + 0x3c);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x19 + 0x32);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_field4a = static_cast<unsigned short>((level - 1) * 0x14 + 6000);
        break;
    case 1:
        out->m_field00 = level * 400 + 0xc99;
        out->m_field04 = level * 300 - 0x39d;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0x46);
        out->m_field0a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x14);
        out->m_field0c = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x73);
        out->m_field0e = static_cast<unsigned short>((t + 1) * 0x19);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x2080);
        out->m_field4a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x16a8);
        break;
    case 2:
        out->m_field00 = level * 0x15e + 0x101d;
        out->m_field04 = level * 0x15e - 0x721;
        out->m_field08 = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 200;
        out->m_field0a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 200;
        out->m_field0c = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 - 0x41);
        out->m_field0e = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 - 0xf5);
        out->m_field10[2] = static_cast<unsigned short>((t + 7) * 8);   // +0x14
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_field40 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 - 0xd2);
        out->m_field48 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x19f0);
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 2 + 0x1810);
        break;
    case 3:
        out->m_field00 = level * 0x226 + 0x20d;
        out->m_field04 = level * 0x96 + 0x6ef;
        out->m_field08 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 - 0xa0);
        out->m_field0a = static_cast<unsigned short>(t * 0x2d - 0xfa);
        out->m_field0c = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 4)) - t + 0x181);
        out->m_field0e = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x73);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_field42 = level * 10 + 0x2080;
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 + 0x15f4);
        break;
    case 4:
        out->m_field00 = level * 0x20d + 0x3cf;
        out->m_field04 = level * 0xaf + 0x52d;
        out->m_field08 = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x14);
        out->m_field0a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x14);
        out->m_field0c = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x73);
        out->m_field0e = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 - 0x41);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_field40 = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 2)) + t + 0x3c);
        out->m_field48 = static_cast<unsigned short>(level * 10 + 0x1aa4);
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x1608);
        break;
    default:
        break;
    }
    return out;
}

// ORIG T 0x0831f282 _ZN8WongWork15CCalcCharacStat12_CalcFighterEP15_Additioal_infoii
// 报告：_CalcFighter.md。
// [b28d-54 N] 同 _CalcSwordman（短整扩展/lea 折叠，N1/N97）；语义等价。
_Additioal_info* CCalcCharacStat::_CalcFighter(_Additioal_info* out,
                                               int secondType, int level)
{
    const short t = static_cast<short>(level);
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 0x1c2 + 0x708;
        out->m_field04 = (level - 1) * 0xfa + 0x5dc;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x20 + 0x3c);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x23 + 0x3c);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x1e + 0x32);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x17 + 0x32);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_field4a = static_cast<unsigned short>((level - 1) * 0x14 + 6000);
        break;
    case 1:
        out->m_field00 = level * 400 + 0x8ca;
        out->m_field04 = level * 300 + 0x15e;
        out->m_field08 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0xf4);
        out->m_field0a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x127);
        out->m_field0c = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 - 0xa0);
        out->m_field0e = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 - 0x117);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_field40 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 - 0xe6);
        out->m_field48 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x25a8);
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 2 + 0x1810);
        break;
    case 2:
        out->m_field00 = level * 500 + 0x1c2;
        out->m_field04 = level * 200 + 0x866;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0x1a);
        out->m_field0a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x73);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 - 0x46);
        out->m_field0e = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x51);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x2468);
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 + 0x15f4);
        break;
    case 3:
        out->m_field00 = level * 600 - 0x546;
        out->m_field04 = level * 100 + 0xf6e;
        out->m_field08 = static_cast<unsigned short>(level * 0x26 - 0x50);
        out->m_field0a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 - 0x41);
        out->m_field0c = static_cast<unsigned short>(level * 0x1b + 0x4a);
        out->m_field0e = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 4)) - t + 0xab);
        out->m_field10[2] = static_cast<unsigned short>(t * 6 - 0x134);  // +0x14
        out->m_field3a = level * 3000 + 0x466f4;
        out->m_field42 = level * 10 + 0x22d8;
        out->m_field4a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x16a8);
        break;
    case 4:
        out->m_field00 = level * 0x1c2 + 0x546;
        out->m_field04 = level * 0xfa + 0x4e2;
        out->m_field08 = static_cast<unsigned short>((level - 0x12) * 0x20 + 0x25c);
        out->m_field0a = static_cast<unsigned short>(level * 0x23 + 0x19);
        out->m_field0c = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x14);
        out->m_field0e = static_cast<unsigned short>(level * 0x17 + 0x1b);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_field4a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x16a8);
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
_Additioal_info* CCalcCharacStat::_CalcMage(_Additioal_info* out,
                                            int secondType, int level)
{
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 0x145 + 1000;
        out->m_field04 = (level - 1) * 0x177 + 2000;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x14 + 0x1e);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x14 + 0x1e);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x28 + 0x50);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x28 + 0x46);
        out->m_field3a = (level - 1) * 0x9c4 + 280000;
        out->m_field40 = static_cast<unsigned short>((level - 1) * 10 + 200);
        out->m_field4a = static_cast<unsigned short>((level - 1) * 10 + 5000);
        break;
    case 1:
        out->m_field00 = level * 0x113 + 0x627;
        out->m_field04 = level * 0x1a9 + 0x2d5;
        out->m_field08 = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 4)) - static_cast<short>(level) + 100);
        out->m_field0a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 4)) - static_cast<short>(level) + 100);
        out->m_field0c = static_cast<unsigned short>(level * 0x2d - 0x32);
        out->m_field0e = static_cast<unsigned short>(level * 0x2d - 0x3c);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_field40 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 4 + 10);
        out->m_field48 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 4 + 0x25a8);
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 2 + 0x137e);
        break;
    case 2:
        out->m_field00 = level * 300 + 0x465;
        out->m_field04 = level * 400 + 0x497;
        out->m_field08 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 4 + 10);
        out->m_field0a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 4 + 10);
        out->m_field0c = static_cast<unsigned short>(
            (static_cast<short>(level) + 1) * 0x28);
        out->m_field0e = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 8 + 0x1e);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_field40 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 4 + 10);
        out->m_field48 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 4 + 0x25a8);
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 2 + 0x137e);
        break;
    case 3:
        out->m_field00 = level * 400 - 0x2a3;
        out->m_field04 = level * 300 + 0xb9f;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0x104);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x50);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 + 0x82);
        out->m_field0e = static_cast<unsigned short>(level * 0x19 + 300);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x265c);
        out->m_field42 = level * 10 + 0x265c;
        out->m_field4a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + static_cast<short>(level) * -2 + 0x1216);
        out->m_field40 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 2 + 0xbe);
        break;
    case 4:
        out->m_field00 = level * 0x15e + 0xe1;
        out->m_field04 = level * 0x15e + 0x81b;
        out->m_field08 = static_cast<unsigned short>(level * 0x19 - 0x50);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x50);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 + 0x82);
        out->m_field0e = static_cast<unsigned short>(level * 0x23 + 0x78);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_field40 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 4 + 10);
        out->m_field48 = static_cast<unsigned short>(level * 10 + 0x265c);
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + static_cast<short>(level)) * 4 + 0x12ca);
        break;
    default:
        break;
    }
    return out;
}

// ORIG T 0x0831f75c _ZN8WongWork15CCalcCharacStat11_CalcGunnerEP15_Additioal_infoii
// 报告：_CalcGunner.md。
// [b28d-54 N] 同 _CalcMage（短整扩展/lea 折叠，N1/N97）；语义等价。
_Additioal_info* CCalcCharacStat::_CalcGunner(_Additioal_info* out,
                                              int secondType, int level)
{
    const short t = static_cast<short>(level);
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 400 + 0x4e2;
        out->m_field04 = (level - 1) * 300 + 0x708;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x19 + 0x28);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x17 + 0x28);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x25 + 0x46);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x23 + 0x3c);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_field4a = static_cast<unsigned short>((level - 1) * 0x14 + 6000);
        break;
    case 1:
        out->m_field00 = level * 0x15e + 0x6d6;
        out->m_field04 = level * 0x15e + 600;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0xa5);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x13);
        out->m_field0c = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x9f);
        out->m_field0e = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x73);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x2468);
        out->m_field42 = level * 10 + 0x1f54;
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 + 0x15f4);
        break;
    case 2:
        out->m_field00 = level * 0x1a9 + 400;
        out->m_field04 = level * 0x113 + 0x79e;
        out->m_field08 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 - 0xff);
        out->m_field0a = static_cast<unsigned short>(level * 0x2d - 0x17b);
        out->m_field0c = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 4)) - t + 0x1ad);
        out->m_field0e = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x127);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_field4a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x16a8);
        break;
    case 3:
        out->m_field00 = level * 300 + 0xa5a;
        out->m_field04 = level * 400 - 300;
        out->m_field08 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x69);
        out->m_field0a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x47);
        out->m_field0c = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 - 0x15);
        out->m_field0e = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 8 - 0x41);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_field40 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 - 0xb9);
        out->m_field48 = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 4 + 0x1dd8);
        out->m_field4a = static_cast<unsigned short>(
            (static_cast<int>(static_cast<short>(level << 2)) + t) * 2 + 0x1810);
        break;
    case 4:
        out->m_field00 = level * 400 + 0x352;
        out->m_field04 = level * 300 + 0x5dc;
        out->m_field08 = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 - 0x4b);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x13);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 + 0x45);
        out->m_field0e = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x73);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_field4a = static_cast<unsigned short>(
            static_cast<int>(static_cast<short>(level << 5)) + t * -2 + 0x16a8);
        break;
    default:
        break;
    }
    return out;
}

// ============================================================================
// CMCAPManager —— CleanPad 点数管理。真实布局：+0x00 点数、+0x04 增长率
// （C1 置 0x3e8）、+0x14 m_saveCleanPadPoint；头内既有 pad 区不可改名，
// 在成员函数体内按 m_pad00 直接取址，保持 ORIG 单帧无子调用形态。
// ============================================================================

// ORIG W 0x0822eea4 _ZN8WongWork12CMCAPManager8getPointEv（非 const）
// ORIG（objdump 0x0822eea4）：(%esp)=this、0x4=&-0x10(0x2710) → min(m_point, cap)，
// 再 max(0u, _)；以 this 直作 min 引用实参，无中间值槽。
unsigned int CMCAPManager::getPoint()
{
    return std::max<unsigned int>(
        0u, std::min<unsigned int>(m_point, 0x2710u));
}

// ORIG W 0x082870de _ZN8WongWork12CMCAPManager8setPointEt
// ORIG（objdump 0x082870de）：min(point(&-0x1c), cap(&-0xc)) 的返回引用直接作
// max 第二实参（mov %eax,0x4(%esp)，无 u16 中间量槽），max(0(&-0xa), _) →
// movzwl 零扩展整字写 +0x00（4B），并置 +0x14=1。
void CMCAPManager::setPoint(unsigned short point)
{
    m_point = static_cast<unsigned int>(std::max<unsigned short>(
        0, std::min<unsigned short>(point, 0x2710)));   // 0x2710 落 u16 常量槽
    m_saveCleanPadPoint = true;                          // movb $0x1,+0x14
}

// ORIG W 0x084eb53e _ZN8WongWork12CMCAPManager8incPointEi
// point(+0) += rate(+4)*value/1000（ORIG 魔数 0x10624dd3; sar 6 = 有符号整除 1000）。
void CMCAPManager::incPoint(int value)
{
    m_point = m_point + m_rate * value / 1000;
    m_saveCleanPadPoint = true;
}

// ORIG W 0x08694ff0 _ZN8WongWork12CMCAPManager10decIncRateEi
// rate(+4) = max(0, rate - value)（_ZSt3maxIiERKT_S2_S2_ 实证调用）。
void CMCAPManager::decIncRate(int value)
{
    m_rate = std::max(0, m_rate - value);
}

// ORIG W 0x08694fe0 _ZN8WongWork12CMCAPManager12resetIncRateEv：rate=1000。
void CMCAPManager::resetIncRate()
{
    m_rate = 0x3e8;
}

// ORIG W 0x0822eede _ZN8WongWork12CMCAPManager19IsSaveCleanPadPointEv（非 const）
bool CMCAPManager::IsSaveCleanPadPoint()
{
    return m_saveCleanPadPoint;              // movzbl +0x14
}

// ORIG W 0x08695025 _ZN8WongWork12CMCAPManager19getPointPerHackTypeENS_13ENUM_HACKTYPEE
// 表基址下标 101（sub $0x65）：pointPerHackType_[type-101]。
int CMCAPManager::getPointPerHackType(WongWork::ENUM_HACKTYPE type)
{
    return pointPerHackType_[static_cast<int>(type) - 0x65];
}

}  // namespace WongWork
