int TimerScheduleTenMinPerUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 3);   // ORIG 第二实参为常量 3
    // ORIG 0x08634106（报告 TimerScheduleTenMinPerUser/dispatch_sig.md）：同 Twin 形。
    if (user == 0)
        return 0;
    if (user->GetLoginTick() != p2)
        return 0;
    if ((bool)(p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3))
        return 0;
    user->WorkPerTenMin();
    // 同 FiveMin 孪生（ORIG 0x08634106）。
    register unsigned int characNo = user->getCurCharacNo();
    register unsigned int tick = user->GetLoginTick();
    register int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x60, 600, tick, characNo);
    return 1;
