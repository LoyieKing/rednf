int TimerCheckPaidPCRoomTerm::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
    // ORIG 报告 TimerCheckPaidPCRoomTerm/dispatch_sig.md：两段独立早退；
    // !isAffectedPremium()（bool 声明 ⇒ xor $0x1）先走 notify/return 0，
    // logout/login/return 1 位于尾部 je 目标块。
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(p2)))
    {
        WongWork::CHandlePremium::handleNotifyPremiumInfo(user, p2, 0);
        return 0;
    }
    user->SendLogoutToPCRoomServer();
    user->SendLoginToPCRoomServer();
    return 1;
