// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSkill/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 技能定义类 CSkill 还原（G1 玩家域第一批）。
// 逐函数对照 docs/class_func_reports/CSkill.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
#include <string.h>

#include "CSkill.h"
#include "CDataManager.h"

// ---- STSecondGrowTypeFitness（ORIG 0x083748da / 0x083748ee）----
#include "CSkill/clear.inc"
{
    memset(m_fitness, 0, sizeof(m_fitness));
    memset(m_maxLevel, 0, sizeof(m_maxLevel));
}

#include "CSkill/STSecondGrowTypeFitness.inc"
{
    clear();
}

// ---- 构造 / 析构（ORIG 0x08374944 / 0x08379fd8）----
#include "CSkill/CSkill.inc"
{
    memset(m_fitnessSecondGrowType, 0, sizeof(m_fitnessSecondGrowType));
    m_isSpecialSkill = 0;
    m_keyCommandList.clear();
    m_isCommandCustomizing = 1;
    memset(m_skillName, 0, sizeof(m_skillName));
    for (int i = 0; i <= 5; ++i)
    {
        m_fitGrowType[i] = -1;
    }
}

#include "CSkill/dtor_CSkill.inc"
{
}

// ORIG 0x83507e8：组号重排（flag → 3；0..3 按 count 改 1/0；4 → 2）
#include "CSkill/reform_ui_group_no.inc"
{
    if (flag)
    {
        uiGroupNo = 3;
        return;
    }
    switch (uiGroupNo)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        if (count > 2)
        {
            uiGroupNo = 0;
        }
        else
        {
            uiGroupNo = 1;
        }
        break;
    case 4:
        uiGroupNo = 2;
        break;
    }
}

// ---- 简单访问器（ORIG weak inline，单条指令形态）----
#include "CSkill/get_index.inc"
{
    return m_index;
}

#include "CSkill/get_group.inc"
{
    return m_group;
}

#include "CSkill/set_index.inc"
{
    m_index = idx;
}

#include "CSkill/set_group.inc"
{
    m_group = group;
}

#include "CSkill/set_job_type.inc"
{
    m_jobType = job;
}

#include "CSkill/set_skill_type.inc"
{
    m_skillType = type;
}

#include "CSkill/set_level_limit.inc"
{
    m_levelLimit = limit;
}

#include "CSkill/SetStealable.inc"
{
    m_stealable = flag;
}

#include "CSkill/SetStealLevelPenalty.inc"
{
    m_stealLevelPenalty = value;
}

#include "CSkill/SetStealSPPenalty.inc"
{
    m_stealSPPenalty = value;
}

#include "CSkill/GetStealLevelPenalty.inc"
{
    return m_stealLevelPenalty;
}

#include "CSkill/GetStealSPPenalty.inc"
{
    return m_stealSPPenalty;
}

#include "CSkill/set_fitGrowType.inc"
{
    m_fitGrowType[idx] = value;
}

#include "CSkill/set_fitness.inc"
{
    m_fitness[idx] = value;
}

#include "CSkill/get_fitness.inc"
{
    return m_fitness[idx];
}

#include "CSkill/set_type_max_level.inc"
{
    m_typeMaxLevel[idx] = value;
}

#include "CSkill/get_type_max_level.inc"
{
    return m_typeMaxLevel[idx];
}

#include "CSkill/set_fitness_second_growtype.inc"
{
    if (secondGrowType <= 0)
        return;
    if (secondGrowType > 2)
        return;
    m_fitnessSecondGrowType[secondGrowType - 1] = value;
}

#include "CSkill/get_fitness_second_growtype.inc"
{
    if (secondGrowType < 1 || 2 < secondGrowType)
    {
        return -1;
    }
    return m_fitnessSecondGrowType[secondGrowType - 1];
}

#include "CSkill/set_second_growtype_fitness.inc"
                                         int value)
{
    if (firstGrowType < 0)
        return;
    if (firstGrowType > 5)
        return;
    if (secondGrowType <= 0)
        return;
    if (secondGrowType > 2)
        return;
    m_secondGrowTypeFitness[firstGrowType].m_fitness[secondGrowType - 1] =
        value;
}

#include "CSkill/get_second_growtype_fitness.inc"
                                        int secondGrowType) const
{
    if (firstGrowType < 0 || 6 < firstGrowType || secondGrowType < 0 ||
        2 < secondGrowType)
    {
        return -1;
    }
    if (firstGrowType >= 0 && firstGrowType <= 6 && secondGrowType == 0)
    {
        return 0;
    }
    return m_secondGrowTypeFitness[firstGrowType].m_fitness[secondGrowType - 1];
}

#include "CSkill/set_second_growtype_max_level.inc"
                                           int secondGrowType, int value)
{
    if (firstGrowType < 0)
        return;
    if (firstGrowType > 5)
        return;
    if (secondGrowType <= 0)
        return;
    if (secondGrowType > 2)
        return;
    m_secondGrowTypeFitness[firstGrowType].m_maxLevel[secondGrowType - 1] =
        value;
}

