bool CPremiumLetheManager::BackToPre(CUser* user, _Mastered_skill* skills,
                                     ENUM_SKILL_TREE_KIND kind)
{
    if (user->getCurCharacR() == 0)
    {
        return false;
    }
    InitSkill(user, false, kind);
    if (!user->master_new_skills(skills, kind))
    {
        return false;
    }
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        user->SetOneDayLetheFlag(NOT_USE_LETHE_STATE);
    }
    else
    {
        user->SetOneDayLetheFlag2ND(NOT_USE_LETHE_STATE);
    }
    cMyTrace(__PRETTY_FUNCTION__,
             0x4f, 0)("ONE_DAY_LETHE : SUCCESS_BACK_TO_PRE, char(%s)",
                      user->getCurCharacName());
    return true;
}