void CUser::_onLevelUp()
{
    // ---- ① 15 级且角色创建时间在活动窗内：记 unlimit 支援日志（+0x0c..+0x81）----
    if (get_charac_level() == 0xf
        && getCurCharacCreateTime() > 0x47bc94cf           // +0x27 cmp/jle（有符号）
        && GlobalData::s_systemTime_.getCurSec() < 0x47d3fb71)  // +0x35 cmp $0x47d3fb70/jg
    {
        DB_InsertUnlimitSupportLog::makeRequest(get_acc_id(), (unsigned)getCurCharacNo(),
                                                getCurCharacName());
    }

    // ---- ② 等级奖励（+0x86..+0xc3）----
    _processLevelReward((unsigned)get_charac_level(),      // _ZN5CUser19_processLevelRewardEjjc
                        (unsigned)getCurCharacNo(),
                        (char)get_charac_job());

    // ---- ③ 重复事件 0x15 活跃时：18/48 级达成事件日志（+0xc8..+0x176）----
    // ORIG 经 vptr+0x34（slot13=IsEventing(CUser*)）判活，实参传 0；无判空（忠实转写）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x15)->IsEventing((CUser*)0))
    {
        if (get_charac_level() == 0x12)
            DB_InsertArchieveEventLog::makeRequest(get_acc_id(), (unsigned)getCurCharacNo(),
                                                   SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE_1);
        if (get_charac_level() == 0x30)
            DB_InsertArchieveEventLog::makeRequest(get_acc_id(), (unsigned)getCurCharacNo(),
                                                   SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE_2);
    }

    // ---- ④ 服务器组 10 活动窗内 30/40 级：系统邮件发放金币（+0x17b..+0x3ce）----
    // G_CEnvironment()+0x378 == m_serverEnvir.m_serverGroup == 10；
    // 时间上界 0x478e7051（cmp $0x478e7050/ja → <=0x478e7050 通过）。
    if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 10
        && (unsigned int)GlobalData::s_systemTime_.getCurSec() < 0x478e7051)
    {
        if (get_charac_level() == 0x1e || get_charac_level() == 0x28)
        {
            Inven_Item item;                                   // +0x1ea Inven_Item::Inven_Item（两分支共享）
            char desc[256];                                    // local_26c（strncpy 上限 0xff）

            if (get_charac_level() == 0x1e)                    // +0x1fd 分支 A
            {
                strncpy(desc, g_scriptStringManager_.findString(4, "game_server_msg_11", (bool*)0),
                        0xff);                                 // +0x227/+0x241
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    /*pSender*/ g_scriptStringManager_.findString(4, "game_server_msg_09", (bool*)0),
                    item, 500000, (unsigned)getCurCharacNo(),  // +0x2c2 movl $0x7a120
                    /*title*/ desc, strlen(desc), 0, GetServerGroup(), false, false);
            }
            else if (get_charac_level() == 0x28)               // +0x2e1 分支 B（ORIG 二次独立调用）
            {
                strncpy(desc, g_scriptStringManager_.findString(4, "game_server_msg_12", (bool*)0),
                        0xff);                                 // +0x319/+0x333
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    /*pSender*/ g_scriptStringManager_.findString(4, "game_server_msg_09", (bool*)0),
                    item, 1000000, (unsigned)getCurCharacNo(), // +0x3b4 movl $0xf4240
                    /*title*/ desc, strlen(desc), 0, GetServerGroup(), false, false);
            }
        }
    }

    // ---- ⑤ 可邮寄任务触发（+0x3ce..+0x4d5）：list<int> + stSelectQuestParam ----
    std::list<int> mailQuests;
    stSelectQuestParam selectParam(this);                  // +0x3e6 stSelectQuestParam(CUser*)
    {
        if (getCurCharacQuestR()->get_mail_quest_info(mailQuests, selectParam))  // +0x407/test %al
        {
            for (std::list<int>::iterator it = mailQuests.begin();
                 it != mailQuests.end(); ++it)                 // +0x414..+0x4cf 迭代
            {
                int questId = *it;                             // +0x438 ORIG 具名槽 local_30
                Quest* quest = G_CDataManager()->find_quest(questId);   // +0x44a find_quest
                // ORIG 三守卫各落独立 nop 中继岛（P223 多岛形：标签序 = 守卫序）
                if (quest == 0) goto questNext1;               // +0x456 岛1
                if (quest->m_mailFlag != 0) goto questNext2;   // +0x462 movzbl 0x108/test 岛2
                if (get_charac_level() != quest->m_levelMin)   // +0x479 cmp 0x20(m_levelMin)
                    goto questNext3;                           // +0x47e 岛3
                _postQuestTriggerItem(quest);                  // +0x48d
            questNext1:;                                       // +0x494
            questNext2:;                                       // +0x497
            questNext3:;                                       // +0x49a
            }
        }
    }                                                          // +0xb95 ~list

    // ---- ⑥ 会员（premium type 0x10）限时内 30/40 级：按背包槽位补发道具邮件（+0x4d5..+0x7a6）----
    // 时间上界 0x46e1bbd0（cmp/ja → <=0x46e1bbd0 通过）。
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x10)
        && (unsigned int)GlobalData::s_systemTime_.getCurSec() <= 0x46e1bbd0)
    {
        // ORIG = 内联读 m_addInfo + 无槽直跳 ja ⇒ 连续 case 汇聚同一块（P320/P197④ 镜像族）
        switch ((unsigned)(getCurCharacInvenR()->GetInvenRef(0, 0xb)->m_addInfo - 0x65d6))
                                                               // +0x536 sub/cmp $0x4/ja（无符号区间）
        {
        case 0: case 1: case 2: case 3: case 4:
        {
            if (get_charac_level() == 0x1e)                    // +0x558 分支 A
            {
                char desc[256];                                // local_26c
                strncpy(desc, g_scriptStringManager_.findString(4, "game_server_msg_05", (bool*)0),
                        0xff);                                 // +0x57f/+0x599
                Inven_Item item;                               // +0x5a7 Inven_Item::Inven_Item（&local_f2）
                item.m_addInfo = 0xf;                          // +0x5b6 movl $0xf（item+2）
                item.m_field1 = 2;                             // +0x5bd movb $0x2（item+1，ITEM_TYPE）
                item.m_addInfo2 = 5;                           // +0x5c7 movl $0x5（item+7 堆叠数）
                item.m_fieldb = 0;                             // +0x5d0 movw $0x0（item+0xb 耐久）
                item.ResetItemAttr();                          // +0x5d9

                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    /*pSender*/ g_scriptStringManager_.findString(4, "game_server_msg_08", (bool*)0),
                    item, 0, (unsigned)getCurCharacNo(),
                    /*title*/ desc, strlen(desc), 0, GetServerGroup(), false, false);
            }
            else if (get_charac_level() == 0x28)               // +0x679 分支 B
            {
                char desc[256];
                strncpy(desc, g_scriptStringManager_.findString(4, "game_server_msg_06", (bool*)0),
                        0xff);                                 // +0x6b1/+0x6cb
                Inven_Item item;                               // +0x6d9（&local_12f）
                item.m_addInfo = 0x24;                         // +0x6de movl $0x24
                item.m_field1 = 2;                             // +0x6e8 movb $0x2
                item.m_addInfo2 = 5;                           // +0x6ef movl $0x5
                item.m_fieldb = 0;                             // +0x6f9 movw $0x0
                item.ResetItemAttr();                          // +0x70b

                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    /*pSender*/ g_scriptStringManager_.findString(4, "game_server_msg_08", (bool*)0),
                    item, 0, (unsigned)getCurCharacNo(),
                    /*title*/ desc, strlen(desc), 0, GetServerGroup(), false, false);
            }
            break;
        }
        }
    }

    // ---- ⑦ NPC 升级礼物 / 升级事件奖励 / 升级事件（+0x7a6..+0x7cf）----
    processNPCGiftOnLevelUp();                                 // 0x0866407a
    processLevelUpEventReward(false);                          // 0x08663cc0（实参 false）
    processLevelUpEvent();                                     // 0x0869115a

    // ---- ⑧ 85 级：疲劳电池 EndLevel 结算与日志（+0x7cf..+0x87b）----
    if (get_charac_level() == 0x55)
    {
        int descFatigue =                                      // cwtl 符号扩展 short→int
            (short)GlobalData::s_fatigueBatteryHandle_->ActiveFatigueBattery_EndLevel(this);
        if (descFatigue != 0)
        {
            DecFatigueBatteryCharging((short)descFatigue);     // +0x816 DecFatigueBatteryCharging
            // ORIG 在 ctor 前右起求 varargs 并跨 ctor 保活（%esi/%ebx）⇒ 全表达式临时链（P227）
            cMyTrace(__PRETTY_FUNCTION__, 0x4290, 0)
                ("FATIGUE BATTERY ACTIVE END_LEVEL: charno(%d) , descFatigueAmount(%d), cur charging(%d)",
                 (int)getCurCharacNo(), descFatigue,
                 (int)(short)GetFatigueBatteryCharging()); // +0x821 movswl（无槽内联实参）
        }
    }

    // ---- ⑨ 20 级：补发 4183(0x1057) 号物品至堆叠上限 3（+0x881..+0xb4e）----
    if (get_charac_level() == 0x14)
    {
        CItem* script = G_CDataManager()->find_item(0x1057);   // +0x8a4 find_item
        if (script != 0)
        {
            Inven_Item item;                                   // +0x8bf Inven_Item::Inven_Item
            int slot = getCurCharacInvenR()->GetInvenData(0x1057, item);  // +0x8e4 GetInvenData
            if (slot < 0)                                      // +0x8f0 jns
            {
                script->make_item(item);                       // +0x910 vtbl+0x08 make_item(Inven_Item&)
                item.m_addInfo2 = 3;                           // +0x912 movl $0x3（item+7 堆叠数）
                item.m_addInfo = 0x1057;                       // +0x91c movl（item+2）

                slot = getCurCharacInvenW()->insertItemIntoInventory(
                    item, eItemAddReason_0xd, true, false);    // +0x9ed（reason/bLog/b=false）
            }
            else if (item.m_addInfo2 <= 2)                     // +0xa00 cmp $0x2/jg（当前堆叠<3）
            {
                // ORIG 第 2 实参直读字段 m_addInfo2、第 3 实参内联 get_add_info（P0 无槽）
                m_historyLog.ItemAdd(INVEN_TYPE_EQUIP, 3 - item.m_addInfo2, item.get_add_info(),
                                     item, eItemAddReason_0xd);// +0xa54（INVEN_TYPE 1/reason 0xd）
                item.m_addInfo2 = 3;                           // +0xa63 movl $0x3
                getCurCharacInvenW()->update_item(INVEN_TYPE_EQUIP, slot, item);  // +0xb21
            }
            if (slot >= 0)                                     // +0xb2a js
                SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);       // +0xb49
        }
    }

    // ---- ⑩ 事件脚本管理器升级奖励（+0xb53..+0xb72）----
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->process_level_up_reward(this, false);                // 0x0810bf56（实参 false）
