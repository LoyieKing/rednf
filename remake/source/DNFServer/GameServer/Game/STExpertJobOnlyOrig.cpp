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
STExpertJobScript::stSelfDisjoinMaterialInfo::stSelfDisjoinMaterialInfo()
{
    clear();
}

// ----------------------------------------------------------------------------
// stMonsterCardBindRate::stMonsterCardBindRate — ORIG W 0x0822b1de
// range [0x0822b1de,0x0822b1f4)：movw $0,+0；movw $0,+2
// ----------------------------------------------------------------------------
STExpertJobScript::stMonsterCardBindRate::stMonsterCardBindRate()
{
    m_field0 = 0;
    m_field2 = 0;
}

// stMonsterCardBindResult — C1/C2 W 0x0896dc20 range [...dc37]
//   movl $0,(%eax)；movw $0,0x4(%eax)   // 只清 +0 dword 与 +4 word
// D1/D2 W 0x08974fac size 5：push ebp; mov ebp,esp; pop ebp; ret（空体）
// ----------------------------------------------------------------------------
STExpertJobScript::stMonsterCardBindResult::stMonsterCardBindResult()
{
    ZERO_WORD(&m_field4);
    m_field0 = 0;
}

STExpertJobScript::stMonsterCardBindResult::~stMonsterCardBindResult()
{
}

// ----------------------------------------------------------------------------
// stDollControllerResult — C1/C2 W 0x0896deac range [...dee5]
//   w+8=0 → w+6=w+8 → w+4=w+6 → d+0=(uint)(word)(w+4)   // 链式置零
// D1/D2 W 0x089751fa size 5（空体）
// ----------------------------------------------------------------------------
STExpertJobScript::stDollControllerResult::stDollControllerResult()
{
    ZERO_WORD(&m_field8);                       // +8 word = 0
    m_field6 = m_field8;                        // +6 word = w+8
    m_field4 = m_field6;                        // +4 word = w+6
    m_field0 = static_cast<int>(
        static_cast<unsigned short>(m_field4)); // +0 dword = zext(w+4)
}

STExpertJobScript::stDollControllerResult::~stDollControllerResult()
{
}

// ----------------------------------------------------------------------------
// stSelfDisjoinMaterialInfo::clear — ORIG W 0x0896dbe6 range [...dc0c]
//   +8=0；+4=+8；+0=+4（链式传递，全零效果）
// ----------------------------------------------------------------------------
void STExpertJobScript::stSelfDisjoinMaterialInfo::clear()
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
STExpertJobScript::stMonsterCardBindInfo::stMonsterCardBindInfo()
{
    clear();
}

STExpertJobScript::stMonsterCardBindInfo::~stMonsterCardBindInfo()
{
}

void STExpertJobScript::stMonsterCardBindInfo::clear()
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

void STExpertJobScript::stAlchemistExtractionResultInfo::clear()
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

void STExpertJobScript::stEnchanterExtractionResultInfo::clear()
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

void STExpertJobScript::stDollControllerResultInfo::clear()
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
STExpertJobScript::stMonsterCardBindInfo* STExpertJobScript::GetCommonBindInfo()
{
    return &m_commonBindInfo;   // this+0x84
}

STExpertJobScript::stMonsterCardBindInfo* STExpertJobScript::GetSpecialBindInfo()
{
    return &m_specialBindInfo;  // this+0xc4
}

// ----------------------------------------------------------------------------
// GetUpgradeCost — ORIG W 0x085d3d3a range [...d7dd]
//   n = m_upgradeCost.size()；if (n <= index) return 0；
//   else return m_upgradeCost.at(index)
// ----------------------------------------------------------------------------
int STExpertJobScript::GetUpgradeCost(unsigned int index)
{
    const std::vector<int>::size_type n = m_upgradeCost.size();
    if (n <= index)
        return 0;
    return m_upgradeCost.at(index);
}

