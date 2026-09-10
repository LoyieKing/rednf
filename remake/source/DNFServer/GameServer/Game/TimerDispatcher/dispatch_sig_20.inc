int TimerPremiumServiceStart::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->GetPremiumInfoW()->startPremium(p2, false))
    {
        return 0;
    }
    if (user->get_state() > 2)
    {
        // ORIG 通过 CUser::GetPremiumInfo()（const）取 CUserPremium*，再查指定类型信息。
        WongWork::SUserPremiumInfo* info =
            user->GetPremiumInfo()->GetPremiumInfo(p2);
        // ORIG 0x08634af4/0x08634b3b：两处正向早退 return 1（mov $1 岛内联）。
        if (info->m_type == 0)
            return 1;
        WongWork::CHandlePremium::handleNotifyPremiumInfo(
            user, info->m_type, info->m_end - info->m_start);
        if (g_SPremiumInfo[info->m_type].m_kind == 2)
            return 1;
        WongWork::CHandlePremium::handlePremiumEffect(
            user, static_cast<ENUM_PREMIUM_TYPE>(info->m_type));
    }
    return 1;
