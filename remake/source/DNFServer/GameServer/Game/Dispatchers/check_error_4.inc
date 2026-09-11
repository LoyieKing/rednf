int Dispatcher_StartGame::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x813f7de：直接以 user 为操作对象（asm 无 target 变量）。
    (void)msg;
    if (!isOpenAdvanceAltar())     // ORIG 0x0812cfea
    {
        return 1;
    }
    if (user->get_state() != 3)
    {
        return 3;
    }
    if (user->get_charac_level() <= 0x31)   // ORIG cmp $0x31; setle
    {
        return 4;
    }
    if (user->isCompetitionMercenary())
    {
        return 3;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 3;
    }
    if (user->CheckInTrade())
    {
        return 2;
    }
    if (user->IsBusyDisjointStore())
    {
        return 3;
    }
    if (user->CheckInParty())
    {
        return 3;
    }
    if (G_GameWorld()->IsPVPChannel())
    {
        return 3;
    }
    return 0;
}