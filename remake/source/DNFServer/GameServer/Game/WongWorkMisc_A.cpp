// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkMisc_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "WongWorkMisc_A/ApplyElixerItem.inc"


#include "WongWorkMisc_A/InitializeStat.inc"


#include "WongWorkMisc_A/CalcStat.inc"


// —— 成长系转写辅助：ORIG 形如 ((short)(lv<<k)+t)*mul+cst 的有符号 16 位中间量
// 在等级/成长数据范围内不可能溢出 s16，先按 int 计算再截断存入 u16 字段，
// 与 mov %dx 存储语义一致（各级 _Calc* 报告反编译 C 同构）。——

#include "WongWorkMisc_A/_CalcSwordman.inc"


#include "WongWorkMisc_A/_CalcFighter.inc"


#include "WongWorkMisc_A/_CalcMage.inc"


#include "WongWorkMisc_A/_CalcGunner.inc"


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
