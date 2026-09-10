bool UserQuest::get_mail_quest_info(std::list<int>& questList,
                                    stSelectQuestParam const& selectParam) const
{
    return G_CDataManager()->m_questList->select_mail_quest(questList, m_cleared, selectParam);
