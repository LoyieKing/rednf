bool CPremiumLetheManager::InitLetheSkill(CUser* user,
                                          ENUM_SKILL_TREE_KIND kind)
{
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        return false;
    }
    _update(user, INIT_LETHE_SKILL, kind);
    cMyTrace(__PRETTY_FUNCTION__,
             0x75, 0)("ONE_DAY_LETHE : SUCCESS_INIT_FOR_1ST_GROW_CHAR, char(%s)",
                      user->getCurCharacName());
    return true;
}