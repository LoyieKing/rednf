void CUser::ResetTrainingQuest()
{
    std::vector<int> questList;
    // b28d-lane5 复测（P0/P200）：ORIG 0x08657662 段——`call get_charac_level;
    //   movswl %ax,%ebx`（level **无槽**、拓宽直传 callee-saved）与
    //   `call G_CDataManager; mov 0x18(%eax),%eax; mov %eax,(%esp)`（m_questList
    //   **无具名指针槽**）⇒ 删 level/pQuestList 两个具名局部，表达式就位内联。
    G_CDataManager()->m_questList->GetTrainingQuestList(questList,
                                                       get_charac_level());
    for (std::vector<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        // b28d-solo-CUser p167 修正：ORIG 0x08657662 调 getCurCharacQuestRefR。
        UserQuest* pQuest = getCurCharacQuestRefR();
        if (pQuest && pQuest->isClearQuest(questId))
        {
            getCurCharacQuestW()->resetClearQuest(questId);
        }
    }
