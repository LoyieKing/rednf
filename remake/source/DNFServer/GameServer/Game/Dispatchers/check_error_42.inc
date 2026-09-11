int Dispatcher_SaveGameOption_2::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081cf050（报告 Dispatcher_SaveGameOption_2/check_error.md 全文已读）：
    // 同 SaveGameOption_1 形态：__PRETTY_FUNCTION__ 直传；state<=1 bool 物化 → -1。
    (void)msg;
    if (user == 0)
    {
        return LineFunc(0x28fa, __PRETTY_FUNCTION__, 0, 0);
    }
    if ((bool)(user->get_state() <= 1))
    {
        return -1;
    }
    return 0;
}