bool checkAcceptableQuest(const Quest* quest,
                          const WongWork::CQuestClear& cleared,
                          const stSelectQuestParam& param)
{
    if (quest->m_index == 0x3f8) return true;
    if (!quest->exposeQuest()) return false;
    if (!quest->check_possible(param)) return false;
    if (!quest->isRepeatableQuest())
    {
        if (!G_CDataManager()->m_questList->check_clear(
                quest->m_index, cleared))
        {
            return false;
        }
    }
    if (!G_CDataManager()->m_questList->check_ahead_quest(
            quest->m_index, cleared))
    {
        return false;
    }
    if (!G_CDataManager()->m_questList->check_anti_quest(
            quest->m_index, cleared))
    {
        return false;
    }
    return true;
