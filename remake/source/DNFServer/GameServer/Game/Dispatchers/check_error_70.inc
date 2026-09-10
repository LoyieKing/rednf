int Dispatcher_Buy_Charac_Status_Using_QP::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    // ORIG：cmp $3 + setne 物化（技巧9），!=3 → 99；尾 mov$0 岛。
    if ((bool)(user->get_state() != 3))
    {
        return 99;
    }
    return 0;
