// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868adfc | _ZN5CUser17setInitQuestPointEv
// 2026-09-11 CuA（剩余 1 条 `jmp` 的终判）：ORIG 0x868aeff `jmp 868af02` + 0x868af01
//   `nop` 是 4.1 为**空 else/空语句**发射的 nop 岛（if 体尾必须 jmp 跳过）；探针
//   /tmp/P5/p_nop p.cpp f1(`else ;`)/f2 在 4.4 下产物**逐条相同且无 nop/无 jmp**
//   ⇒ 该条 jmp 不可造（N）。本方其余指令已逐条对齐。
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
        // [CUserLane 2026-09-11 T2] P223 尾标签岛：ORIG 0x868ae89 `je 0x868af01`（nop 岛）
        //   + body 尾 0x868aeff `jmp 0x868af02` ⇒ quest==0 与体尾在**不同**落点上，
        //   用两条独立 goto + 双尾标签复现（旧 if 包裹形被 4.4 jump-thread）。
        if (quest == 0)
            goto QUEST_NULL;
        // ORIG 0x0868ae8b..0x0868aeb5：先 `call operator->; mov 0x4(%eax),%ebx`
        //   取 it->second，再 `call getCurCharacQuestR; add $0x4,%eax` 直内联 receiver
        //   （无具名 cleared 指针槽）⇒ 删具名 cleared（P183 地址重物化铁律）。
        if (((WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4))
                ->isClearedQuest((unsigned int)it->second))
        {
            incInitQuestPoint(quest->m_field138);
            incQuestPiece(quest->m_field13a, quest->m_index, true);
        }
        goto LOOP_NEXT;
QUEST_NULL:;
LOOP_NEXT:;
    }
}