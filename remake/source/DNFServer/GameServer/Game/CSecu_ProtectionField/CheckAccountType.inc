int CSecu_ProtectionField::CheckAccountType(CUser* user, unsigned long protType)
{
    // ORIG T 0x08288ac8（报告 CheckAccountType.md）：不缓存 card/pad 指针，
    // 每次属性查询均重新取对象。
    if (user == 0)
        return 1;

    if (user->getSecurityCard()->isActivate())
    {
        if ((unsigned int)user->getSecurityCard()->getFailCnt() > 2)
            return 0x7a;
        if (!user->getSecurityCard()->isCertified())
        {
            user->sendBackSecurityAuthRequest();
            return 0x7b;
        }
    }
    else
    {
        if (user->getPad()->isActivate())
        {
            if (user->getPad()->getFailCnt() > 2)
                return 0x88;
            if (!user->getPad()->isCertified())
            {
                user->sendBackSecurityAuthRequest();
                return 0x89;
            }
        }
    }

    if ((protType & 0x10) != 0 && user->isTradePunishType(0x10))
        return 0x72;
    return 0;
