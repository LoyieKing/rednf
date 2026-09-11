bool TimerScheduleFiveMinPerUser::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 3);   // ORIG 第二实参为常量 3
    // ORIG 0x086341d2：三段独立早退；第三段 && 经 (bool) 物化（+0x57..+0x7b 岛形）。
    if (user == 0)
        return 0;
    if (user->GetLoginTick() != p2)
        return 0;
    if ((bool)(p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3))
        return 0;
    user->WorkPerFiveMin();
    // ORIG 0x0863425f..：右到左求值，RCR 依次落 %edi(no)/%esi(tick)/%ebx(uid)
    // （4.4 自由分配先选 %ebx，asm 绑名复现）。
    register unsigned int characNo = user->getCurCharacNo();
    register unsigned int tick = user->GetLoginTick();
    register int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x5f, 300, tick, characNo);
    return 1;
}