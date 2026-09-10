void COnTimeEvent::set_reward_end_time(int span)
{
    time_t t = time(0);
    m_rewardEndTime = t + span;
