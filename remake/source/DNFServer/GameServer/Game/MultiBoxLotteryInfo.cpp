// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MultiBoxLotteryInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— MultiBoxLotteryInfo（ORIG class，含 C1/D1/clear）。
// 报告缺失：docs/class_func_reports 无 MultiBoxLotteryInfo 目录，以下实现
// 依据 ORIG nm -S --defined-only 定址 + objdump -d 逐函数转写（2026-08-28）。
// 符号/地址/语义：
//   _ZN19MultiBoxLotteryInfoC1Ev            0x8513ed6  ctor：m_units + m_default 构造后 clear()
//   _ZN19MultiBoxLotteryInfoD1Ev            0x85145b2  dtor：m_default + m_units 析构
//   _ZN19MultiBoxLotteryInfo5clearEv        0x8513f44  清 m_units，置 field0/4/8/c=-1/0/-1/0
//   LotteryUnit  C1 0x89c351e / CC 0x85151a8 / D1 0x8515236 / clear 0x89c358c / aSERKS0_ 0x89cdf46
//   stDefaultItem C1 0x89c35d4 / D1 0x89d48b2
//   stDefaultItemInfo C1 0x8513e50 / D1 0x8513ec0
//   LotteryDefaultArad C1Ejjj 0x8513e30 / D1 0x8522962
#include "MultiBoxLotteryInfo.h"

#include "MultiBoxLotteryInfo/LotteryDefaultArad.inc"


#include "MultiBoxLotteryInfo/stDefaultItem.inc"


// stDefaultItem 隐式平凡析构（ORIG _Destroy_aux<true> 实证，见头文件判词）。

#include "MultiBoxLotteryInfo/stDefaultItemInfo.inc"


#include "MultiBoxLotteryInfo/dtor_stDefaultItemInfo.inc"

#include "MultiBoxLotteryInfo/LotteryUnit.inc"


#include "MultiBoxLotteryInfo/LotteryUnit_1.inc"


#include "MultiBoxLotteryInfo/dtor_LotteryUnit.inc"

#include "MultiBoxLotteryInfo/clear.inc"


MultiBoxLotteryInfo::LotteryUnit& MultiBoxLotteryInfo::LotteryUnit::operator=(
    const LotteryUnit& other)
{
    m_count = other.m_count;
    m_defaultIdx = other.m_defaultIdx;
    m_defaultNum = other.m_defaultNum;
    m_defaultFlag = other.m_defaultFlag;
    m_items = other.m_items;
    m_bonus = other.m_bonus;
    return *this;
}

#include "MultiBoxLotteryInfo/MultiBoxLotteryInfo.inc"


#include "MultiBoxLotteryInfo/dtor_MultiBoxLotteryInfo.inc"

#include "MultiBoxLotteryInfo/clear_1.inc"

