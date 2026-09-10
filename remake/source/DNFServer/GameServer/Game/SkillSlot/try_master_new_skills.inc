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
    int curLevel;
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
        if (m_parent->is_clear_stealingSkillMission() == 0)
        {
            return -1;
        }
        isSteal = 1;
        level = level - skill->GetStealLevelPenalty();
    }
    unsigned short overLevel =
        m_parent->GetPremiumInfo()->getOverSkillLevel();
    if (G_GameWorld()->IsPvPSkilTreeChannel() == 0)
    {
        if (skill->can_learn(level + overLevel, firstGrow, secondGrow,
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
