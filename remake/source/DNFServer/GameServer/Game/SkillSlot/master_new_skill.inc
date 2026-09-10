int SkillSlot::master_new_skill(int skillIdx, int level, int job,
                                int firstGrow, int secondGrow, int addLevel)
{
    // [round1-L19 2026-09-09] 复修（P0/P183/P248 判据）：删 `UserQuest* quest` 与
    // `GameWorld* world` 两个具名中间局部（ORIG Ghidra `this_00`/`this_02` 为寄存器
    // 无槽）⇒ 读数 464/462 gap=2 → **464/458 gap=6**、ndiff 584→562；
    // 剩余 = ORIG 0x08603bb6 `xor $1;test %al,%al;jmp`（isClearQuest 死 bool 物化，
    // P168a③ 跨版本族）+ preSkill 迭代器段槽差 0x10 + 二维索引中转（N18a）。
    // [b28d-solo-SkillSlot] 判词（N82/P75 封顶；464 vs 472，gap=+8）：
    // 本件在 b28d-solo 波次内未完成逐条收敛（预算封口）。已取证：p167 残差 1 处
    // `_List_const_iterator<_pre_skill>::operator==` EXTRA（同 try_master_new_skills，
    // 迭代器比较形待改 P7 形）。其余为跨版本槽/寄存器/求值序族（同族实证）。
    // 后续重开时优先：preSkill 循环改 `for(it=begin();it!=end();it++)`。
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
    int slot = get_skillslot_no(
        skillIdx, group,
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex(),
        1);
    int curLevel = 0;
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return -4;
    }
    int targetLevel;
    if (slot != -1)
    {
        curLevel = buf[slot].m_level;
        targetLevel = buf[slot].m_level + addLevel;
    }
    else
    {
        curLevel = 0;
        targetLevel = addLevel;
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
        remain = get_cur_remain_sp(
            (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                ->GetCurCharacSkillTreeIndex());
    }
    else
    {
        if (((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == -1 ||
            ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == 0)
        {
            remain = get_cur_remain_sp((ENUM_SKILL_TREE_KIND)2);
        }
        else
        {
            remain = get_cur_remain_sp((ENUM_SKILL_TREE_KIND)3);
        }
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
        if (check_skill_mastered(preSkill.m_index, preSkill.m_jobType,
                                 preSkill.m_group) != 1)
        {
            return -3;
        }
        it++;
    }
    if (slot == -1)
    {
        *remain = *remain - needSP;
        slot = insert_skill(
            skillIdx, group, skill->is_active_skill(),
            (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                ->GetCurCharacSkillTreeIndex(),
            addLevel);
        if (m_parent != 0)
        {
            (&m_parent->m_historyLog)
                ->SkillAdd(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, addLevel,
                           (eSkillAddReason)0);
            (&m_parent->m_historyLog)
                ->SPSub(
                    (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                        ->GetCurCharacSkillTreeIndex(),
                    m_remainSP1, needSP, (eSPSubReason)0);
        }
    }
    else
    {
        buf[slot].m_level = (char)targetLevel;
        *remain = *remain - needSP;
        if (m_parent != 0)
        {
            (&m_parent->m_historyLog)
                ->SkillAdd(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, targetLevel,
                           (eSkillAddReason)0);
            (&m_parent->m_historyLog)
                ->SPSub(
                    (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                        ->GetCurCharacSkillTreeIndex(),
                    *remain, needSP, (eSPSubReason)0);
        }
    }
    return slot;
