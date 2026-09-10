bool Arad_INTER_kAradJumpingCharacter::clearEpicQuest(CUser* user)
{
    // [b28d-solo-Arad_InternalDispatcher_INTER] P157/P183 逐字段重读 + P171 照抄：ORIG 0x081993c4 全链式
    // （getCurCharacQuestW/R 结果直接下标，无具名 questW/questR 槽）；
    // 返回类型 bool（尾 movzbl -0x29(%ebp),%eax 双出口）；r0 原码未初始化
    // （仅 3 条 movl $0 落 -0x34/-0x38/-0x3c）；getQuestNPCList 无 npcQuestList
    // 判空（ORIG 直接 mgr+0x18 取表调用）。
    bool anyCleared = false;                                // -0x29

    // [b28d-solo] ORIG 调 const 版 begin/end（_ZNKSt8multimap…3endEv/5beginEv，p167
    // MISSING）⇒ 指针本身 const。
    const std::multimap<int, int>* npcMap =
        G_CDataManager()->m_questList->getQuestNPCList((ENUM_QUEST_GRADE)0);  // -0x28
    if (npcMap == 0)
        return anyCleared;

    int clearedCount = 0;                                   // -0x24
    int level = user->get_charac_level();                   // -0x20

    for (int i = 0; i < 0x14; ++i)                          // -0x1c
    {
        if (user->getCurCharacQuestW()->m_quest[i] == 0)
            continue;
        Quest* quest = G_CDataManager()->find_quest(
            user->getCurCharacQuestR()->m_quest[i]);        // -0x18
        if (quest == 0)
            continue;
        if (quest->m_type != 0)
            continue;
        int r3 = 0, r2 = 0, r1 = 0, r0;                     // -0x34/-0x38/-0x3c/-0x40
        int levelMin = quest->m_levelMin;                   // -0x14
        if (levelMin < level)
        {
            user->getCurCharacQuestW()->m_cleared.setClearedQuest(
                (unsigned int)user->getCurCharacQuestR()->m_quest[i]);
            user->getCurCharacQuestW()->m_quest[i] = 0;
            user->getCurCharacQuestW()->m_trigger[i] = 0;
            user->quest_basic_reward(quest, r3, r2, r1, r0, true);
            anyCleared = true;
            ++clearedCount;
        }
    }

    // 第二遍：NPC list 对应任务族（P74 迭代器双旋钮：end 先于 begin 求值）。
    // [b28d-solo] P96/P270：ORIG 调 const_iterator 族（p167 EXTRA 非 const 5 符号）⇒
    // 显式 const_iterator（Ghidra C 亦为 _Rb_tree_const_iterator）。
    std::multimap<int, int>::const_iterator itEnd = npcMap->end();       // -0x30
    for (std::multimap<int, int>::const_iterator it = npcMap->begin();   // -0x44
         it != itEnd; ++it)
    {
        Quest* quest = G_CDataManager()->find_quest(it->second);   // -0x10
        if (quest == 0)
            continue;
        if (quest->m_index == 0xe04)
            continue;
        if (quest->m_index == 0xe05)
            continue;
        if (quest->m_index == 0xe06)
            continue;
        if (quest->m_index == 0xe07)
            continue;
        if (quest->m_index == 0xe08)
            continue;
        if (!quest->checkPossibleJob((ENUM_CHARACTERJOB)user->get_charac_job()))
            continue;
        int r3 = 0, r2 = 0, r1 = 0, r0;                     // -0x48/-0x4c/-0x50/-0x54
        int levelMin = quest->m_levelMin;                   // -0xc
        if (levelMin < level)
        {
            if (!user->getCurCharacQuestW()->m_cleared.isClearedQuest(
                    (unsigned int)quest->m_index))
            {
                user->getCurCharacQuestW()->m_cleared.setClearedQuest(
                    (unsigned int)quest->m_index);
                user->quest_basic_reward(quest, r3, r2, r1, r0, true);
                anyCleared = true;
                ++clearedCount;
            }
        }
    }
    return anyCleared;
