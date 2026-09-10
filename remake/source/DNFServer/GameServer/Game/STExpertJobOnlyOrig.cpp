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


// ----------------------------------------------------------------------------
// stSelfDisjoinMaterialInfo — C1/C2 W 0x0896dc0c range [...dc20]：体仅尾调 clear
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stSelfDisjoinMaterialInfo.inc"
{
    clear();
}

// ----------------------------------------------------------------------------
// stMonsterCardBindRate::stMonsterCardBindRate — ORIG W 0x0822b1de
// range [0x0822b1de,0x0822b1f4)：movw $0,+0；movw $0,+2
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stMonsterCardBindRate.inc"
{
    m_field0 = 0;
    m_field2 = 0;
}

// stMonsterCardBindResult — C1/C2 W 0x0896dc20 range [...dc37]
//   movl $0,(%eax)；movw $0,0x4(%eax)   // 只清 +0 dword 与 +4 word
// D1/D2 W 0x08974fac size 5：push ebp; mov ebp,esp; pop ebp; ret（空体）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stMonsterCardBindResult.inc"
{
    ZERO_WORD(&m_field4);
    m_field0 = 0;
}


// ----------------------------------------------------------------------------
// stDollControllerResult — C1/C2 W 0x0896deac range [...dee5]
//   w+8=0 → w+6=w+8 → w+4=w+6 → d+0=(uint)(word)(w+4)   // 链式置零
// D1/D2 W 0x089751fa size 5（空体）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stDollControllerResult.inc"
{
    ZERO_WORD(&m_field8);                       // +8 word = 0
    m_field6 = m_field8;                        // +6 word = w+8
    m_field4 = m_field6;                        // +4 word = w+6
    m_field0 = static_cast<int>(
        static_cast<unsigned short>(m_field4)); // +0 dword = zext(w+4)
}


// ----------------------------------------------------------------------------
// stSelfDisjoinMaterialInfo::clear — ORIG W 0x0896dbe6 range [...dc0c]
//   +8=0；+4=+8；+0=+4（链式传递，全零效果）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/clear.inc"
{
    m_field8 = 0;
    m_field4 = m_field8;
    m_field0 = m_field4;
}

// ----------------------------------------------------------------------------
// stMonsterCardBindInfo — C1/C2 W 0x0896dc80 range [...dd3d]
//   构造 3×vector<stMonsterCardBindResult> @+0x10（编译器成员默认构造等价），
//   尾调 clear()。
// clear W 0x0896dc38 range [...dc80]：i=0..3 { cnt[i]=0；vec[i].clear() }
// D1/D2 W 0x0896e218 range [...e250]：逆序析构 3 vector（编译器成员析构等价）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/stMonsterCardBindInfo.inc"
{
    clear();
}

#include "STExpertJobOnlyOrig/dtor_stMonsterCardBindInfo.inc"
{
}

