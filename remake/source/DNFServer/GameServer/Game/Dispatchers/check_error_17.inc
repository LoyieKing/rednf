int Dispatcher_EvadeAssault::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081ce582：帧 0x18（无 pretty 指针槽——具名 pretty 局部膨胀帧并
    // 产 -0xc 存取，技巧36）；state 判定走 bool 物化 `cmp $3;setne;test;je
    // mov$0 尾岛`（技巧9/19），LineFunc 真体 fall-through 在前（技巧17）。
    if (user == 0)
    {
        return LineFunc(0x27c2, __PRETTY_FUNCTION__, 0, 0);
    }
    if ((bool)(user->get_state() != 3))
    {
        return LineFunc(0x27c5, __PRETTY_FUNCTION__, 0, user->get_acc_id());
    }
    return 0;
}