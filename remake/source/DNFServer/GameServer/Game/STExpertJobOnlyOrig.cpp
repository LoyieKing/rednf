// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STExpertJobOnlyOrig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// STExpertJobOnlyOrig.cpp — Wave5 only_orig 批：STExpertJobScript 域（独占 TU）
//
// worklist：/tmp/only_orig/STExpertJobScript.txt（48 符号）。
// 全部符号经 df_game_r elf32-i386 nm -S 核实为 W；布局与控制流由 objdump
// 逐指令转写（各函数 ORIG 地址与 range 见注释）。C1/C2（D1/D2）双别名由
// GNU 编译器对无虚基类的 out-of-line 生命周期函数自动同体输出。
//
// 权威头：STExpertJobScript.h（本批次将 bind/doll/extraction POD 族嵌套化，
// 全局 typedef 兼容既有调用方；偏移证据见头注释）。
// ============================================================================

#include "STExpertJobScript.h"

// 复刻 ORIG「只清 word 槽」的半字段写法（-O0 无函数调用，直写 unsigned short）。
#define ZERO_WORD(p) (*reinterpret_cast<unsigned short*>(p) = 0)


// ----------------------------------------------------------------------------
// stSelfDisjoinMaterialInfo — C1/C2 W 0x0896dc0c range [...dc20]：体仅尾调 clear
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stSelfDisjoinMaterialInfo.inc"
}

// ----------------------------------------------------------------------------
// stMonsterCardBindRate::stMonsterCardBindRate — ORIG W 0x0822b1de
// range [0x0822b1de,0x0822b1f4)：movw $0,+0；movw $0,+2
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stMonsterCardBindRate.inc"
}

// stMonsterCardBindResult — C1/C2 W 0x0896dc20 range [...dc37]
//   movl $0,(%eax)；movw $0,0x4(%eax)   // 只清 +0 dword 与 +4 word
// D1/D2 W 0x08974fac size 5：push ebp; mov ebp,esp; pop ebp; ret（空体）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stMonsterCardBindResult.inc"
}


// ----------------------------------------------------------------------------
// stDollControllerResult — C1/C2 W 0x0896deac range [...dee5]
//   w+8=0 → w+6=w+8 → w+4=w+6 → d+0=(uint)(word)(w+4)   // 链式置零
// D1/D2 W 0x089751fa size 5（空体）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stDollControllerResult.inc"
}


// ----------------------------------------------------------------------------
// stSelfDisjoinMaterialInfo::clear — ORIG W 0x0896dbe6 range [...dc0c]
//   +8=0；+4=+8；+0=+4（链式传递，全零效果）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/clear.inc"
}

// ----------------------------------------------------------------------------
// stMonsterCardBindInfo — C1/C2 W 0x0896dc80 range [...dd3d]
//   构造 3×vector<stMonsterCardBindResult> @+0x10（编译器成员默认构造等价），
//   尾调 clear()。
// clear W 0x0896dc38 range [...dc80]：i=0..3 { cnt[i]=0；vec[i].clear() }
// D1/D2 W 0x0896e218 range [...e250]：逆序析构 3 vector（编译器成员析构等价）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stMonsterCardBindInfo.inc"
}

#include "STExpertJobOnlyOrig/dtor_stMonsterCardBindInfo.inc"
}

#include "STExpertJobOnlyOrig/clear_1.inc"
}

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
}

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
}

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
}

// ============================================================================
// 查询方法
// ============================================================================

// ----------------------------------------------------------------------------
// GetCommonBindInfo — ORIG W 0x0822b366 range [...b374]：return this+0x84
// GetSpecialBindInfo — ORIG W 0x0822b358 range [...b366]：return this+0xc4
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetCommonBindInfo.inc"
}

#include "STExpertJobOnlyOrig/GetSpecialBindInfo.inc"
}

// ----------------------------------------------------------------------------
// GetUpgradeCost — ORIG W 0x085d3d3a range [...d7dd]
//   n = m_upgradeCost.size()；if (n <= index) return 0；
//   else return m_upgradeCost.at(index)
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetUpgradeCost.inc"
}

// ----------------------------------------------------------------------------
// GetMonsterCardBindRate — ORIG W 0x0822b2d0 range [...b358]
//   it = m_monsterCardBindRateMap.find(key)；miss → out=零值、copy dword、
//   return 0；hit → copy dword(it->second)、return 1（dword 一次拷贝 = 两 word）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetMonsterCardBindRate.inc"
}

// ----------------------------------------------------------------------------
// _GetEnduranceRepairCost — ORIG W 0x085d3c26 range [...c74]
//   if (size() <= grade-1) return NULL（无符比较；grade=0 时恒真）
//   else return &m_enduranceRepairCost.at(grade-1)
// ----------------------------------------------------------------------------
stEnduranceRepairCost*
#include "STExpertJobOnlyOrig/_GetEnduranceRepairCost.inc"
}

// ----------------------------------------------------------------------------
// GetDisjointMachineMaxGrade — ORIG W 0x085d3c0e range [...c26]：size()
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetDisjointMachineMaxGrade.inc"
}

// ----------------------------------------------------------------------------
// GetDisjointMachineRepairCost — ORIG W 0x085d3c74 range [...cc8]
//   c = _GetEnduranceRepairCost(grade)；if (!c) return 0；
//   return c->m_field0 - (c->m_field0 * count) / c->m_field4
//   （乘除均为 32 位符号运算：imul/sar/idiv 截断口径）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetDisjointMachineRepairCost.inc"
}

// ----------------------------------------------------------------------------
// GetDisjointMachineRepair — ORIG W 0x085d3cc8 range [...d3a]
//   c = _GetEnduranceRepairCost(grade)；if (!c) return 0；
//   div = c->m_field0 / c->m_field4；if (div <= 0) return 0；
//   outCost = endurance - endurance % div；return endurance / div
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetDisjointMachineRepair.inc"
}

// 同构四联（find 命中返 second、未中返 -1；ORIG 为逐调用完整 find/end 展开，
// 不抽 helper 以保持函数体形态）：
#include "STExpertJobOnlyOrig/GetUnsealScrollPossibleLevel.inc"
}

#include "STExpertJobOnlyOrig/GetAlchemistExtractionPossibleLevel.inc"
}

#include "STExpertJobOnlyOrig/GetEnchanterExtractionPossibleLevel.inc"
}

#include "STExpertJobOnlyOrig/GetDollControllerExtractionPossibleLevel.inc"
}

// ResultInfo getter 三连（纯地址返回）
STExpertJobScript::stAlchemistExtractionResultInfo*
#include "STExpertJobOnlyOrig/GetAlchemistExtractionResultInfo.inc"
}

STExpertJobScript::stEnchanterExtractionResultInfo*
#include "STExpertJobOnlyOrig/GetEnchanterExtractionResultInfo.inc"
}

STExpertJobScript::stDollControllerResultInfo*
#include "STExpertJobOnlyOrig/GetDollControllerExtractionResultInfo.inc"
}
