void COnTimeEvent::process_set_reward_start(int ontimeId, int itemNo,
                                            int itemCount, int endSpan)
{
    set_reward_state(0);
    set_ontime_id(ontimeId);
    set_ontime_reward_item(itemNo, itemCount);
    set_reward_end_time(endSpan);
    m_rewardUsers.clear();
