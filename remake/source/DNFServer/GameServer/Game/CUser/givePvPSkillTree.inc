void CUser::givePvPSkillTree(int a, bool b, int c)
{
    // ORIG T 0x8665400 全录（报告双段读完）：mode 判断为 **param_3==1/2/3 三分支
    // 复制**（SkillR/W、FindSkill、growtype_skill 各 4 次调用，勿 hoist 指针/合并
    // 条件——旧 `c==1||c==3` 共享形 call 计数 17 vs ORIG 46）；内层 **iterator
    // 扫描**（begin/end/++/pt，每字段读独立 pt 调用共 16）；b 真才 FindSkill
    //（短路 `b && ...`），条件 `FindSkill < second`（FindSkill 先、second 后读）；
    // growtype 实参求值序 second→first→job→SkillW（Ghidra uVar1/uVar2 序）。
    for (int secondGrow = 0;
         secondGrow <= (int)(signed char)getCurCharSecondGrowType(); ++secondGrow)
    {
        for (int grade = a; grade <= get_pvp_grade(); ++grade)
        {
            std::vector<std::pair<int, int> >* tree = 0;
            // b28d-solo-CUser 试形：正条件 if 包裹（P214①/P59）——ORIG `call; test
            //   %al,%al; je <循环尾>`（无 xor 段）⇒ 原码是 `if (fn(...)) { body }`，
            //   非 `==0 continue`（后者 4.4 产 xor$1;test;jne 反向形）。
            if (G_CDataManager()->m_pvpSkillTree.getPvPSkillTree(
                    get_charac_job(), (int)(signed char)getCurCharFirstGrowType(),
                    secondGrow, grade, b, &tree) != 0)
            {
            for (std::vector<std::pair<int, int> >::iterator it = tree->begin();
                 it != tree->end(); ++it)
            {
                if (c == 1)
                {
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)0) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)0);
                    }
                }
                else if (c == 2)
                {
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)1) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                else if (c == 3)
                {
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)0) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)0);
                    }
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)1) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)1);
                    }
                }
            }
            }
        }
    }
