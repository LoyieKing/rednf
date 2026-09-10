UserQuest* CUser::getCurCharacQuestW()
{
    enableSaveQuest();
    return (UserQuest*)&m_quest;
