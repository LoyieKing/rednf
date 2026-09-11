// ============================================================================
// CUser::quest_action(int cmd, int questId, int triggerVal, int opt)
// ORIG T _ZN5CUser12quest_actionEiiii @0x0866da8a。
// 报告 docs/class_func_reports/CUser/quest_action.md 全文已读；草稿（本文件
// quest_action W7-BLOCKED 注释区）按报告全文转写，本定义为其真实落地。
// cmd：0x21 accept / 0x22 giveup / 0x23 trigger / 0x24 finish(reward)。
// ============================================================================
// [b28d-22 N82] ORIG 0x0866da8a：p167=M0/E12:_ZN9__gnu_cxx17__normal_iteratorIPKiSt6vectorIiSaIiEEEppEv;_ZNK9__gnu_cxx17__normal_iteratorIPKiSt6vectorIiSaIiEEEdeEv;_ZNSt20_List_const_iteratorIiEC1ERKSt14_List_iteratorIiE，gap=9；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0866da8a：N82 封顶——帧/槽差 + 迭代器族（p167 M0/E12，gap 9）。
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
                // [M8BigTURest 2026-09-11 04:05 语义修复·字段错位] ORIG 0x0866dd34/0x0866dd5c/
                // 0x0866dd81 三处**都是 Quest+0x70**：`add $0x70,%eax; call
                // vector<pair<int,int>>::empty/size()` 与 `lea 0x70(%eax),%ebx` 作
                // delete_event_items 第一实参；我方 Quest 布局 0x70=m_rewardSelectList、
                // 0xac=m_triggerDataPairs（Quest.h:443，且 quest_reward 侧 ORIG 0xac 与
                // `m_triggerDataPairs.begin/end` 已对齐=布局两侧一致）⇒ 原稿把字段写成
                // m_triggerDataPairs 是**读错列**（运行期清错列表），按 ORIG 直证改回
                // m_rewardSelectList（三处：empty/size/传参）。
                if (!q2->m_rewardSelectList.empty())
                {
                    bool touched = false;
                    std::vector<int> updates;
                    updates.reserve(q2->m_rewardSelectList.size() * 3);
                    getCurCharacInvenW()->delete_event_items(
                        q2->m_rewardSelectList, updates, touched,
                        (eMoneySubReason)4, (eItemDelReason)4, false);
                    // [b28d-solo-CUserOnlyOrig P96/P270] p167 差集：MISSING
                    // `__normal_iterator<int*>::ppEv/deEv` + EXTRA `<const int*>` 族
                    // ⇒ ORIG 用非 const iterator（updates 非 const 局部）。
                    std::vector<int>::iterator ui = updates.begin();
                    while (ui != updates.end())
                    {
                        const int spId = *ui; ++ui;
                        // [H6 2026-09-11 单旋钮·类型轴] ORIG 该件 `mov %ax,-0x1c(%ebp)` +
                        // `movswl -0x1c(%ebp),%eax`（16 位具名槽 + 符号扩展为 SendUpdateItem 实参）
                        // ⇒ 原码是 **short** 局部（数量按 16 位截断）。旧 `const int cntS` 给 32 位槽。
                        const short cntS = static_cast<short>(*ui); ++ui;
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
            // [H6 2026-09-11 单旋钮·符号性] ORIG 0x0866e01d `cmp $0x1,%eax; jbe 0x0866e0cf`
            // = **无符号**比较；4.4 对 int 字段产 `jle`（有符号）⇒ 显式 unsigned 视角复现
            // （计数字段，语义等价区间内的负数不可达）。
            if (1 < static_cast<unsigned int>(m_postChecksumData.m_field4c))
            {
                // [H6 2026-09-11 单旋钮·P0 去具名] ORIG 0x0866e029 把 getHackAnalyzer() 结果
                // 直接落 (%esp) 调 addServerHackCnt（**无** ha 具名槽），旧形多一对 store/load。
                getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x192,
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
            // [H6 2026-09-11 单旋钮·P0 去具名 bool] ORIG 0x0866e166 `test %al,%al; je
            // 0x0866e220` 直判返回值（**无** hasMailQ 槽）；旧形多 `mov %al,-0x11; cmpb $0x0,-0x11`。
            if (getCurCharacQuestR()->get_mail_quest_info(mailQuestIds, selParam))
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
}