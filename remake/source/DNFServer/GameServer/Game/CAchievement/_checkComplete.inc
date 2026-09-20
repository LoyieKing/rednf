void CAchievement::_checkComplete(unsigned int id)
{
    if (!isClearedAchievement(id))
    {
        return;
    }
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if (quest == 0)
    {
        return;
    }
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)0;
    int slot = 0;
    int titleIdx = quest->get_title_reward();
    if (titleIdx != 0)
    {
        CTitleBook::putTitleOnly(m_user, titleIdx, category, slot);
    }
    _sendEachAchieveReward(quest);
    _sendCompleteReward(quest, category, slot);
    _sendComplete(id, titleIdx, category, slot);
    // CUser 冻结头 +0x79700 即 cUserHistoryLog（本地最小视图；本 TU 混 CTitleBook
    // 冲突类不能并入 CUser.h，保持具名视图 + 文档化基址访问）
    m_user->m_historyLog.AchievementComplete((int)id);
}