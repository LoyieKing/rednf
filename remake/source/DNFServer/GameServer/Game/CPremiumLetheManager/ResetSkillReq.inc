bool CPremiumLetheManager::ResetSkillReq(CUser* user)
{
    if (user->getCurCharacR() == 0)
    {
        return false;
    }
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        return false;
    }
    _resetSkill(user);
    return true;
}