void CEventScriptMng::process_level_up_reward_recv_level(
    CUser* user, int level, int growType)
{
    if (user == 0)
        return;
    if (!is_eventing(0x194))
        return;

    CLevelUpReward* entity =
        static_cast<CLevelUpReward*>(get_event_entity(0x194));
    if (entity != 0)
        entity->send_reward_from_recv_level(user, level, growType);