// ----------------------------------------------------------------------------
// GetMonsterCardBindRate — ORIG W 0x0822b2d0 range [...b358]
//   it = m_monsterCardBindRateMap.find(key)；miss → out=零值、copy dword、
//   return 0；hit → copy dword(it->second)、return 1（dword 一次拷贝 = 两 word）
// ----------------------------------------------------------------------------
bool STExpertJobScript::GetMonsterCardBindRate(int key,
                                               stMonsterCardBindRate& out)
{
    std::map<int, stMonsterCardBindRate>::iterator it =
        m_monsterCardBindRateMap.find(key);
    if (it == m_monsterCardBindRateMap.end())
    {
        stMonsterCardBindRate zeroed;
        // ORIG 以单 dword 拷贝整体（sizeof==4）
        *reinterpret_cast<unsigned int*>(&out) =
            *reinterpret_cast<const unsigned int*>(&zeroed);
        return false;
    }
    *reinterpret_cast<unsigned int*>(&out) =
        *reinterpret_cast<const unsigned int*>(&it->second);
    return true;
}

// ----------------------------------------------------------------------------
// _GetEnduranceRepairCost — ORIG W 0x085d3c26 range [...c74]
//   if (size() <= grade-1) return NULL（无符比较；grade=0 时恒真）
//   else return &m_enduranceRepairCost.at(grade-1)
// ----------------------------------------------------------------------------
stEnduranceRepairCost*
STExpertJobScript::_GetEnduranceRepairCost(unsigned int grade)
{
    const std::vector<stEnduranceRepairCost>::size_type n =
        m_enduranceRepairCost.size();
    if (n <= grade - 1u)            // 无符口径：grade=0 → 0xffffffff 恒真
        return 0;
    return &m_enduranceRepairCost.at(grade - 1u);
}

// ----------------------------------------------------------------------------
// GetDisjointMachineMaxGrade — ORIG W 0x085d3c0e range [...c26]：size()
// ----------------------------------------------------------------------------
int STExpertJobScript::GetDisjointMachineMaxGrade()
{
    return static_cast<int>(m_enduranceRepairCost.size());
}

// ----------------------------------------------------------------------------
// GetDisjointMachineRepairCost — ORIG W 0x085d3c74 range [...cc8]
//   c = _GetEnduranceRepairCost(grade)；if (!c) return 0；
//   return c->m_field0 - (c->m_field0 * count) / c->m_field4
//   （乘除均为 32 位符号运算：imul/sar/idiv 截断口径）
// ----------------------------------------------------------------------------
int STExpertJobScript::GetDisjointMachineRepairCost(int grade, int count)
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
int STExpertJobScript::GetDisjointMachineRepair(int grade, int endurance,
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
int STExpertJobScript::GetUnsealScrollPossibleLevel(int level)
{
    // ORIG：非 const iterator（_Rb_tree_iterator 直用，无 const 转换）。
    std::map<int, unsigned short>::iterator it = m_unsealScrollMap.find(level);
    if (it == m_unsealScrollMap.end())
        return -1;
    return static_cast<int>(it->second);
}

int STExpertJobScript::GetAlchemistExtractionPossibleLevel(int level)
{
    std::map<int, unsigned short>::iterator it =
        m_alchemistExtractInfo.find(level);
    if (it == m_alchemistExtractInfo.end())
        return -1;
    return static_cast<int>(it->second);
}

int STExpertJobScript::GetEnchanterExtractionPossibleLevel(int level)
{
    std::map<int, unsigned short>::iterator it =
        m_enchanterExtractInfo.find(level);
    if (it == m_enchanterExtractInfo.end())
        return -1;
    return static_cast<int>(it->second);
}

int STExpertJobScript::GetDollControllerExtractionPossibleLevel(int level)
{
    std::map<int, unsigned short>::iterator it = m_dollExtractInfo.find(level);
    if (it == m_dollExtractInfo.end())
        return -1;
    return static_cast<int>(it->second);
}

// ResultInfo getter 三连（纯地址返回）
STExpertJobScript::stAlchemistExtractionResultInfo*
STExpertJobScript::GetAlchemistExtractionResultInfo()
{
    return &m_alchemistResultInfo;  // this+0x14c
}

STExpertJobScript::stEnchanterExtractionResultInfo*
STExpertJobScript::GetEnchanterExtractionResultInfo()
{
    return &m_enchanterResultInfo;  // this+0x17c
}

STExpertJobScript::stDollControllerResultInfo*
STExpertJobScript::GetDollControllerExtractionResultInfo()
{
    return &m_dollResultInfo;       // this+0x18c
}
