int TimerPremiumServiceUpdate::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
    // ORIG 0x086348d4：三段独立早退。
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->IsPremiumUser())
        return 0;
    user->GetPremiumInfoW()->ReCalcAdvantage();
    user->GetPremiumInfoW()->RecalcAdditionalInfo(user);
    if (user->get_state() > 3)
    {
        user->SendFatigue();
    }
    const time_t now = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm midnight;
    localtime_r(&now, &midnight);   // ORIG 0x086349ae：localtime_r（线程安全）
    midnight.tm_hour = 0;
    midnight.tm_min = 0;
    midnight.tm_sec = 0;
    int nextAt = static_cast<int>(mktime(&midnight));   // ORIG mov %eax,-0xc(%ebp)
    if (midnight.tm_hour >= 0)                          // ORIG mov -0x38(%ebp) test js
        nextAt += 0x15180;
    register int delay = nextAt - static_cast<int>(now);   // ORIG %ebx 直落 0x10(%esp)
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, p1, (TIMER_MESSAGE)5,
                                delay, p2, p3);
    return 1;
