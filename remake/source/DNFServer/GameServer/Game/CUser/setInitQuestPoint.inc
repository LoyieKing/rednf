void CUser::setInitQuestPoint()
{
    // ORIG 0x868adfc T：清空 init QP 与 piece，再按 NPC 任务表逐条累加已清任务奖励点。
    clearInitQuestPoint();
    clearQuestPiece();

    // b28d-solo-CUser p167：ORIG 0x0868adfc 用 `multimap::begin()/end() const`
    //   （无 iterator→const_iterator 转换）；我方非 const 指针致 +2 转换构造 ⇒
    //   容器指针 const 化（P96 旋钮）。
    const std::multimap<int, int>* npcList =
        G_CDataManager()->m_questList->getQuestNPCList(ENUM_QUEST_GRADE_NONE);
    if (npcList == 0)
        return;

    for (std::multimap<int, int>::const_iterator it = npcList->begin();
         it != npcList->end(); ++it)
    {
        Quest* quest = G_CDataManager()->find_quest(it->second);
        if (quest != 0)
        {
            WongWork::CQuestClear* cleared =
                (WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4);
            if (cleared->isClearedQuest((unsigned int)it->second))
            {
                incInitQuestPoint(quest->m_field138);
                incQuestPiece(quest->m_field13a, quest->m_index, true);
            }
        }
    }
