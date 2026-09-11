int Dispatcher_SaveGameOption_1::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081ced4e（报告 Dispatcher_SaveGameOption_1/check_error.md 全文已读）：
    // 判空走 __PRETTY_FUNCTION__ 直传（无局部 pretty 槽）；state<=1 经 bool 物化。
    (void)msg;
    if (user == 0)
    {
        return LineFunc(0x28ad, __PRETTY_FUNCTION__, 0, 0);
    }
    if ((bool)(user->get_state() <= 1))
    {
        return -1;
    }
    return 0;
}