// ORIG 0x081e86a6（报告 Dispatcher_GrowthCreatureChangeInfinityCreature/check_error.md）
int Dispatcher_GrowthCreatureChangeInfinityCreature::check_error(CUser* user,
                                                                 MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    int err = check_error_event(user);
    if (err != 0)
    {
        return err;
    }
    return 0;
}