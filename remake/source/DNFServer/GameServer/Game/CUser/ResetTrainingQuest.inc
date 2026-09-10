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
        // 2026-09-10（G3CUser-B，P171 删发明防御）：ORIG 0x086576cb
        //   `call getCurCharacQuestRefR` 的返回值 %eax **直传** isClearQuest 的 this
        //   （0x086576d0 `mov -0xc(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp);
        //   call isClearQuest`），全程**无** `pQuest` 具名槽、**无** `test %eax,%eax /
        //   je` 判空，也无 `&&` 的 setcc 物化段 ⇒ 删除我方具名 pQuest 与判空。
        if (getCurCharacQuestRefR()->isClearQuest(questId))
        {
            getCurCharacQuestW()->resetClearQuest(questId);
        }
    }
