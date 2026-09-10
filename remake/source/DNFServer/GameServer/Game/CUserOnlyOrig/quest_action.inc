void CUser::quest_action(int cmd, int questId, int triggerVal, int opt)
{
    Quest* q = G_CDataManager()->find_quest(questId);
    if (q == 0)
        return;
    if (q->m_type == 7)
        return;

    PacketGuard packet;
    packet.put_header(1, cmd);

    // [b28d-22 修法] ORIG 0x0866dc84-0x0866dca1 为 4-case switch 跳转链
    // （case 序 0x22/0x21/0x23/0x24，探针 /tmp/p22_g1.cpp sw 逐条同构）；
    // if/else 链使分支体内联主干造成 854 行错位。
    switch (cmd)
    {
    case 0x22:                                                  // giveup
    {
        // ORIG：giveup_quest 返回值 local_38 <1（0 成功）→ 成功包；
        // ≥1（0x12/0x13/0x14 失败码）→ put_byte(0)+失败码。
        const int gResult = getCurCharacQuestW()->giveup_quest(questId);
        if (gResult < 1)
        {
            packet.put_byte(1);
            packet.put_short(questId);
            Quest* q2 = G_CDataManager()->find_quest(questId);
            if (q2 != 0)
            {
                if (!q2->m_triggerDataPairs.empty())
                {
                    bool touched = false;
                    std::vector<int> updates;
                    updates.reserve(q2->m_triggerDataPairs.size() * 3);
                    getCurCharacInvenW()->delete_event_items(
                        q2->m_triggerDataPairs, updates, touched,
                        (eMoneySubReason)4, (eItemDelReason)4, false);
                    // [b28d-solo-CUserOnlyOrig P96/P270] p167 差集：MISSING
                    // `__normal_iterator<int*>::ppEv/deEv` + EXTRA `<const int*>` 族
                    // ⇒ ORIG 用非 const iterator（updates 非 const 局部）。
                    std::vector<int>::iterator ui = updates.begin();
                    while (ui != updates.end())
                    {
                        const int spId = *ui; ++ui;
                        const int cntS = *ui; ++ui;
                        SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)spId, cntS);
                        ++ui;                                   // 第三元素只读丢弃
                    }
                }
                SendHasNoGoWithApcQuestInParty(q2);
            }
        }
        else
        {
            packet.put_byte(0);
            packet.put_byte(gResult);
        }
        packet.finalize(true);
        Send(packet);
        break;
    }
    case 0x21:                                                  // accept
    {
        Quest* qa = G_CDataManager()->find_quest(questId);
        // [b28d-solo-CUserOnlyOrig P133 真语义缺口] p167 MISSING 2×put_byte +
        // 1×finalize + 1×Send：ORIG 0x0866db5b/0x0866dbd1 两个独立守卫**各自内联**
        // 一份 `put_byte(0); put_byte(0x15); finalize(true); Send(packet);` 后 jmp 函数尾；
        // 合并 `revengeBlock || scheduleBlock` 单块少一组调用点（1 组=2 put_byte+finalize+Send）。
        if (qa != 0 && qa->m_field134 == 1 &&
            !GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
        {
            packet.put_byte(0);
            packet.put_byte(0x15);
            packet.finalize(true);
            Send(packet);
            break;
        }
        if (qa != 0 && qa->m_field134 == 2 &&
            !qa->IsOpenScheduleQuest())
        {
            packet.put_byte(0);
            packet.put_byte(0x15);
            packet.finalize(true);
            Send(packet);
            break;
        }
        {
            int local42 = 0;
            const int errCode =
                getCurCharacQuestW()->accept_quest(
                    questId, (char*)&packet, local42);
            if (errCode > 0)
            {
                packet.put_byte(0);
                packet.put_byte(errCode);
            }
            packet.finalize(true);
            Send(packet);
        }
        break;
    }
    case 0x23:                                                  // trigger
    {
        const char b = (char)opt;
        const int trigResult =
            getCurCharacQuestW()->set_trigger(
                questId, (char)triggerVal, b != 0);
        if (trigResult == -1)
        {
            packet.put_byte(0);
            packet.put_byte(0x16);
        }
        else
        {
            packet.put_byte(1);
            packet.put_short(questId);
            packet.put_int(trigResult);
            if (trigResult == 0)
            {
                Quest* qt = G_CDataManager()->find_quest(questId);
                if (qt != 0)
                    SendHasNoGoWithApcQuestInParty(qt);
            }
        }
        packet.finalize(true);
        Send(packet);
        break;
    }
    case 0x24:                                                  // finish(reward)
    {
        const unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
        if ((now - m_postChecksumData.m_field48) <= 0x3e8)      // 0x79644 近期已请求
        {
            ++m_postChecksumData.m_field4c;                     // 0x79648
            if (1 < m_postChecksumData.m_field4c)
            {
                WongWork::CHackAnalyzer* ha = getHackAnalyzer();
                ha->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x192,
                                     1, 0, 0);
                packet.put_byte(0);
                packet.put_byte(1);
                packet.finalize(true);
                Send(packet);
                return;
            }
        }
        else
        {
            m_postChecksumData.m_field4c = 0;
            m_postChecksumData.m_field48 =
                GlobalData::s_systemTime_.getCurTickCount();
        }

        const int err = quest_reward(questId, triggerVal, opt);
        if (err > 0)
        {
            packet.put_byte(0);
            packet.put_byte(err);
            packet.finalize(true);
            Send(packet);
            HistoryLog::WriteFineQuest((FILE*)m_field796f8, questId);
        }
        else
        {
            std::list<int> mailQuestIds;
            stSelectQuestParam selParam(this);
            const bool hasMailQ = getCurCharacQuestR()
                                      ->get_mail_quest_info(mailQuestIds,
                                                            selParam);
            if (hasMailQ)
            {
                // [b28d-solo-CUserOnlyOrig P96/P270] p167：MISSING `_List_iterator<int>`
                // 族 + EXTRA `_List_const_iterator` 转换构造 ⇒ 非 const iterator。
                for (std::list<int>::iterator mi = mailQuestIds.begin();
                     mi != mailQuestIds.end(); ++mi)
                {
                    Quest* mq = G_CDataManager()->find_quest(*mi);
                    if (mq != 0 && mq->m_mailFlag == 1 && mq->m_index == questId)
                        _postQuestTriggerItem(mq);
                }
            }
            getCurCharacQuestW()->finish_quest(questId);
            {
                PacketGuard qi;
                getCurCharacQuestR()->get_quest_info((char*)&qi);
                Send(qi);
            }
            Quest* qz = G_CDataManager()->find_quest(questId);
            if (qz != 0 && qz->m_type == 3)
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    this, (APSystem::_ActionGroupIndex)0xe, 0,
                    (ENUM_PACKETCLASS)0);
            m_historyLog.QuestComplete(questId);
            _onQuestClear(questId, opt);
        }
        break;
    }
    default:
        break;
    }
