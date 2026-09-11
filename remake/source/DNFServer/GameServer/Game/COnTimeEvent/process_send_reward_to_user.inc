// ORIG @0x081bbd46：遍历 key 队列（上限 0x33 个/次），逐个投递奖励。
// 汇编要点：① while (!m_index.empty()) 前置 jmp 循环头（P25 while 形态）；
// ② 循环尾 ++processed 先、pop_front 后、cmpl $0x32; jg break；
// ③ iterator 函数头默认构造（P28 声明序 it→user→processed）；
// ④ 条件嵌套 if（两个独立 je，非 && 汇聚）；⑤ 比较用 it->second.a（charac_no）。
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
}