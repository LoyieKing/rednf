void _postCheckForceChangeGrowType(CUser* user, int firstGrow, int secondGrow)
{
    // b28d-solo-CUser p167：ORIG 0x08668823 经 `getQuestList()` 一次取得 QuestList
    //   （MISSING ×1），容器按 const 引用访问（`begin() const`/`end() const`；我方非
    //   const 致 2 次转换构造）；命中块由两分支 **goto 共享**（我方重复两份 ⇒
    //   EXTRA 4AheadQuest/getCurCharacQuestW/setClearedQuest 各 ×1，P276）。
    QuestList* questList = G_CDataManager()->m_questList;   // ORIG G_CDataManager()+0x18
    const __gnu_cxx::hash_map<int, Quest*>& qmap =
        questList->getQuestList()->m_questMap;
    __gnu_cxx::hash_map<int, Quest*>::const_iterator it = qmap.begin();
    for (; it != qmap.end(); ++it)
    {
        Quest* quest = it->second;
        // ORIG 0x8668893：checkPossibleJob 取反后为真 → 跳过
        if (!quest->checkPossibleJob((ENUM_CHARACTERJOB)user->get_charac_job()))
            continue;

        if (quest->m_fieldB8 == 1)
        {
            // +0xb8==1：+0xc0 == firstGrow 命中
            if (quest->m_fieldC0 == firstGrow)
                goto HIT;
        }
        else if (quest->m_fieldB8 == 2)
        {
            // +0xb8==2：checkMatchedGrowType(job, firstGrowType, -1) 且 +0xc0 == secondGrow
            char growType = user->getCurCharFirstGrowType();     // movsbl
            if (!quest->checkMatchedGrowType((ENUM_CHARACTERJOB)user->get_charac_job(),
                                             growType, -1))
                continue;
            if (quest->m_fieldC0 != secondGrow)
                continue;
            goto HIT;
        }
        continue;
HIT:
        _postCheckForceChangeGrowType4AheadQuest(user, quest);
        // ORIG 0x8668939：getCurCharacQuestW()+0x4 → CQuestClear::setClearedQuest(idx)
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest((unsigned int)quest->m_index);
    }

    // ORIG 0x8668984：PacketGuard → getCurCharacQuestR()->get_quest_info(pg) → Send
    PacketGuard packet;
    user->getCurCharacQuestR()->get_quest_info(
        reinterpret_cast<char*>(&packet));                       // ORIG mangled 形参 Pc
    user->Send(packet);
