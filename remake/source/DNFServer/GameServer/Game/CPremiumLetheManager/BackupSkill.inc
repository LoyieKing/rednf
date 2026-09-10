bool CPremiumLetheManager::BackupSkill(CUser* user,
                                       ENUM_SKILL_TREE_KIND kind)
{
    if (user->getCurCharacR() == 0)
    {
        return false;
    }
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        if (user->GetOneDayLetheFlag() != NOT_USE_LETHE_STATE &&
            user->GetOneDayLetheFlag() != SKILL_CONFIRM_STATE)
        {
            return false;
        }
    }
    else
    {
        if (user->GetOneDayLetheFlag2ND() != NOT_USE_LETHE_STATE &&
            user->GetOneDayLetheFlag2ND() != SKILL_CONFIRM_STATE)
        {
            return false;
        }
    }
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        user->SetOneDayLetheFlag(USE_LETHE_STATE);
        _update(user, USE_LETHE_STATE, SKILL_TREE_1);
    }
    else
    {
        user->SetOneDayLetheFlag2ND(USE_LETHE_STATE);
        _update(user, USE_LETHE_STATE, SKILL_TREE_2);
    }
    return true;
