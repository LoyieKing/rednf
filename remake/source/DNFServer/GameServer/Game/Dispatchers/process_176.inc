int Dispatcher_Request_Seria_Buff::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG：err@-0x14（先声明远槽）；msg→-0x10、param→-0xc 两 dead 绑定
    // 在 set_growth 之前才物化（技巧13 声明位=落槽时机）。
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    MSG_BASE* pm = &msg;
    ParamBase* pp = &param;
    (void)pm;
    (void)pp;
    user->set_growth_power_exp_reward_right(false);
    return 0;
