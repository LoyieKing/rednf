unsigned int Dispatcher_SecretShopOpenClose::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() != 5)
    {
        return (int)0x7fffffff;
    }
    return 0;
