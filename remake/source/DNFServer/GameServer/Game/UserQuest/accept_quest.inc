int UserQuest::accept_quest(int questIdx, char* buf, int& trigger)
{
    // ORIG 0x086abf38. Its reused stack slot holds 11 inventory counters;
    // keep that storage separate from the 40-byte eligibility parameters.
    if (m_user == 0)
    {
        LogManager::logFormat(1, "user_quest.cpp",
                                 __PRETTY_FUNCTION__, 0x1e8,
                                 "user is null");
        return 1;
    }
    if (m_user->getCurCharacR() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1ef, 5);
        trace("UserQuest::accept_quest user->m_selected is null");
        return 1;
    }
    else
    {

    Quest* quest = G_CDataManager()->find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(1, "user_quest.cpp",
                                 __PRETTY_FUNCTION__, 0x1f8,
                                 "G_CDataManager()->find_quest(%d)", questIdx);
        return 1;
    }

    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            return 0x12;
        }
    }

    if (!m_user->IsGameMasterMode())
    {
        // P225：ORIG 0x086ac0c5/0x086ac0d7/0x086ac0e8 三守卫各自 je 同一出口
        // （无 setcc 物化）⇒ 源码必须是嵌套 if，禁 && 短路链（4.4 恒物化 bool）。
        if (!quest->isRepeatableQuest())
        {
            if (m_cleared.isClearedQuest(questIdx))
            {
                return 0x12;
            }
        }
    }

    if (!m_user->IsGameMasterMode())
    {
        stSelectQuestParam selectParam(m_user);
        if (!checkAcceptableQuest(quest, m_user->getCurCharacQuestR()->m_cleared,
                                  selectParam))
        {
            m_user->getHackAnalyzer()->addServerHackCnt(
                m_user, (WongWork::ENUM_HACKTYPE)0x191, 1, 0, 0);
            return 1;
        }
    }

    std::vector<std::pair<int, int> > rewardItems;
    std::vector<std::pair<int, int> > erasedItems;
    rewardItems = quest->m_rewardSelectList;

    int erased = m_user->getCurCharacInvenW()->erase_repeated_item(
        rewardItems, erasedItems);
    if (erased > 0)
    {
        // ORIG 0x086ac1b5..0x086ac1c9 reuses the stack slot as 11 ints.
        // stSelectQuestParam is only 10 ints wide; it is not slot-count storage.
        int neededSlots[11] = {};
        // P183 禁指针缓存：ORIG 0x086ac1d3/0x086ac1f9/0x086ac21d 每使用点各取一次
        // getCurCharacInvenR()（p167 差集实证 MISSING ×2）。
        m_user->getCurCharacInvenR()->CalNeedSlot(neededSlots, erasedItems);
        int quick = m_user->getCurCharacInvenR()->QuickEmptyCount();
        for (int i = 1; i <= 10; ++i)
        {
            int cnt = m_user->getCurCharacInvenR()->InvenEmptycount(
                (Inven_Item::ITEM_TYPE)i);
            if (neededSlots[i] > cnt)
            {
                quick += cnt - neededSlots[i];
                if (quick < 0)
                {
                    return 0x11;
                }
            }
        }
    }

    trigger = quest->get_init_trigger();
    for (int i = 0; i < 20; ++i)
    {
        if (m_quest[i] == 0)
        {
            m_cleared.resetClearedQuests(questIdx);
            m_quest[i] = questIdx;
            m_trigger[i] = trigger;

            InterfacePacketBuf* pb = reinterpret_cast<InterfacePacketBuf*>(buf);
            pb->put_byte(1);
            pb->put_short(questIdx);
            pb->put_int(trigger);

            std::vector<int> itemIdx;
            int insertResult = m_user->getCurCharacInvenW()->insert_event_items(
                erasedItems, itemIdx,
                (eMoneyAddReason)3, (eItemAddReason)8, false, "", "");
            pb->put_byte(insertResult);

            for (std::vector<int>::iterator it = itemIdx.begin();
                 it != itemIdx.end(); it++)
            {
                pb->put_short(*it);
                it++;
                pb->put_int(*it);
                it++;
                pb->put_int(*it);
            }
            return 0;
        }
    }
    return 4;
    }
}