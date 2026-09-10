bool COnTimeEvent::process_send_reward_to_user()
{
    std::map<unsigned int, RewardUserData>::const_iterator it;
    CUser* user = 0;
    int processed = 0;

    while (!m_rewardUsers.m_index.empty())
    {
        it = m_rewardUsers.m_map.find(m_rewardUsers.m_index.front());

        if (it != m_rewardUsers.m_map.end())
        {
            if (it->second.mark != 1)
            {
                if ((user = G_GameWorld()->find_user_from_world_byaccid(
                         it->first)) != 0)
                {
                    if (user->get_charac_no(-1) == it->second.a &&
                        user->get_state() > 2)
                    {
                        user->InsertOnTimeEventReward(
                            get_ontime_reward_item_no(),
                            get_ontime_reward_item_count());
                        DB_InsertOnTimeRewardUserLog::makeRequest(
                            (int)user->GetUID(), it->first, get_ontime_id());
                    }
                }
            }
        }
        ++processed;
        m_rewardUsers.m_index.pop_front();
        if (processed > 0x32)
            break;
    }
    return true;
