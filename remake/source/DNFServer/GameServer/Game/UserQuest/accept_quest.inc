int UserQuest::accept_quest(int questIdx, char* buf, int& trigger)
{
    // b28d-40: N82 封顶——帧 0xcc vs 0xc0 + 字符串地址重定位/槽平移，语义闭合。
    // ORIG 0x086abf38。
    // 【b28e-M1S2 本波复核】403/404（我方多 1）。P331 核过：quest（ORIG -0x3c / 我方
    // -0x30）与循环 i（ORIG -0x34 / 我方 -0x28）槽序**同向**（非倒置）⇒ 不属
    // P331 可翻域；平移恒 0xc = ORIG 独有 `push %edi`（帧 0xcc vs 0xc0）⇒ N82/N7。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N7/N78/N82）→ 试=①prett2 缺口 0、
    //   strall 串差 0、p167 新增复核（EXTRA memset x1 = 我方 call vs ORIG 4.1
    //   `rep stos` 内联 11 dwords，N78 在案；其余 MISSING 属 GI/undefined-weak
    //   假阳性族）；②444 后端变体逐项相同；③帧差 0xc = ORIG 独有 `push %edi`
    //   （4.1 三 callee-saved：edi/esi/ebx vs 4.4 两：esi/ebx）+ 槽平移，属寄存器
    //   分配不可择；P339 quest/i 槽序已核同向非倒置 ⇒ 无新旋钮，维持（新=DIFF
    //   403/404 gap=-1, blocks=15）。
    // P339 明文禁把非 POD（stSelectQuestParam / vector）提前 ⇒ 不适用。
    // 语义轴：logFormat/cMyTrace 串与 ORIG 0x8cf3a14 / 0x8cf47a0 / 0x8cf3a21 /
    // 0x8cf3a64 一致（已核）。
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

    stSelectQuestParam selectParam(m_user);
    if (!m_user->IsGameMasterMode())
    {
        UserQuest* cur = m_user->getCurCharacQuestR();
        if (!checkAcceptableQuest(quest, cur->m_cleared, selectParam))
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
        // N78：ORIG 0x086ac1b5..0x086ac1c9 为 4.1 内联 memset（rep stos 11 dwords，
        // `mov $0xb,%edx`）；4.4 恒发 call memset（结构性差异，语义等价）。
        memset(reinterpret_cast<SelectQuestParamDataView*>(&selectParam)->m_data,
               0, 0x2c);
        // P183 禁指针缓存：ORIG 0x086ac1d3/0x086ac1f9/0x086ac21d 每使用点各取一次
        // getCurCharacInvenR()（p167 差集实证 MISSING ×2）。
        m_user->getCurCharacInvenR()->CalNeedSlot(&selectParam.m_level, erasedItems);
        int quick = m_user->getCurCharacInvenR()->QuickEmptyCount();
        for (int i = 1; i <= 10; ++i)
        {
            int cnt = m_user->getCurCharacInvenR()->InvenEmptycount(
                (Inven_Item::ITEM_TYPE)i);
            if (reinterpret_cast<SelectQuestParamDataView*>(&selectParam)->m_data[i] > cnt)
            {
                quick += cnt -
                    reinterpret_cast<SelectQuestParamDataView*>(&selectParam)->m_data[i];
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