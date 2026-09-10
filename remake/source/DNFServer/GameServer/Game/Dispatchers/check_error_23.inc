int Dispatcher_ChangeEmotion::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return LineFunc(0x2e77,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x2e7a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
