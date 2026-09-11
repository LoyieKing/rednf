bool UserQuest::check_quest_condition(int questIdx)
{
    Quest* quest = G_CDataManager()->m_questList->find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "user_quest.cpp", __PRETTY_FUNCTION__, 0x403,
            "G_CDataManager()->m_pQuestList->find_quest(%d) fail", questIdx);
        return 0;
    }
    return check_quest_type_condition(quest);
}