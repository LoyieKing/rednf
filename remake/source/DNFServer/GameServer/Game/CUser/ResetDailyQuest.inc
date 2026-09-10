void CUser::ResetDailyQuest()
{
    std::list<int> questList;
    // ORIG 0x865753x：G_CDataManager()->m_questList 结果不留槽（技巧 37），
    // 直接 `mov %eax,(%esp)` 传 GetDailyQuestList——具名 pQuestList 反产 -0x10 spill
    // 使整帧平移 4（ORIG it=-0x18/end=-0x1c 证据）。
    G_CDataManager()->m_questList->GetDailyQuestList(questList);
    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        // ORIG 0x08657574：getCurCharacQuestRefR 链式调用 isClearQuest（无中间变量、不判空）
        if (getCurCharacQuestRefR()->isClearQuest(questId))
        {
            getCurCharacQuestW()->resetClearQuest(questId);
        }
    }
