int UserQuest::item_reward_quest(int mapIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0)
        {
            if (m_trigger[i] == 0)
            {
                continue;
            }
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest == 0)
            {
                return 0;
            }
            if (quest->check_clear_item(mapIdx, (int)m_authenData.m_characClass))
            {
                return m_quest[i];
            }
        }
    }
    return 0;
