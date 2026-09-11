int Dispatcher_Equipslot_Switch::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)   // ORIG 判 user@+0xc；我们误判 msg ref@+0x10
    {
        return LineFunc(0x3e1c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() <= 2)
    {
        return LineFunc(0x3e1d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}