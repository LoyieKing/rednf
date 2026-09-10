int CSkill::get_need_sp_sfp_for_level(int level, int targetLevel) const
{
    if (IsSpecialSkill())
    {
        return get_need_sfp_for_level(level, targetLevel);
    }
    return get_need_sp_for_level(level, targetLevel);
