unsigned int Dispatcher_DeleteToBlackList::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081cdee0（报告 Dispatcher_DeleteToBlackList/check_error.md 全文已读）：
    // 同 Dispatcher_RequestBlackList::check_error 形态：无判空，bool 物化 cmp $2/
    // setle/test；state<=2 → -1，否则 0。
    (void)msg;
    if ((bool)(user->get_state() <= 2))
        return -1;
    return 0;
