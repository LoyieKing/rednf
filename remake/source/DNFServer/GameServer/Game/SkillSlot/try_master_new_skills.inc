// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a,N82）→ 试=prett 缺口实证（ORIG 串
// `int SkillSlot::try_master_new_skills(...)`@0x8cde540 无指令引用 = 孤立符号）+ nd
// 339/334（12×`mov -0x30(%ebp),%eax` 重读）+ p167 calls CLEAR（preSkill 已是 `!=` 正极性）
// ⇒ 维持（新=DIFF gap=+5, blocks=50）。
int SkillSlot::try_master_new_skills(int skillIdx, int level, int job,
                                     int firstGrow, int secondGrow,
                                     int addLevel)
{
    // [round1-L19 2026-09-09] 复修（同 master_new_skill）：删 quest/world 具名局部
    // ⇒ 339/337 gap=2 → **339/333 gap=6**、ndiff 452→376；剩余同族（N18a 索引 +
    // list 迭代器槽差 + P168a③ bool 物化）。
    // [b28d-solo-SkillSlot] 判词（N82/P75 封顶；339 vs 347，gap=+8）：
    // 本件在 b28d-solo 波次内未完成逐条收敛（预算封口）。已取证：结构与 ORIG
    // 同族（preSkillList const_iterator + `it++` 后置、_pre_skill 逐字段、
    // can_learn/fitness/needSP 段），p167 残差 1 处
    // `_List_const_iterator<_pre_skill>::operator==` EXTRA ⇒ 迭代器比较写法
    // 与 ORIG 差一处（ORIG 用 ne 判续 + 循环后 eq，见 P7；本件为 list）。
    // 残余主体为跨版本槽/寄存器分配（同族已由 get_give_skill/InitSkill 实证）。
    // 后续重开时优先：`for (it = begin(); it != end(); it++)` 形 + P7 双比较。
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return -1;
    }
    int group = skill->get_group();
    if (m_parent == 0)
    {
        return -4;
    }
    int slot = get_skillslot_no(m_tempSkillSlot, skillIdx, group, true);
    // [H10-SkillSlot 2026-09-11] 判据 P0（槽/初始化直证）：ORIG 0x086074ee
    // `movl $0x0,-0x1c(%ebp)` 位于 `cmpl $0xffffffff,-0x24(%ebp)` **之前**
    // （无条件），且 0x0860752e 又有一处 `movl $0x0,-0x1c(%ebp)`（then 分支）
    // ⇒ 原码为 `int curLevel = 0;` + then 内重复 `curLevel = 0;`。我方缺无条件
    // 初始化（0x0 立即数计数 14 vs 12）。同 master_new_skill 的已对齐形。
    int curLevel = 0;
    int targetLevel;
    if (slot == -1)
    {
        curLevel = 0;
        targetLevel = addLevel;
    }
    else
    {
        curLevel = m_tempSkillSlot[slot].m_level;
        targetLevel = m_tempSkillSlot[slot].m_level + addLevel;
    }
    char isSteal = 0;
    if (skill->IsStealable(firstGrow, secondGrow) != 0)
    {
        m_parent->getCurCharacQuestR()->isClearQuest(0x163);
        // [V331b02 2026-09-12 修复·死块族] ORIG 0x0860757e `jmp 0x0860759b`（无条件）：
        //   紧随的 is_clear_stealingSkillMission 判空早退块（ORIG 0x08607580..0x08607599）
        //   **无任何跳转入口**（deadblk 静态可达性扫描：9 条不可达）⇒ 运行期不可达，
        //   ORIG 活路径为 0x0860759b `mov $0x0,%eax; test %al,%al; je 0x086075ae`
        //   （恒 0 ⇒ 永不 return -1，直接 isSteal=1 + GetStealLevelPenalty 扣减）。
        //   旧形在可达路径调用并在 ==0 时 return -1 = 多一条失败早退（行为不同）。
        //   P337 形：恒 false volatile 守卫保留该调用点（p167 调用多重集不变）且运行期不可达。
        volatile bool deadStealCheck = false;
        if (deadStealCheck)
        {
            if (m_parent->is_clear_stealingSkillMission() == 0)
            {
                return -1;
            }
        }
        isSteal = 1;
        level = level - skill->GetStealLevelPenalty();
        // [g11-slot 2026-09-10] P0/P183 真形（直证）：ORIG 0x086075d5
        // `movzwl %ax,%eax` + 0x086075d8 `add %eax,0x10(%ebp)` 位于 isSteal
        // 分支内、无具名槽；后续 can_learn 直读形参（0x08607663
        // `mov 0x10(%ebp),%eax`）。原具名 `unsigned short overLevel` 是
        // 我方多出的槽（我方 -0x1a store/load 往返，ORIG 全无）。
    }
    // [J10GameRest.J10SkillSlot 2026-09-11] 语义轴修正（同 master_new_skill）：
    // ORIG 0x0860755a `je 0x086075c0`（IsStealable==0 直跳 premium 段起点）
    // ⇒ `level += GetPremiumInfo()->getOverSkillLevel()`（0x086075c0..0x086075d8
    // `movzwl %ax,%eax; add %eax,0x10(%ebp)`）在 isSteal 分支**外**无条件执行；
    // 旧形误置 if 内 ⇒ 非偷取路径 level 少加 overSkillLevel。
    level = level + m_parent->GetPremiumInfo()->getOverSkillLevel();
    if (G_GameWorld()->IsPvPSkilTreeChannel() == 0)
    {
        if (skill->can_learn(level, firstGrow, secondGrow,
                             targetLevel) != 1)
        {
            return -3;
        }
    }
    else
    {
        if (((CUserCharacInfo*)m_parent)->get_pvp_grade() <
            skill->m_pvpSkillGrade)
        {
            return -3;
        }
        if (skill->can_learn(0x46, firstGrow, secondGrow, targetLevel) != 1)
        {
            return -3;
        }
    }
    int fitness = skill->get_second_growtype_fitness(firstGrow, secondGrow);
    if (fitness == 0)
    {
        fitness = skill->get_fitness(firstGrow);
    }
    int needSP = skill->get_need_sp_sfp_for_level(curLevel, targetLevel);
    if (needSP < 0)
    {
        return -4;
    }
    if (0 < fitness)
    {
        needSP = ((100 - fitness) * needSP) / 100;
    }
    if (isSteal != 0)
    {
        needSP = getSkillSpendSPOnSteal(needSP, skill->GetStealSPPenalty());
    }
    if (needSP < 0)
    {
        needSP = 0;
    }
    int* remain;
    if (skill->IsSpecialSkill() == 0)
    {
        remain = get_temp_remain_sp();
    }
    else
    {
        remain = get_temp_remain_sfp();
    }
    if (remain == 0)
    {
        return -1;
    }
    if (*remain < needSP)
    {
        return -2;
    }
    std::list<_pre_skill>::const_iterator it;
    const std::list<_pre_skill>& preSkillList = skill->m_preSkillList;
    it = preSkillList.begin();
    while (it != preSkillList.end())
    {
        _pre_skill preSkill = *it;
        if (check_skill_mastered(m_tempSkillSlot, preSkill.m_index,
                                 preSkill.m_jobType, preSkill.m_group) != 1)
        {
            return -3;
        }
        it++;
    }
    if (slot == -1)
    {
        *remain = *remain - needSP;
        slot = insert_skill(m_tempSkillSlot, skillIdx, group,
                            skill->is_active_skill() != 0, addLevel);
    }
    else
    {
        m_tempSkillSlot[slot].m_level = (char)targetLevel;
        *remain = *remain - needSP;
    }
    return slot;
}