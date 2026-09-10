unsigned int Dispatcher_RequestBlackList::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081ce046（报告 Dispatcher_RequestBlackList/check_error.md 全文已读）：
    // 无 user==0 判空；state<=2 条件经 bool 物化（cmp $2/setle %al/test %al,%al），
    // 真 → -1（真分支先置），假 → 0。
    (void)msg;
    if ((bool)(user->get_state() <= 2))
        return -1;
    return 0;
