void CAchievement::_gm_clearAll()
{
    // ORIG 0x828d73f：stAchievement 临时对象循环外构造（get_init 直接写其 trigger 字段）
    stAchievement ach;
    for (int idx = 0x1965; (int)idx <= 0x1bbb; ++idx)
    {
        // ORIG 0x828d756：QuestList::find_quest 死调用（结果丢弃）
        G_CDataManager()->m_questList->find_quest(idx);
        Quest* quest = G_CDataManager()->find_quest((int)idx);
        if ((quest == 0) || (quest->m_type != 7))
        {
            continue;
        }
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        setTrigger(idx, ach.m_trigger0, ach.m_trigger1, ach.m_trigger2);
    }
