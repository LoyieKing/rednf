void CUser::gain_sfp(int v)
{
    // ORIG 0x866aad2 守卫是正向 `if (v != 0) {…}`（同已 identical 的 gain_sp 形）：
    // 写 `if (v == 0) return;` 早返形会让 4.4 在末支 else 的 call 后多发一条
    // `jmp END`（早返块的 exit 边强制跳转），正向块形则自然 fall-through。
    // 两段展开：cap=INT_MAX-v 每段先算、else 支重呼 SkillR+get_remain_sfp、无缓存变量。
    if (v != 0)
    {
        if (0x7fffffff - v <
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2))
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)2);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(
                    ((SkillSlot*)getCurCharacSkillR())
                        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2) + v,
                    (ENUM_SKILL_TREE_KIND)2);
        }
        if (0x7fffffff - v <
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3))
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)3);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(
                    ((SkillSlot*)getCurCharacSkillR())
                        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3) + v,
                    (ENUM_SKILL_TREE_KIND)3);
        }
    }
