int Dispatcher_ObjectBringUp::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)                             // cmp $2,setle（P2）
    {
        return -2;
    }
    return 0;