#include "CSkill/get_second_growtype_max_level.inc"
                                          int secondGrowType) const
{
    if (firstGrowType < 0 || 5 < firstGrowType || secondGrowType < 1 ||
        2 < secondGrowType)
    {
        return -1;
    }
    if (firstGrowType >= 0 && firstGrowType <= 6 && secondGrowType == 0)
    {
        return 0;
    }
    return m_secondGrowTypeFitness[firstGrowType].m_maxLevel[secondGrowType - 1];
}

#include "CSkill/set_prelearned_skill.inc"
{
    std::list<_pre_skill>::iterator it;
    int count = (int)m_preSkillList.size();
    CSkill* found;
    int i;
    _pre_skill* pSkill;
    it = m_preSkillList.begin();
    for (i = 0; i < count; ++i)
    {
        pSkill = &*it;
        found = G_CDataManager()->m_skillList->find_skill(
            m_jobType, pSkill->m_index);
        if (found != 0)
        {
            pSkill->m_group = found->get_group();
        }
        it++;
    }
}

#include "CSkill/add_need_sp.inc"
{
    m_needSP.push_back(value);
}

#include "CSkill/add_need_sfp.inc"
{
    m_needSFP.push_back(value);
}

#include "CSkill/add_learned_skill.inc"
{
    m_preSkillList.push_back(skill);
}

#include "CSkill/setSkillName.inc"
{
    strncpy(m_skillName, name, 0x64);
}

#include "CSkill/getSkillName.inc"
{
    return m_skillName;
}

#include "CSkill/is_active_skill.inc"
{
    return m_skillType == 0;
}

#include "CSkill/IsSpecialSkill.inc"
{
    if (m_isSpecialSkill == 0)
    {
        return false;
    }
    return true;
}

#include "CSkill/isCommandCustomizing.inc"
{
    return m_isCommandCustomizing;
}

// ---- 主逻辑（ORIG strong）----
// ORIG 0x83504ec _ZNK6CSkill11IsStealableEii：体内 `xor $0x1,%eax; test %al,%al` bool 物化形
#include "CSkill/IsStealable.inc"
{
    if (m_stealable == 0)
    {
        return 0;
    }
    for (int i = 0; i <= 5; ++i)
    {
        if (m_fitGrowType[i] == firstGrowType)
        {
            return 0;
        }
    }
    if (0 < secondGrowType)
    {
        for (int i = 0; i <= 1; ++i)
        {
            if (m_fitnessSecondGrowType[i] == secondGrowType)
            {
                return 0;
            }
        }
    }
    return 1;
}

#include "CSkill/can_learn.inc"
                       int targetLevel) const
{
    // [M5C-SkillSlang 2026-09-11 · P339 + 三元形变，ORIG 0x0835039e] 旧判词只覆盖 ① 的
    // setcc 残余（P194/N-88），② ③ 为源码形差，本波改：
    // ① B 段 ORIG 0x0835040d `cmp %edx,%eax; jge 0x08350425`，then 再发 call
    //    （0x0835041e）、两臂各自落 eax 后 **汇合点** 0x0835043a `mov %eax,-0x14` 才写槽
    //    ⇒ ORIG 是三元赋值；我方 if/else 双写槽（+1）且条件被 4.4 物化 setl+test+je（+2）。
    // ② else 段 ORIG 0x083504a0 `mov -0xc(%ebp),%edx; cmp %edx,%eax; jle 0x083504a9;
    //    mov %edx,%eax`（**不重算** v+1，两值择一）⇒ 三元 min 形；我方 if/else 反极性
    //    `jg` + 重算整段（+5）。
    // ③ ORIG 三个 int 局部槽 -0x14(maxLevel)/-0x10(needLevel)/-0xc(secondGrowMax)
    //    （B 段写 -0x14、else 段临时写 -0xc、尾部写 -0x10）⇒ 头部全声明 + 该声明序。
    int maxLevel;
    int needLevel;
    int secondGrowMax;
    if (secondGrowType < 1)
    {
        if (targetLevel < 1 || get_type_max_level(firstGrowType) < targetLevel)
        {
            return false;
        }
        maxLevel = (get_type_max_level(firstGrowType) <
                    (level - m_levelLimit) + 1)
                       ? get_type_max_level(firstGrowType)
                       : (level - m_levelLimit) + 1;
    }
    else
    {
        secondGrowMax = get_second_growtype_max_level(firstGrowType,
                                                      secondGrowType);
        if (secondGrowMax == 0)
        {
            secondGrowMax = get_type_max_level(firstGrowType);
        }
        if (targetLevel < 1 || secondGrowMax < targetLevel)
        {
            return false;
        }
        maxLevel = ((level - m_levelLimit) + 1 <= secondGrowMax)
                       ? (level - m_levelLimit) + 1
                       : secondGrowMax;
    }
    if (maxLevel < targetLevel)
    {
        return false;
    }
    needLevel = m_levelLimit + m_fieldC8 * (targetLevel - 1);
    return needLevel <= level;
}

