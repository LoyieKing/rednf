void CHandlePremium::handlePremiumTimeout(CUser* user)
{
    // [b28d-56 判词·N] N82：ORIG 0x086afa7c 118 条 vs 我们 113 条（ORIG `push %ebx;
    // sub $0x44` vs 我们 `sub $0x48`，SUserPremiumInfo 返回槽 -0x1c vs -0x20）；语义完成。
    SUserPremiumInfo info = user->GetPremiumInfo()->CheckPremiumTimeout();
    int timeoutType = info.m_type;
    if (timeoutType == 0xc)
    {
        user->GetPremiumInfoW()->setPremiumState((ENUM_PREMIUM_TYPE)0xc,
                                                 (ENUM_PREMIUM_STATE)3);
        handleNotifyPremiumInfo(user, 0xc, 0);
        return;
    }
    if (timeoutType == 0)
        return;

    handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)timeoutType, 0, 0, 0, true);
    // ORIG 0x086afb4c：mov -0x10(%ebp) = info.m_flag(+0xc)（非 m_type）。
    user->RemoveRestrictedGoods(info.m_flag);
    if (user->getCurCharacR() != 0)
    {
        user->ProcPremiumFatigue();
        user->SendFatigue();
        if (g_SPremiumInfo[timeoutType].m_kind == 1)
        {
            if (g_SPremiumInfo[timeoutType].m_field64 > 0)
                user->SetNeedCheckOverEquipItem();
            user->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
        }
        if (timeoutType == 0x53 || timeoutType == 0x57)
        {
            user->ReCalcChattingEmoticon();
            user->SendChattingEmoticon();
        }
        else if (timeoutType == 0x21)
        {
            user->SetOneDayLetheFlag(
                CPremiumLetheManager::NOT_USE_LETHE_STATE);
            user->SetIsEndLetheAtLogin();
        }
    }
    handleNotifyPremiumInfo(user, timeoutType, 0);
