int Dispatcher_GoldTakeIncreasingAmount::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081d5656（报告 Dispatcher_GoldTakeIncreasingAmount/check_error.md 全文已读）：
    // 判空 → LineFunc(0x3903, __PRETTY_FUNCTION__, 0, 0)（无局部 pretty 槽）；
    // state!=5 经 bool 物化（setne/test）→ -1；==5 → 0。
    (void)msg;
    if (user == 0)
    {
        return LineFunc(0x3903, __PRETTY_FUNCTION__, 0, 0);
    }
    if ((bool)(user->get_state() != 5))
    {
        return -1;
    }
    return 0;
}