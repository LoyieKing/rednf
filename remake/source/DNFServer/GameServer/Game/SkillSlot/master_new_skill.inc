// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a,N82）→ 试=prett 缺口实证：ORIG 串
// `int SkillSlot::master_new_skill(int, int, int, int, int, int)`@0x8cde780 在 ORIG ELF
// 全文件仅 1 次字节命中且**非指令引用**（孤立符号；函数体 0 引用）⇒ 非缺口非可落地项；
// nd 多重集 464/458 含 12×`mov -0x34(%ebp),%eax` 重读（4.4 spill 族）+ 槽相位 ⇒ 维持
// （新=DIFF gap=+5, blocks=56）。
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
    // [J10GameRest.J10SkillSlot 2026-09-11] 语义轴**落地**：ORIG 0x08603b97
    // `je 0x08603bfd`（IsStealable==0 直跳 premium 段）证明
    // `level += GetPremiumInfo()->getOverSkillLevel()`（0x08603bfd..0x08603c15
    // `movzwl %ax,%eax; add %eax,0x10(%ebp)`）在 isSteal 分支**外**无条件执行；
    // 旧形误置于 `if (IsStealable(...))` 内 ⇒ 非偷取路径 level 少加 overSkillLevel、
    // can_learn 第 2 实参偏低。移出后我方 0x2e4a(sub) → 0x2e5a/0x2e62(GetPremiumInfo/
    // getOverSkillLevel)+add 的顺序与 ORIG 一致（同址 je 目标 +0x190→+0x175）。
    // 读数 464/458 gap=+6 与残差 28 均不变（语句移动不改指令数），无劣化。
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
        // [V331b02 2026-09-12 修复·死块族] ORIG 0x08603bbb `jmp 0x08603bd8`（无条件）：
        //   紧随的 is_clear_stealingSkillMission 判空早退块（ORIG 0x08603bbd..0x08603bd6）
        //   **无任何跳转入口**（deadblk：9 条不可达）⇒ 运行期不可达；ORIG 活路径
        //   0x08603bd8 `mov $0x0,%eax`（恒 0 ⇒ 永不 return -1）。与
        //   try_master_new_skills（0x0860757e 同形）为同族第 4 实例。
        //   P337 形：恒 false volatile 守卫保留调用点（p167 不变）且运行期不可达。
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
        // [g11-slot 2026-09-10] P0/P183 真形（直证）：ORIG 0x08603c12
        // `movzwl %ax,%eax` + 0x08603c15 `add %eax,0x10(%ebp)` 位于 isSteal
        // 分支内、无具名槽；后续 can_learn 直读形参（0x08603ca0
        // `mov 0x10(%ebp),%eax`）。原具名 `unsigned short overLevel` 是
        // 我方多出的槽（我方 -0x1a store/load 往返，ORIG 全无）。
    }
    // [J10GameRest.J10SkillSlot 2026-09-11] 语义轴修正：ORIG 0x08603b97
    // `je 0x08603bfd`（IsStealable==0 直接跳到 0x08603bfd = GetPremiumInfo 段）
    // 证明本段在 isSteal 分支**外**、无条件执行（0x08603c12 `movzwl %ax,%eax` +
    // 0x08603c15 `add %eax,0x10(%ebp)` 位于 sub GetStealLevelPenalty 之后且
    // 早于 G_GameWorld 调用）。旧形放在 if 内 ⇒ 不偷技能时 level 少加
    // overSkillLevel，can_learn 实参偏低 = 真语义偏差。
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
    // H10回滚 2026-09-11：删具名引用 `preSkillList` 使用点直写 ⇒ ours 458→467
    // （gap 6→−3，劣）+ missing_in_game 36→39（新 emit `_List_iterator::C2`/
    // `_List_const_iterator::C2(iterator const&)` 等 3 个 C2 别名）⇒ 复原。
    // 指令形证据：ORIG 0x08603d?? `mov -0x30(%ebp),%eax; lea 0x18(%eax),%edx`
    //（引用槽 -0xc 在 ORIG 侧同样存在，帧 0x74 差来自别处）。
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
}