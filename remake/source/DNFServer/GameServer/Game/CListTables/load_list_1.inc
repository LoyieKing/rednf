bool QuestList::load_list()
{
    // [b28d-solo-CListTables] ORIG 0x08353da4..0x08353dd8 函数头构造/清零序：
    // it C1Ev(-0x40c) → script C1Ev(-0x734) → quest=0(-0x410) → mit C1Ev(-0x414)
    // → item=0(-0x40) → truncate 调用 ⇒ 声明序照此排列（4.4 按声明序发射）。
    std::map<int, std::string>::iterator it;
    QuestScript script;
    Quest* quest = 0;
    std::vector<MonsterKillItem>::iterator mit;
    CItem* item = 0;
    sync_script::GetInstanceSyncScript()->truncate_quest_category();
    it = g_questScriptFileList.begin();
    while (it != g_questScriptFileList.end())
    {
        char* path = (char*)it->second.c_str();
        if (path == 0)
        {
            break;
        }
        if (!importQuestScript(&script, path))
        {
            LogManager::logFormat(1, "data_manager.cpp",
                                  __PRETTY_FUNCTION__, 0x1357,
                                  "Quest Script Error: %s\n", path);
            return 0;
        }
        // [b28d-solo-CListTables] ORIG 用 size()（p167: MISSING vector<_DailySchedule>::size）。
        if (script.m_dailySchedule.size() != 0)
        {
            // ORIG 0x8353eb1：lea script+0x2dc 直传 AddDailSchedule
            // （vector<_DailySchedule> const&，ORIG 0x8125c3c），无逐项转换。
            GlobalData::s_DailyScheduleManager->AddDailSchedule(
                script.m_dailySchedule);
        }
        if (!script.m_monsterKillList.empty())
        {
            // [b28d-solo-CListTables] ORIG 用非 const iterator + deEv（P167：it-> 改 (*it).）
            // + 函数头已 C1Ev、此处仅赋值 + 尾后置 mit++（ppEi，P28/P50）。
            mit = script.m_monsterKillList.begin();
            while (mit != script.m_monsterKillList.end())
            {
                item = G_CDataManager()->find_item((*mit).m_itemIdx);
                if (item == 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x136f,
                        "Quest Script Error: %s - Monster Kill Item %d is not "
                        "Exist\n",
                        path, (*mit).m_itemIdx);
                    return 0;
                }
                if (!(item->is_stackable() && item->GetItemType() == 8))
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x1375,
                        "Quest Script Error: %s - Monster Kill Item %d is not "
                        "Quest Item\n",
                        path, (*mit).m_itemIdx);
                    return 0;
                }
                mit++;
            }
        }
        if (script.m_questDataType == 2 || script.m_questDataType == 6)
        {
            int div = 4;
            if (script.m_questDataType == 6)
            {
                div = 5;
            }
            if (script.m_questData.size() % div != 0 ||
                script.m_questData.size() / div >= 4)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", __PRETTY_FUNCTION__,
                    0x1383,
                    "Quest Script Error: %s - Quest Data Type is %d, But "
                    "Argumemt Size is Wrong(%d)\n",
                    path, script.m_questDataType,
                    (int)script.m_questData.size());
                return 0;
            }
            // [b28d-solo-CListTables] ORIG 用 const_iterator（p167: C1<Pi> 转换构造 + deEv
            // + ppEi + ne(const,const)）。
            std::vector<int>::const_iterator vit = script.m_questData.begin();
            std::vector<int>::const_iterator vend = script.m_questData.end();
            // [P171/P232 照抄] ORIG 0x083541ec/0x083541f6 在循环前写
            // -0x422(int)/-0x41e(short) 两个死局部（全函数无读取点）。
            int deadQuestData = 0;
            short deadQuestData2 = 0;
            while (vit != vend)
            {
                int v1 = *vit++;
                int v2 = *vit++;
                int v3 = *vit++;
                if (script.m_questDataType == 6)
                {
                    // [b28d-solo-CListTables P310] ORIG 0x083542eb `mov -0x28,%eax;
                    // cmp $0xa,%eax; ja` 两段寄存器比较（unsigned 局部被 4.4 折成
                    // mem 直比）⇒ 局部声明 int + 比较处显式 (unsigned int)。
                    int enemyType = *vit++;
                    if ((unsigned int)enemyType > 10 ||
                        ((1 << (enemyType & 0x1f)) & 0x40e) == 0)
                    {
                        LogManager::logFormat(
                            1, "data_manager.cpp", __PRETTY_FUNCTION__,
                            0x139d,
                            "Quest Script Error: %s - Unknown enemy type(%d)\n",
                            path, enemyType);
                        return 0;
                    }
                }
                int triggerSize = *vit++;
                if (triggerSize > 0x1ff)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x13a4,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Trigger Size is Wrong(%d)\n",
                        path, script.m_questDataType, triggerSize);
                    return 0;
                }
            }
        }
        else if (script.m_questDataType == 0)
        {
            if ((script.m_questData.size() & 1) != 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", __PRETTY_FUNCTION__,
                    0x13ba,
                    "Quest Script Error: %s - Quest Data Type is %d, But "
                    "Argumemt Size is Wrong(%d)\n",
                    path, script.m_questDataType,
                    (int)script.m_questData.size());
                return 0;
            }
        }
        else if (script.m_questDataType == 1)
        {
            // [b28d-solo-CListTables] ORIG 0x0835440a.. 每处重调
            // script.m_questData.size()（10 次调用，无 size 具名缓存）。
            if (script.m_questDataSubType == 4)
            {
                if (script.m_questData.size() != 2)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x13c4,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Argumemt Size is Wrong(%d)\n",
                        path, script.m_questDataType,
                        (int)script.m_questData.size());
                    return 0;
                }
            }
            else if (script.m_questDataSubType == 6)
            {
                if (script.m_questData.size() != 2 &&
                    script.m_questData.size() != 3)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x13cc,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Argumemt Size is Wrong(%d)\n",
                        path, script.m_questDataType,
                        (int)script.m_questData.size());
                    return 0;
                }
            }
            else if (script.m_questDataSubType < 4 ||
                     script.m_questDataSubType == 5 ||
                     (script.m_questDataSubType > 6 &&
                      script.m_questDataSubType < 0xc))
            {
                if (script.m_questData.size() != 3)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x13d4,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Argumemt Size is Wrong(%d)\n",
                        path, script.m_questDataType,
                        (int)script.m_questData.size());
                    return 0;
                }
            }
            else if (script.m_questData.size() != 4)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", __PRETTY_FUNCTION__,
                    0x13dc,
                    "Quest Script Error: %s - Quest Data Type is %d, But "
                    "Argumemt Size is Wrong(%d)\n",
                    path, script.m_questDataType,
                    (int)script.m_questData.size());
                return 0;
            }
        }
        if (script.m_mapIdx != -1)
        {
            CMap* map = (CMap*)G_CDataManager()->find_map(script.m_mapIdx);
            if (map == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", __PRETTY_FUNCTION__,
                    0x13e9,
                    "Quest Script Error: %s - NPC wrong npc appear map index "
                    "%d.",
                    path, script.m_mapIdx2);
                return 0;
            }
            if (!G_CDataManager()->VerifyMap(*map, script.m_mapIdx2))
            {
                return 0;
            }
        }
        if (!sync_script::GetInstanceSyncScript()
                 ->insert_quest_category(&script))
        {
            printf("insert_quest_category Error: %s\n", path);
            return 0;
        }
        quest = new Quest();
        if (quest == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x13ff, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x13ff);
            return 0;
        }
        // [b28d-solo-CListTables] ORIG 0x08354d..：直接按值传 script（调用点仅 1 次
        // C1ERKS_ + 1 次 D1Ev），无具名 script2 副本。
        quest->set_quest(script);
        insert_NPC(script.m_npcIdx);
        m_questMap.insert(std::map<int, Quest*>::value_type(script.m_index, quest));
        if (script.m_questType == 4)
        {
            m_npcQuestList2.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 5)
        {
            m_npcQuestList1.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 0)
        {
            m_npcQuestList0.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 2)
        {
            m_npcQuestList4.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 6)
        {
            m_npcQuestList5.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 3)
        {
            m_npcQuestList7.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 1)
        {
            m_npcQuestList3.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 8)
        {
            m_npcQuestList6.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        if (script.m_mailQuestFlag != -1)
        {
            m_vecD4.push_back(script.m_index);
        }
        it++;
    }
    if (!m_vecD4.empty())
    {
        // [b28d-solo-CListTables] ORIG 0x08354c82 `movl $0x0,-0x1c` 循环外清零 +
        // 循环内 `q = find_quest()`（0x08354cde）⇒ q 声明在循环外。
        Quest* q = 0;
        // [b28d-solo-CListTables] ORIG 用 const_iterator（p167: C1IPi 共 2 处）。
        for (std::vector<int>::const_iterator it = m_vecD4.begin();
             it != m_vecD4.end(); ++it)
        {
            int questIdx = *it;
            q = find_quest(questIdx);
            if (q != 0)
            {
                if (q->m_mailFlag == -1)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x145c, "Mail Quest Error (quest index : %d)",
                        questIdx);
                    return 0;
                }
                if (q->m_mailTitle.size() == 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x1462,
                        "Mail Quest Error! mail title err(quest index : %d)",
                        questIdx);
                    return 0;
                }
                if (q->m_mailContents.size() == 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x1467,
                        "Mail Quest Error! mail contents err(quest index : %d)",
                        questIdx);
                    return 0;
                }
                if ((q->m_mailItems.size() & 1) != 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x146c,
                        "Mail Quest Error! mail enclose item err(quest index : "
                        "%d)",
                        questIdx);
                    return 0;
                }
            }
        }
    }
    if (WongWork::CQuestClear::syncScript())
    {
        return 1;
    }
    LogManager::logFormat(1, "data_manager.cpp", __PRETTY_FUNCTION__,
                          0x1496,
                          "WongWork::CQuestClear::syncScript() fail");
    return 0;
