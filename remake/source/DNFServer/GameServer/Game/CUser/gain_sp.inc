void CUser::gain_sp(int v)
{
    // ORIG 0x866a3fe（80 insn 无回跳 = **源码两段手动展开**；remain 无变量：
    // else 支重呼 SkillR+get_remain；cap=INT_MAX-v 每段表达式内联先算（4.1 落
    // %ebx 三行），cmp 结果 setl 物化岛（技巧 22 register bool al 方向）。
    if (0x7fffffff - v <
        ((SkillSlot*)getCurCharacSkillR())
            ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0))
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)0);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0) + v,
                (ENUM_SKILL_TREE_KIND)0);
    }
    if (0x7fffffff - v <
        ((SkillSlot*)getCurCharacSkillR())
            ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1))
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)1);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1) + v,
                (ENUM_SKILL_TREE_KIND)1);
    }
