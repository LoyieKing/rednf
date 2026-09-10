UserQuest* CUser::getCurCharacQuestRefR() const
{
    return const_cast<UserQuest*>(&m_quest);
