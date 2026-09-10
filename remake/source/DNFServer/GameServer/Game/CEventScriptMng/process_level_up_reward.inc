void CEventScriptMng::process_level_up_reward(CUser* user, bool flag)
{
    if (user == 0)
        return;
    if (!is_eventing(0x194))
        return;

    CLevelUpReward* reward =
        static_cast<CLevelUpReward*>(get_event_entity(0x194));
    if (reward != 0)
    {
        if (reward->is_recv_check_item() != 0)
        {
            if (reward->check_recv_level(user->get_charac_level(),
                                         user->get_charac_job(),
                                         user->getCurCharFirstGrowType(),
                                         flag) != 0)
                DB_SelectLevelUpReward::makeRequest(
                    static_cast<int>(user->GetUID()),
                    user->getCurCharacNo(),
                    user->get_charac_level());
        }
        else if (reward->set_reward_item(user->get_charac_level(),
                                         user->get_charac_job(),
                                         user->getCurCharFirstGrowType()) != 0)
        {
            reward->do_action(user);
        }
    }
