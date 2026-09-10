int Dispatcher_Direct_Entrance_Quick_Party::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() <= 3 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    return 0;