#include "STExpertJobOnlyOrig/clear_1.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        m_count[i] = 0;
        m_vec[i].clear();
    }
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
{
    for (int i = 0; i <= 2; ++i)
    {
        m_count[i] = 0;
        m_vec[i].clear();
    }
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
{
    m_count = 0;
    m_vec.clear();
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
{
    for (int i = 0; i <= 2; ++i)
    {
        m_count[i] = 0;
        m_vec[i].clear();
    }
}

// ============================================================================
// 查询方法
// ============================================================================

// ----------------------------------------------------------------------------
// GetCommonBindInfo — ORIG W 0x0822b366 range [...b374]：return this+0x84
// GetSpecialBindInfo — ORIG W 0x0822b358 range [...b366]：return this+0xc4
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetCommonBindInfo.inc"
{
    return &m_commonBindInfo;   // this+0x84
}

#include "STExpertJobOnlyOrig/GetSpecialBindInfo.inc"
{
    return &m_specialBindInfo;  // this+0xc4
}

// ----------------------------------------------------------------------------
// GetUpgradeCost — ORIG W 0x085d3d3a range [...d7dd]
//   n = m_upgradeCost.size()；if (n <= index) return 0；
//   else return m_upgradeCost.at(index)
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetUpgradeCost.inc"
{
    // P0：ORIG `call size; cmp index,%eax; setbe` 无槽、帧 0x18；具名 n 局部致帧 0x28。
    if (index >= m_upgradeCost.size())
        return 0;
    return m_upgradeCost.at(index);
}

// ----------------------------------------------------------------------------
// GetMonsterCardBindRate — ORIG W 0x0822b2d0 range [...b358]
//   it = m_monsterCardBindRateMap.find(key)；miss → out=零值、copy dword、
//   return 0；hit → copy dword(it->second)、return 1（dword 一次拷贝 = 两 word）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetMonsterCardBindRate.inc"
                                               stMonsterCardBindRate& out)
{
    std::map<int, stMonsterCardBindRate>::iterator it =
        m_monsterCardBindRateMap.find(key);
    if (it == m_monsterCardBindRateMap.end())
    {
        stMonsterCardBindRate zeroed;
        out = zeroed;   // ORIG miss：C1→dword 槽拷 out→return 0
        return false;
    }
    out = it->second;   // ORIG hit：pt→mov 0x4(%eax)→store out
    return true;
}

// ----------------------------------------------------------------------------
// _GetEnduranceRepairCost — ORIG W 0x085d3c26 range [...c74]
//   if (size() <= grade-1) return NULL（无符比较；grade=0 时恒真）
//   else return &m_enduranceRepairCost.at(grade-1)
// ----------------------------------------------------------------------------
stEnduranceRepairCost*
#include "STExpertJobOnlyOrig/_GetEnduranceRepairCost.inc"
{
    if (m_enduranceRepairCost.size() <= grade - 1u)  // 无符：grade=0 → 0xffffffff 恒真
        return 0;
    stEnduranceRepairCost* c = &m_enduranceRepairCost.at(grade - 1u);
    return c;
}

// ----------------------------------------------------------------------------
// GetDisjointMachineMaxGrade — ORIG W 0x085d3c0e range [...c26]：size()
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetDisjointMachineMaxGrade.inc"
{
    return static_cast<int>(m_enduranceRepairCost.size());
}

// ----------------------------------------------------------------------------
// GetDisjointMachineRepairCost — ORIG W 0x085d3c74 range [...cc8]
//   c = _GetEnduranceRepairCost(grade)；if (!c) return 0；
//   return c->m_field0 - (c->m_field0 * count) / c->m_field4
//   （乘除均为 32 位符号运算：imul/sar/idiv 截断口径）
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetDisjointMachineRepairCost.inc"
{
    const stEnduranceRepairCost* c = _GetEnduranceRepairCost(
        static_cast<unsigned int>(grade));
    if (c == 0)
        return 0;
    return c->m_field0 - (c->m_field0 * count) / c->m_field4;
}

// ----------------------------------------------------------------------------
// GetDisjointMachineRepair — ORIG W 0x085d3cc8 range [...d3a]
//   c = _GetEnduranceRepairCost(grade)；if (!c) return 0；
//   div = c->m_field0 / c->m_field4；if (div <= 0) return 0；
//   outCost = endurance - endurance % div；return endurance / div
// ----------------------------------------------------------------------------
#include "STExpertJobOnlyOrig/GetDisjointMachineRepair.inc"
                                                int& outCost)
{
    const stEnduranceRepairCost* c = _GetEnduranceRepairCost(
        static_cast<unsigned int>(grade));
    if (c == 0)
        return 0;
    const int div = c->m_field0 / c->m_field4;
    if (div <= 0)
        return 0;
    outCost = endurance - endurance % div;
    return endurance / div;
}

// 同构四联（find 命中返 second、未中返 -1；ORIG 为逐调用完整 find/end 展开，
// 不抽 helper 以保持函数体形态）：
#include "STExpertJobOnlyOrig/GetUnsealScrollPossibleLevel.inc"
{
    // ORIG：非 const iterator（_Rb_tree_iterator 直用，无 const 转换）。
    std::map<int, unsigned short>::iterator it = m_unsealScrollMap.find(level);
    if (it == m_unsealScrollMap.end())
        return -1;
    return static_cast<int>(it->second);
}

#include "STExpertJobOnlyOrig/GetAlchemistExtractionPossibleLevel.inc"
{
    std::map<int, unsigned short>::iterator it =
        m_alchemistExtractInfo.find(level);
    if (it == m_alchemistExtractInfo.end())
        return -1;
    return static_cast<int>(it->second);
}

#include "STExpertJobOnlyOrig/GetEnchanterExtractionPossibleLevel.inc"
{
    std::map<int, unsigned short>::iterator it =
        m_enchanterExtractInfo.find(level);
    if (it == m_enchanterExtractInfo.end())
        return -1;
    return static_cast<int>(it->second);
}

#include "STExpertJobOnlyOrig/GetDollControllerExtractionPossibleLevel.inc"
{
    std::map<int, unsigned short>::iterator it = m_dollExtractInfo.find(level);
    if (it == m_dollExtractInfo.end())
        return -1;
    return static_cast<int>(it->second);
}

// ResultInfo getter 三连（纯地址返回）
STExpertJobScript::stAlchemistExtractionResultInfo*
#include "STExpertJobOnlyOrig/GetAlchemistExtractionResultInfo.inc"
{
    return &m_alchemistResultInfo;  // this+0x14c
}

STExpertJobScript::stEnchanterExtractionResultInfo*
#include "STExpertJobOnlyOrig/GetEnchanterExtractionResultInfo.inc"
{
    return &m_enchanterResultInfo;  // this+0x17c
}

STExpertJobScript::stDollControllerResultInfo*
#include "STExpertJobOnlyOrig/GetDollControllerExtractionResultInfo.inc"
{
    return &m_dollResultInfo;       // this+0x18c
}
