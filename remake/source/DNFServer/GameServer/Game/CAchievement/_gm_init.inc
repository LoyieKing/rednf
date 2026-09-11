void CAchievement::_gm_init()
{
    for (std::map<unsigned int, stAchievement>::iterator it =
             m_achievements.begin();
         it != m_achievements.end(); ++it)
    {
        Quest* quest = G_CDataManager()->find_quest((int)it->second.m_id);
        if (quest == 0)
        {
            break;
        }
        if (!quest->get_init_achievement_trigger(it->second.m_trigger0,
                                                 it->second.m_trigger1,
                                                 it->second.m_trigger2))
        {
            break;
        }
        _sendSetTrigger(it->second.m_id, 0);
    }
    alter();
}