bool CPremiumLetheManager::UpdateBackupSkillFlag(CUser* user,
                                                 ENUM_SKILL_TREE_KIND kind)
{
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        if (user->GetOneDayLetheFlag() != NOT_USE_LETHE_STATE)
        {
            return false;
        }
    }
    else
    {
        if (user->GetOneDayLetheFlag2ND() != NOT_USE_LETHE_STATE)
        {
            return false;
        }
    }
    _update(user, NOT_USE_LETHE_STATE, kind);
    cMyTrace(__PRETTY_FUNCTION__,
             0x69, 0)("ONE_DAY_LETHE : SUCCESS_INIT_FOR_LEGACY_LETHE, char(%s)",
                      user->getCurCharacName());
    return true;
}