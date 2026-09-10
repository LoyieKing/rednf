int Dispatcher_GrowthCreatureChangeInfinityCreature::process(CUser* user,
                                                             MSG_BASE& msg,
                                                             ParamBase& param)
{
    int err = check_error(user, msg);
    static_cast<stGrowthCreature&>(param).m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x64e2,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    static_cast<stGrowthCreature&>(param).m_4 = process_event(user);
    return 0;
