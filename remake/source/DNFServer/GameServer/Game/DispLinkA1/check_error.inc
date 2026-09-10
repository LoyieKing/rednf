int Dispatcher_Client_Spec_Statistic::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0) {  // ORIG 判 user@+0xc（0x081ce760 cmp param_1,0）；我们误判 msg ref@+0x10
        (void)msg;
        return LineFunc(
            0x27fd,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
