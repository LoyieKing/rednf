// ============================================================================
// PvPSkillTreeParameterScript 查询（ORIG 0x08a5e178 / 0x08a5dd62, 符号 T）
// 最小实现保证符号 T + 可链接；完整脚本解析逻辑待细化。
// [P6S5 2026-09-11 本波复核] getPvPSkillPoint 131/131（gap +0，NEAR 残差 = 4.1/4.4
//   寄存器角色，非结构）；本 TU 的 `_ZNSt20__uninitialized_copyILb0EE…` DIFF 残余与
//   ILb0/ILb1 头轴同源（`APSystemTypes.h:48` 的显式 `~_TodayRewardItem();`），但
//   P3/P7/P6 三方实测：我方无 `new_allocator<_TodayRewardItem>::destroy` 实例 ⇒ 删
//   `~T()` 会丢 D1/D2（ORIG 有）⇒ **维持现状（不改 .h）**，已上报。
// ============================================================================
int PvPSkillTreeParameterScript::getPvPSkillPoint(
    int job, int firstGrow, int secondGrow, int grade, bool flag) const
{
    // ORIG 0x08a5dd62：grade 下限钳制到 m_field4c；
    // m_map18[grade][job*5+firstGrow] 为已投入点数；
    // m_map30[(job,firstGrow)] 的 m_pointCost(+0x40)：flag=true 且已有点数则扣减，
    // flag=false 且点数为 0 则取消耗值。secondGrow 在 ORIG 中被强制置 0（未参与）。
    // [b28d-56 判词·P167/P212] ORIG 0x08a5dd62 用三参 ctor `PvPSkillTreeKey(h,h,h)`
    // （_ZN15PvPSkillTreeKeyC1Ehhh，ORIG W 0x08a5e768），第三实参为立即数 0（ORIG
    // 无 secondGrow 形参槽写）；且 `movl $0x0,-0xc`（points）在**函数头**（P212 全
    // 头部声明）。旧码默认 ctor + 逐字段赋值 ⇒ p167 MISSING 三参 ctor/EXTRA 默认 ctor。
    // [b28d-56 判词·N] 残余 N82：ORIG 131 条 vs 我们 131 条。旧判词把 grade 钳制的
    // `jge` vs `jle` 归为 P59/P222 纯极性差 —— N3-T3 复核推翻：两者落点都是钳制体，
    // jge 跳**过**它、jle 也跳**过**它，故 cc 差 ⇒ **条件反**（真语义 bug，见下）。
    // 现残余仅剩 map 迭代器/临时槽号映射（-0x24/-0x18/-0x13 vs -0x28/-0x14/-0x23）。
    // [N3-T3 语义修正 · 真 bug] ORIG 0x08a5dd78-0x08a5dd88：
    //   mov 0x4c(%eax),%edx ; mov 0x18(%ebp),%eax ; cmp %eax,%edx ; jge 0x8a5dd8b
    //   ⇒ 计算 m_field4c − grade，`jge`（field4c >= grade）**跳过**钳制体
    //   ⇒ 等价 `if (grade > m_field4c) grade = m_field4c;`（取 min = 上限钳制）。
    //   原写法 `grade < m_field4c` 编译成 `jle` 跳过 ⇒ 取 **max**（下限钳制），
    //   与 ORIG 相反：field4c=10/grade=50 时 ORIG 返回 10、旧码返回 50。
    //   操作数序两边相同（field4c→%edx 先于 grade→%eax），改写后 cc 由 jle 变 jge。
    int points = 0;
    if (grade > m_field4c)
        grade = m_field4c;

    std::map<int, std::map<int, int> >::const_iterator gradeIt = m_map18.find(grade);
    if (gradeIt != m_map18.end())
    {
        std::map<int, int>::const_iterator slotIt =
            gradeIt->second.find(job * 5 + firstGrow);
        if (slotIt != gradeIt->second.end())
            points += slotIt->second;
    }

    // [N3-T3 · P212/P329] ORIG 0x08a5de56 段：
    //   movl $0x0,0x14(%ebp) ; mov 0x14(%ebp),%eax ; movzbl %al,%ecx
    //   ⇒ secondGrow **形参槽**清零后回读参与 key 构造（与 getGiveSkill 0x08a5e17e 同形），
    //   而非把立即数 0 直填第三实参槽（原写法少这 2 条 = 本件 gap+2 的另一来源）。
    secondGrow = 0;
    PvPSkillTreeKey key(static_cast<unsigned char>(job),
                        static_cast<unsigned char>(firstGrow),
                        static_cast<unsigned char>(secondGrow));
    std::map<PvPSkillTreeKey,
             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::const_iterator costIt =
        m_map30.find(key);
    if (costIt != m_map30.end())
    {
        if (flag)
        {
            if (points != 0)
                points -= costIt->second.m_pointCost;
        }
        else if (points == 0)
        {
            points = costIt->second.m_pointCost;
        }
    }
    return points;
}