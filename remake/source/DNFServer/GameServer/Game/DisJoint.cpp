// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DisJoint/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [b28d-64 判词汇总 · 泳道64] 本 TU 1 件：_GetAdditionalItem 0x084732b4
//   103/104 N封顶（P167 CLEAR；残余 = N1 折叠 add $0x150 vs $0x154 + bool
//   局部落槽 vs ORIG `test %al;je` 寄存器形）。

#include "DisJoint.h"
#include "CItem.h"

#include "DisJoint/get_rand_int.inc"

// ============================================================================
// DisJoint::GetAddtionalSkill @ 0x08473184
//   遍历 m_skills（[skill info] 行集，this+0xc0）找
//   m_field00==job && m_field04==level；命中 → outSkill=m_field08、
//   outLevel=m_field0c；未命中 → 两个 out 置 0。
//   ORIG 形态：局部 DisjointSkillScript entry 先走 ctor（{0xb,0,0,4}），
//   循环内 vector::operator[] 一次取行后拷贝到 entry 再双条件比较
//   （P54 逐字段赋值，勿用聚合拷贝）；循环界为无符号 size 比较（seta）。
//   [b28d-58] AE：outLevel=m_field0c 的 store ORIG 0x084731e5 为两段中转
//   （`mov -0x10(%ebp),%eax; mov %eax,%edx`）⇒ 该字段须为 unsigned int
//   （DisJoint.h 已改；探针 J1 int 单段 / J2 unsigned 两段）——改后逐条全等。
// ============================================================================
#include "DisJoint/GetAddtionalSkill.inc"
                                 int& outLevel)
{
    DisjointSkillScript entry;
    for (std::vector<DisjointSkillScript>::size_type i = 0;
         i < m_skills.size(); ++i)
    {
        entry = m_skills[i];
        if (entry.m_field00 == job && entry.m_field04 == level)
        {
            outSkill = entry.m_field08;
            outLevel = entry.m_field0c;
            return;
        }
    }
    outSkill = 0;
    outLevel = 0;
}

// ============================================================================
// DisJoint::_GetAdditionalItemInfo @ 0x08473224
//   m_additional[rarity]（+0x5c8+rarity*0x10）m_count==0 → 0；
//   否则 idx = get_rand_int(m_count*1000)/1000，返回 m_items[idx]。
//   （旧裸偏移转写迁出 ItemUpgradeVtableB2.cpp，改按 DisJoint.h 真实成员访问；
//   保留 ORIG 的 result(-0x10)/bound(-0xc) 槽结构，pick 内联进 operator[]。）
// ============================================================================
#include "DisJoint/_GetAdditionalItemInfo.inc"
{
    if (m_additional[rarity].m_count == 0)
    {
        return 0;
    }
    int result = 0;
    const int bound = m_additional[rarity].m_count * 1000;
    result = m_additional[rarity]
        .m_items[get_rand_int(bound) / 1000];
    return result;
}

// ============================================================================
// DisJoint::_GetAdditionalItem @ 0x084732b4（ORIG mangled ...EPK5CItemRiS3_：
//   outIdx/outCount 为 int& 引用参数，非指针）
//   outIdx = _GetAdditionalItemInfo(item->get_rarity())；非零时按
//   m_additionalConst[rarity].m_item*100 权重二选一分母浮点
//   （m_additionalWeight / m_additionalRate[rarity]），
//   outCount = (int)(grade / denom)（ORIG fistpl 前 fldcw 置 RC=truncate，
//   截断非四舍五入），最小 1。
//   分支内 get_grade/get_rarity 各自独立再调用（O0 无 CSE，P48）。
// ============================================================================
#include "DisJoint/_GetAdditionalItem.inc"
{
    // [b26N1] m_additionalWeight[rarity] 大偏移寻址：ORIG(4.1) 拆 add $0x150 进
    // scaled idx + disp 0x10；4.4 恒折 add $0x154 单步（N1 族折叠方向不可翻），
    // 其余全等，ndiff 12 封顶。
    outIdx = _GetAdditionalItemInfo(item->get_rarity());
    // ORIG 0x084732de `test %eax;setne %al;test %al;je`：outIdx!=0 物化为
    // bool 局部（P194 setne 形）。[b28c-21] 清 register asm 存量绑定（P0/N96），
    // 改普通 bool 自然形。
    bool nonzero = (outIdx != 0);
    if (nonzero)
    {
        // ORIG：m_item 先落 -0x18 再乘 100 落 -0x14，随后才 call get_rand_int。
        int w = m_additionalConst[item->get_rarity()].m_item;
        int weight = w;
        weight *= 100;
        if (get_rand_int(10000) < weight)
        {
            // ORIG 中间 float 槽：fstps/flds 往返后 fistpl 截断。
            const float v = (float)item->get_grade() /
                            m_additionalWeight[item->get_rarity()];
            outCount = (int)v;
        }
        else
        {
            const float v = (float)item->get_grade() /
                            m_additionalRate[item->get_rarity()];
            outCount = (int)v;
        }
        if (outCount < 1)
        {
            outCount = 1;
        }
    }
}
