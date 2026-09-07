// ============================================================================
// DisJoint.cpp — DisJoint 查询方法实现（本批次独占文件）
//
// 依据 docs/class_func_reports/DisJoint/GetAddtionalSkill.md（## 汇编 +
// ## 反编译 C 全文读取后转写）：
//   DisJoint::GetAddtionalSkill(int,int,int&,int&)  T 0x08473184
//
// [skill info] 行元素语义来自 importDisjointScript（ScriptDepsA.cpp 实证）：
//   m_field00 = getCharacterJob(str)  职业
//   m_field04 = ScanInt()             技能等级
//   m_field08 = ScanInt()             附加技能索引
//   m_field0c = getElement(str)       附加技能元素/等级
// 其余 DisJoint 方法（GetResult 由 ItemUpgradeVtableB2.cpp 交付；
// GetPredictResult 由 ExpertJobLink4.cpp 交付；LoadDisJoint 待脚本导入域批次）
// ——本 TU 补 GetAddtionalSkill 与 _GetAdditionalItem/_GetAdditionalItemInfo
// （自 ItemUpgradeVtableB2.cpp 迁入，按真实成员布局重写）。
// ============================================================================

#include "DisJoint.h"
#include "CItem.h"

int get_rand_int(int range);

// ============================================================================
// DisJoint::GetAddtionalSkill @ 0x08473184
//   遍历 m_skills（[skill info] 行集，this+0xc0）找
//   m_field00==job && m_field04==level；命中 → outSkill=m_field08、
//   outLevel=m_field0c；未命中 → 两个 out 置 0。
//   ORIG 形态：局部 DisjointSkillScript entry 先走 ctor（{0xb,0,0,4}），
//   循环内 vector::operator[] 一次取行后拷贝到 entry 再双条件比较
//   （P54 逐字段赋值，勿用聚合拷贝）；循环界为无符号 size 比较（seta）。
// ============================================================================
void DisJoint::GetAddtionalSkill(int job, int level, int& outSkill,
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
int DisJoint::_GetAdditionalItemInfo(int rarity)
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
void DisJoint::_GetAdditionalItem(const CItem* item, int& outIdx, int& outCount)
{
    // [b26N1] m_additionalWeight[rarity] 大偏移寻址：ORIG(4.1) 拆 add $0x150 进
    // scaled idx + disp 0x10；4.4 恒折 add $0x154 单步（N1 族折叠方向不可翻），
    // 其余全等，ndiff 12 封顶。
    outIdx = _GetAdditionalItemInfo(item->get_rarity());
    // ORIG 0x084732de `test %eax;setne %al;test %al;je`：outIdx!=0 物化为
    // register bool（P194 setne 形），非直接 if(outIdx!=0)（那会少 setne+test）。
    register bool nonzero asm("al") = (outIdx != 0);
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
