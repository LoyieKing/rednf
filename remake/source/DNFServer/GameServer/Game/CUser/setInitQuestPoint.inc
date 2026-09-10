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

    // 2026-09-10 gA（A 组）：ORIG 0x0868ae31 无判空（`mov %eax,-0x10(%ebp)` 后直接
    //   lea -0x14/call multimap::end）⇒ 删我方发明的 `if (npcList==0) return;`（判据库 1）。
    // ORIG 0x0868ae34 在循环外只调一次 end()（结果落 -0x14 槽，循环体内 0x0868ae63..
    //   0x0868af1f 无 end() call，`jmp cond` + 尾部 `call operator!=`）⇒ 具名缓存 end（P13）。
    const std::multimap<int, int>::const_iterator itEnd = npcList->end();
    for (std::multimap<int, int>::const_iterator it = npcList->begin();
         it != itEnd; ++it)
    {
        Quest* quest = G_CDataManager()->find_quest(it->second);
        if (quest != 0)
        {
            // ORIG 0x0868ae8b..0x0868aeb5：先 `call operator->; mov 0x4(%eax),%ebx`
            //   取 it->second，再 `call getCurCharacQuestR; add $0x4,%eax` 直内联 receiver
            //   （无具名 cleared 指针槽）⇒ 删具名 cleared（P183 地址重物化铁律）。
            if (((WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4))
                    ->isClearedQuest((unsigned int)it->second))
            {
                incInitQuestPoint(quest->m_field138);
                incQuestPiece(quest->m_field13a, quest->m_index, true);
            }
        }
    }
