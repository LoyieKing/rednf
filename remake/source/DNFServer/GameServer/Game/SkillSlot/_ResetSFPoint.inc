void CSkillChanger::_ResetSFPoint(CUser* user, ENUM_SKILL_TREE_KIND kind)
{
    if (user->get_charac_level() <= 0x31)
    {
        return;
    }
    if (kind <= 1)
    {
        return;
    }
    // 2026-09-10 试形留痕（A 轴）：去 `const` ⇒ InitSkill insns 340 与 ndiff 代理
    //   466 均不变（初值非常量、O0 无传播）⇒ 已回滚，维持原 `const`。
    register const int newPoint = user->get_charac_level() - 0x31;
    ((SkillSlot*)user->getCurCharacSkillW())
        ->set_remain_sfp_at_index(newPoint, kind);
