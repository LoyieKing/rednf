int Dispatcher_UseDye::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81e727e：仅允许 state==3（setne 条件形态，无 user 判空）；
    // CheckInTrade 真 → 1；假分支含 msg 死槽 -0xc（objdump 0x081e72b6 区）。
    if (user->get_state() != 3)
        return 0x13;
    if (user->CheckInTrade())
        return 1;
    MSG_BASE* pMsg = &msg;    // ORIG dead store -0xc(%ebp)
    (void)pMsg;
    return 0;
}