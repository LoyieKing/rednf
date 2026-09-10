void CUser::SendItemMakingSkillInfo()
{
    CItemMakingSkill* skill =
        reinterpret_cast<CItemMakingSkill*>(GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)10));
    if (skill != 0) {
        skill->Send_ItemMakingSkillList(this);
    }
    return;
