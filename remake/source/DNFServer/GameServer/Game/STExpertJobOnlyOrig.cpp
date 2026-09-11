// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STExpertJobOnlyOrig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "STExpertJobScript.h"

// 复刻 ORIG「只清 word 槽」的半字段写法（-O0 无函数调用，直写 unsigned short）。
#define ZERO_WORD(p) (*reinterpret_cast<unsigned short*>(p) = 0)


#include "STExpertJobOnlyOrig/stSelfDisjoinMaterialInfo.inc"


#include "STExpertJobOnlyOrig/stMonsterCardBindRate.inc"


#include "STExpertJobOnlyOrig/stMonsterCardBindResult.inc"



#include "STExpertJobOnlyOrig/stDollControllerResult.inc"



#include "STExpertJobOnlyOrig/clear.inc"


#include "STExpertJobOnlyOrig/stMonsterCardBindInfo.inc"


#include "STExpertJobOnlyOrig/dtor_stMonsterCardBindInfo.inc"


#include "STExpertJobOnlyOrig/clear_1.inc"


// ----------------------------------------------------------------------------
// stAlchemistExtractionResultInfo — C1/C2 W 0x0896dd86 range [...de44]
//   构造 3×vector @+0xc 后尾跳 clear；
// clear W 0x0896dd3e range [...dd86]：i=0..2 { cnt[i]=0；vec[i].clear() }
// D1/D2 W 0x0896e250 range [...e288]
// ----------------------------------------------------------------------------
STExpertJobScript::stAlchemistExtractionResultInfo::
    stAlchemistExtractionResultInfo()
{
    clear();
}

STExpertJobScript::stAlchemistExtractionResultInfo::
    ~stAlchemistExtractionResultInfo()
{
}

#include "STExpertJobOnlyOrig/clear_2.inc"


// ----------------------------------------------------------------------------
// stEnchanterExtractionResultInfo — C1/C2 W 0x0896de64 range [...deac]
//   构造 vector@+4 后调 clear；clear W 0x0896de44 range [...de64]：
//   +0=0；vec.clear()
// D1/D2 W 0x0896e288 range [...e29e]
// ----------------------------------------------------------------------------
STExpertJobScript::stEnchanterExtractionResultInfo::
    stEnchanterExtractionResultInfo()
{
    clear();
}

STExpertJobScript::stEnchanterExtractionResultInfo::
    ~stEnchanterExtractionResultInfo()
{
}

#include "STExpertJobOnlyOrig/clear_3.inc"


// ----------------------------------------------------------------------------
// stDollControllerResultInfo — C1/C2 W 0x0896df2e range [...dfec]
//   构造 3×vector<stDollControllerResult> @+0xc 后尾跳 clear；
// clear W 0x0896dee6 range [...df2e]：i=0..2 { cnt[i]=0；vec[i].clear() }
// D1/D2 W 0x0896e29e range [...e2d6]
// ----------------------------------------------------------------------------
STExpertJobScript::stDollControllerResultInfo::
    stDollControllerResultInfo()
{
    clear();
}

STExpertJobScript::stDollControllerResultInfo::
    ~stDollControllerResultInfo()
{
}

#include "STExpertJobOnlyOrig/clear_4.inc"


// ============================================================================
// 查询方法
// ============================================================================

#include "STExpertJobOnlyOrig/GetCommonBindInfo.inc"


#include "STExpertJobOnlyOrig/GetSpecialBindInfo.inc"


#include "STExpertJobOnlyOrig/GetUpgradeCost.inc"


#include "STExpertJobOnlyOrig/GetMonsterCardBindRate.inc"


// ----------------------------------------------------------------------------
// _GetEnduranceRepairCost — ORIG W 0x085d3c26 range [...c74]
//   if (size() <= grade-1) return NULL（无符比较；grade=0 时恒真）
//   else return &m_enduranceRepairCost.at(grade-1)
// ----------------------------------------------------------------------------
stEnduranceRepairCost*
#include "STExpertJobOnlyOrig/_GetEnduranceRepairCost.inc"


#include "STExpertJobOnlyOrig/GetDisjointMachineMaxGrade.inc"


#include "STExpertJobOnlyOrig/GetDisjointMachineRepairCost.inc"


#include "STExpertJobOnlyOrig/GetDisjointMachineRepair.inc"


#include "STExpertJobOnlyOrig/GetUnsealScrollPossibleLevel.inc"


#include "STExpertJobOnlyOrig/GetAlchemistExtractionPossibleLevel.inc"


#include "STExpertJobOnlyOrig/GetEnchanterExtractionPossibleLevel.inc"


#include "STExpertJobOnlyOrig/GetDollControllerExtractionPossibleLevel.inc"


// ResultInfo getter 三连（纯地址返回）
STExpertJobScript::stAlchemistExtractionResultInfo*
#include "STExpertJobOnlyOrig/GetAlchemistExtractionResultInfo.inc"


STExpertJobScript::stEnchanterExtractionResultInfo*
#include "STExpertJobOnlyOrig/GetEnchanterExtractionResultInfo.inc"


STExpertJobScript::stDollControllerResultInfo*
#include "STExpertJobOnlyOrig/GetDollControllerExtractionResultInfo.inc"

