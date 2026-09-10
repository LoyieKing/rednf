int Dispatcher_CeraPackageOpen::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    // ORIG 0x081e7cd3：cmp $3 + setne 物化（技巧9）。
    if ((bool)(user->get_state() != 3))
    {
        return 0x13;
    }
    // ORIG 0x081e7cef：bool callee 直判 test %al（无物化）。
    if (user->CheckInTrade())
    {
        return 0x13;
    }
    // ORIG 0x081e7d05：结果落具名槽 -0xc + cmpl $0（技巧8 槽版）。
    const _Charac_info* characR = user->getCurCharacR();
    if (characR == 0)
    {
        return 0x13;
    }
    return 0;
