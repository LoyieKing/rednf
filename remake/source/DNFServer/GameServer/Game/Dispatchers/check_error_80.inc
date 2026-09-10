int Dispatcher_BlueMarbleConfirmInfo::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081e68c8：函数首即物化 msg 指针@-0xc（后续未读回，死绑定；P31）。
    const MSG_BASE* __orig_bind = &msg;
    (void)__orig_bind;
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    return 0;
