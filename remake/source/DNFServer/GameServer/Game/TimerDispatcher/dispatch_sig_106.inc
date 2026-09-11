bool TimerSetPunishUser::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    // ORIG 0x08638ba0：独立早退（jne 岛）；比较对象 p2（0x10(%ebp)）；
    // get_unique_id unsigned short ⇒ movzwl %ax 直比，无 &0xffff。
    if (user == 0)
        return 0;
    if (user->get_unique_id() != p2)
        return 0;
    user->setHackUserFlag();
    user->SetTradePunishType(0x40);   // ORIG T _ZN5CUser17SetTradePunishTypeEi
    if (user->isARSUserKick())
    {
        user->DisConnSig(static_cast<DISCONN_SIG>(0x2e), 1, 0);   // ORIG 0x8638ba0
    }
    return 1;
}