#include "CSkill/getMaxSkillLevel.inc"
                             int secondGrowType) const
{
    // b28d-37 [真语义 bug 修复，ORIG 0x08350700]：maxLevel 须取 min((level-m_levelLimit)+1, maxLevel)（旧实现恒无操作致上限偏大）；残 cmov（P230① -march=i486 可消，待路由）+ N82 帧。
    int maxLevel;
    if (secondGrowType < 1)
    {
        maxLevel = get_type_max_level(firstGrowType);
    }
    else
    {
        maxLevel = get_second_growtype_max_level(firstGrowType,
                                                 secondGrowType);
        if (maxLevel == 0)
        {
            maxLevel = get_type_max_level(firstGrowType);
        }
    }
    if (maxLevel == 0)
    {
        return -1;
    }
    // b28d-37 [真语义 bug 修正，ORIG 0x08350785]：ORIG 为
    // `eax=v+1; edx=maxLevel; cmp %edx,%eax; jle L; mov %edx,%eax; L: mov %eax,-0x10`
    // ⇒ maxLevel = min((level-m_levelLimit)+1, maxLevel)（v+1<=maxLevel 时上限缩小）。
    // 旧实现 `if (v+1 > keep) maxLevel = keep;` 恒为无操作（keep==maxLevel），
    // 导致返回上限偏大（v+1<=maxLevel 时未缩小）。改三元 min 形。
    // [N 封顶] 残差：4.4 三元产 `cmovle`（ORIG 为 jle 分支），P230① -march=i486
    // 可消 cmov 族但本波不改路由 ⇒ 待 Main 统一路由变更后复测；另帧 0x20 vs
    // ORIG 0x38 属 N82 槽分区。78/77。
    // [P6S3 round6 · 路由杠杆实测（证据留给 Main）] ① 同源码整 TU 叠
    //  `-march=i486 -mtune=generic` 后：本件 78/77 DIFF → **NEAR 78/79**（cmov 归零，
    //  出 ORIG 形 `add $0x1,%eax; jle`），TU 四数 near 0→2 / diff 2→0（零回退）；
    //  残差仅剩帧 0x38 vs 0x20 + 槽 -0x10/-0xc/-0x1c vs -0x8/-0x4/-0x14（N82 槽分区）。
    //  ② **per-function 不用改路由也能落地**：探针 /tmp/p6/sc/p2.cpp 实证
    //  `__attribute__((target("arch=i486")))` 挂在**声明**上（.h 侧）即可让本函数出
    //  ORIG 形（`addl $1,%eax; jle`，无 cmov）；挂在定义上被 4.4.7 C++ 拒
    //  （「attributes are not allowed on a function-definition」）；`#pragma GCC
    //  push_options/target("arch=i486")` 在 4.4.7 对本函数**无效**（探针 p4.cpp 仍 cmov）。
    //  ⇒ 若 .h 侧愿加该声明属性（P7HeaderAxis/P6AuditOldN 域），本件与 can_learn 可
    //  直接取 i486 形；否则需整 TU 路由。两者都属头/路由轴，本泳道不再动。
    maxLevel = ((level - m_levelLimit) + 1 < maxLevel)
                   ? (level - m_levelLimit) + 1
                   : maxLevel;
    int q = (level - m_levelLimit) / m_fieldC8 + 1;
    if ((unsigned)(q < maxLevel ? q : maxLevel) >> 31)
    {
        return 0;
    }
    return q < maxLevel ? q : maxLevel;
}

#include "CSkill/get_need_sp_for_level.inc"
{
    std::list<int>::const_iterator it;
    int count = (int)m_needSP.size();
    if (count < targetLevel)
    {
        return -1;
    }
    if (level < 0)
    {
        return -1;
    }
    int total = 0;
    it = m_needSP.begin();
    for (int i = 1; i < targetLevel; ++i)
    {
        if (i > level && i < targetLevel)
        {
            total += *it;
        }
        it++;
    }
    total += *it;
    return total;
}

#include "CSkill/get_need_sfp_for_level.inc"
{
    std::list<int>::const_iterator it;
    int count = (int)m_needSFP.size();
    if (count < targetLevel)
    {
        return -1;
    }
    if (level < 0)
    {
        return -1;
    }
    int total = 0;
    it = m_needSFP.begin();
    for (int i = 1; i < targetLevel; ++i)
    {
        if (i > level && i < targetLevel)
        {
            total += *it;
        }
        it++;
    }
    total += *it;
    return total;
}

#include "CSkill/get_need_sp_sfp_for_level.inc"
{
    if (IsSpecialSkill())
    {
        return get_need_sfp_for_level(level, targetLevel);
    }
    return get_need_sp_for_level(level, targetLevel);
}

#include "CSkill/get_guildSkillNeedGSP.inc"
{
    if ((unsigned)m_guildSkillNeedGSP.size() < (unsigned)count)
    {
        return -1;
    }
    return m_guildSkillNeedGSP.at(count - 1);
}

#include "CSkill/get_guildSkillNeedGold.inc"
{
    if ((unsigned)m_guildSkillNeedGold.size() < (unsigned)count)
    {
        return -1;
    }
    return m_guildSkillNeedGold.at(count - 1);
}

#include "CSkill/get_needGuildLevel.inc"
{
    return m_needGuildLevel + m_needGuildLevelAdd * (count - 1);
}
