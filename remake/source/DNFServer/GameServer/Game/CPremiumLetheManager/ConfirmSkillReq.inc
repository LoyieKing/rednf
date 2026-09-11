bool CPremiumLetheManager::ConfirmSkillReq(CUser* user)
{
    if (user == 0)
    {
        return false;
    }
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        return false;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x35, 0)(
        "ONE_DAY_LETHE : CONFIRM_SKILL, char(%s)", user->getCurCharacName());
    _update(user, SKILL_CONFIRM_STATE,
            (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex());
    return true;
}