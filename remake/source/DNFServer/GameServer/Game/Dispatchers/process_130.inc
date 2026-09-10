int Dispatcher_Req_Urgent_Quest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    ReqResult& res = static_cast<ReqResult&>(param);
    if (user->get_state() <= 2)
    {
        return 0;
    }

    res.m_found = 0;
    res.m_questIdx = -1;

    int level = user->get_charac_level();
    UserQuest* questR = user->getCurCharacQuestR();
    bool eligible = level > 0x1a && level <= 0x45 &&
                    questR->m_authenData.m_urgentQuest == -1;
    if (!eligible)
    {
        res.m_errorCode = 1;
        return 0;
    }
    if (user->getCurCharacQuestW()->hasEpicQuest())
    {
        res.m_errorCode = 1;
        return 0;
    }

    // 函数局部静态（ORIG __cxa_guard 保护）：候选任务列表 + 随机骰。
    static std::list<int> selected_quest;
    selected_quest.clear();

    // [b28d-solo-Dispatchers] ORIG local_7589 = 0x7531 字节快照（memcpy 直填，
    //   0x081df57c mov $0x7531 / 0x081df58c call memcpy；全函数**无** CQuestClear
    //   构造调用）。ORIG 有 D1Ev ×2（0x081df71f 正常 + 0x081df739 EH），但
    //   `_ZN8WongWork11CQuestClearD1Ev` 0x0822e4cc 为**空体**（push/mov/pop/ret）
    //   ⇒ 缺该调用无观察行为差。实测真实对象形（`WongWork::CQuestClear cleared;`）
    //   会额外发射 C1Ev call（ORIG 无）并使 missing 822→825（四数闸门）⇒ 保留
    //   char 占位形，本项按 N/P64 建模豁免封顶。
    char cleared[0x7531];
    memcpy(cleared, &user->getCurCharacQuestR()->m_cleared, 0x7531);

    stSelectQuestParam selectParam(user);
    // ORIG：getQuestNPCList/allowable_questlist_as_npc 各自写完整表达式
    //（两次 G_CDataManagerv 调用，不缓存 questList 指针）
    std::multimap<int, int>* npcList =
        G_CDataManager()->m_questList->getQuestNPCList(ENUM_QUEST_GRADE_8);
    G_CDataManager()->m_questList->allowable_questlist_as_npc(
        selected_quest, *npcList, selectParam,
        *reinterpret_cast<const WongWork::CQuestClear*>(cleared), true);

    if (selected_quest.empty())
    {
        res.m_errorCode = 4;
    }
    else
    {
        static CMTRand urgentRandomDice;  // ORIG __cxa_guard 静态
        unsigned long range = (unsigned long)(selected_quest.size() - 1);
        unsigned int dice = urgentRandomDice.randInt(range);
        if (dice < selected_quest.size())
        {
            std::list<int>::iterator it = selected_quest.begin();
            std::advance(it, dice);
            int selectedIdx = *it;
            user->getCurCharacQuestW()->m_authenData.m_urgentQuest = selectedIdx;
            res.m_questIdx = selectedIdx;
            res.m_found = 1;
        }
    }
    return 0;
