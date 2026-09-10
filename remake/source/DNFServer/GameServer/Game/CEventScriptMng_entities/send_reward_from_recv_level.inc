int CLevelUpReward::send_reward_from_recv_level(CUser* user, int level,
                                                int growType)
{
    int job = user->get_charac_job();
    int curGrowType = static_cast<int>(user->getCurCharFirstGrowType());

    std::map<int, Condition_LevelUp>::iterator it =
        static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.find(job);
    if (it != static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.end() &&
        !(it->second.m_filter1 != 0xff &&
          static_cast<int>(static_cast<signed char>(it->second.m_filter1)) !=
              curGrowType))
    {
        for (int i = 0; i < (int)it->second.m_items.size(); ++i)
        {
            if (growType < it->second.m_items[i].m_level &&
                it->second.m_items[i].m_level <= level)
            {
                static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.reset();
                static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.set_item(
                    it->second.m_items[i].m_itemIndex,
                    it->second.m_items[i].m_itemCount);
                static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field30 =
                    it->second.m_items[i].m_title;
                static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field34 =
                    it->second.m_items[i].m_content;
                do_action(user);
            }
        }
    }
    return 1;
