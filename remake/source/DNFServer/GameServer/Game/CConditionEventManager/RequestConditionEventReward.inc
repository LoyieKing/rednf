// ORIG T 0x08334850
// _ZN22CConditionEventManager27RequestConditionEventRewardEP5CUserscRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 主奖励链：repeat 事件 0x71 未开启 → 0；curIdx 与 m_conditionEvent.m_field0 不符
// → 1；step<1 / step 超前 / rewardStep != step-1 → 0x13；物品缺失 → 0x11。
// 按 rewardMap 逐条发放：key==1 且 count>=0 → 事件币（EventCoinAdd 日志+保存+发送）；
// 其余 → find_item+create_item，guard（itemIdx==0 或 get_add_info()==0）跳过；
// 非特殊物品直接入库（失败邮件回退），特殊物品按 GetItemType()==0x10 / key==1
// 分支：0x10 直接入库，否则 ProcessSpecialItem（outRestrict!=0 → 0x11）。
// 全部成功 → IncCurConditionEventRewardStep() 并返回 0。
int CConditionEventManager::RequestConditionEventReward(
    CUser* user, short curIdx, char step, std::map<int, int>& rewardMap)
{
    // [M4S3 round5c · 已转 AE] 原残差 4 条 = ORIG `mov slot,%eax; shr $0x1f,%eax;
    // test %al,%al; je`（0x08334e64 / 0x08335074，insertItemIntoInventory 返回值
    // 最高位判定）×2 处 vs 4.4 `cmpl $0; jns`（2 条）。P211 类型轴破法：把两处判定
    // 写成 `(unsigned char)((unsigned int)slot >> 31)` —— 经 unsigned char 截断后
    // 4.4.6 不再折成 `test $0;jns`，保留 `mov;shr;test %al;je` 四条形（探针
    // /tmp/m4s3/p4.cpp g8/g10 逐条命中）。旧探针 f1..f5 失败原因是判据写在**有符号
    // 变量**上（`r<0`/`r>>31` 恒折），且 (unsigned)r>>31 缺 unsigned char 截断。
    // 改后等长同形（objdump 同口径 663/663；报告头 614 系其口径）、ndiff 0；
    // p167 双向差集仅 C1/C2·D1/D2 同址别名 P93，de/ptEv 双方各 7 条已对齐。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
    {
        return 0;
    }

    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if (curIdx != field0)
    {
        return 1;
    }

            if (step < 1)
        {
            return 0x13;
        }

        // ORIG 0x083348e6..0x083348fb：`movsbw step,%ax; cmp curStep,%ax; jle`
        // ⇒ 局部与比较均为 short 有符号（jbe 是无符号版，失配）。
        short curStep = static_cast<short>(user->GetCurConditionEventStep());
        if (curStep < static_cast<short>(step))
        {
            return 0x13;
        }

        short rewardStep = static_cast<short>(user->GetCurConditionEventRewardStep());
        if (static_cast<int>(rewardStep) != step - 1)
        {
            return 0x13;
        }

    std::vector<stConditionEventRewardInfo> infos;
    GetConditionRewardInfo(0, infos, curIdx, step);
    ConditionRewardSameItemCombine(infos, rewardMap);

    if (!rewardMap.empty())
    {
        for (std::map<int, int>::iterator it = rewardMap.begin();
             it != rewardMap.end(); ++it)
        {
            if (it->first == 1 && it->second >= 0)
            {
                // ORIG 0x083349ee 区：key==1 → 事件币发放。
                // ORIG 用 %esi 跨调用保活、无栈槽往返 ⇒ 原码是内联表达式（P0）。
                user->getCurCharacInvenW()->SetEventCoin(
                    user->getCurCharacInvenR()->GetEventCoin() + it->second);
                user->m_historyLog.EventCoinAdd(
                    user->getCurCharacInvenR()->GetEventCoin(), it->second,
                    static_cast<eCoinAddReason>(2));
                user->SaveMoneyCoin();
                user->SendCoin();
                continue;
            }

            CItem* item = G_CDataManager()->find_item(it->first);
            if (item != 0)
            {
                Inven_Item inv;
                // ORIG 0x08334acd：create_item 之前把 itemIdx 写进 inv+0x02（m_addInfo），
                // 其后 guard/IsSpecialItem/GetRestrictCode/ProcessSpecialItem 全部复读该
                // 成员（不再重取 it->first ⇒ p167 ptEv 多余 4 次的来源）。
                inv.m_addInfo = it->first;
                G_CDataManager()->m_itemList->create_item(
                    static_cast<ENUM_ITEM_CREATE_TYPE>(0), inv, it->second);

                // ORIG guard（0x08334b07..+0x2df）：itemIdx==0 或 get_add_info()==0 → 不处理。
                // ORIG 为内联短路（mov$1/mov$0 岛 + test %al，无槽）；具名 bool 会多 store+cmpb。
                if (inv.m_addInfo != 0 && inv.get_add_info() != 0)
                {
                    int outType = 0;
                    int outValue = 0;
                    bool isSpecial = false;
                    if (GlobalData::s_pCeraShop->IsSpecialItem(inv.m_addInfo, outType, outValue, isSpecial))
                    {
                        // ORIG 0x08334b7f 区：特殊物品 → GetItemType() 检查。
                        // ORIG 为否定守卫（`je <else>` 直落 ProcessSpecialItem），且 itemType
                        // 不落局部（两次独立 call GetItemType）⇒ 内联调用 + 翻转块序。
                        if (!(item->GetItemType() == 0x10 || inv.m_addInfo == 1))
                        {
                            // ORIG 0x08334bb5 区：ProcessSpecialItem 发放特殊物品。
                            int outRestrict = 0;
                            int restrictCode =
                                G_CDataManager()->m_itemList->GetRestrictCode(inv.m_addInfo);
                            long curSec = GlobalData::s_systemTime_.getCurSec();
                            std::vector<Inven_Item> items;
                            WongWork::CCeraShop::stCeraShopItemParam_t param;
                            // ORIG 0x08334c66..+0x6d 为内联短路（call;test %al,%al;je），
                            // 具名 bool 会多 store+cmpb 两条（P0）。
                            if (GlobalData::s_pCeraShop->ProcessSpecialItem(
                                    user, inv.m_addInfo, restrictCode, outRestrict, curSec, true,
                                    items, param, 0, 0, false, 1))
                            {
                                // ORIG 0x08334c73..+0x89 为 guard 形（outRestrict!=0 直落
                                // 0x11 出口），非 if/else 反向布局。
                                if (outRestrict != 0)
                                {
                                    return 0x11;
                                }
                                if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
                                {
                                    user->SetRestrictedGoods(restrictCode);
                                    GlobalData::s_pCeraShop->SaveFeaturedIdx(
                                        user, restrictCode, curSec, 0);
                                    user->SendRestrictedGoods(true, restrictCode);
                                }
                            }
                        }
                        else
                        {
                            // type==0x10 或 itemIdx==1：不做 ProcessSpecialItem；
                            // 仅 type==0x10 直接入库（0x08334d8e 区）。
                            if (item->GetItemType() == 0x10)
                            {
                                // ORIG 0x08335069..+0x71 两段式：先 `movl $-1,slot`
                                // 再独立赋值（P197③），合并初始化式会少一条 store。
                                int slot = -1;
                                slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                                    inv, static_cast<eItemAddReason>(0x25), true, true);
                                // ORIG 0x08335071..+0x82c `mov slot;shr $0x1f;test %al;je`
                                // = `(unsigned char)((unsigned)slot>>31)` 判定（P211 类型轴：
                                // 裸 `slot<0` / `(unsigned)slot>>31` 在 4.4 均折 `cmpl $0;jns`，
                                // 经 unsigned char 截断后 4.4.6 保留 shr+test %al 四条形）。
                                if ((unsigned char)((unsigned int)slot >> 31))
                                {
                                    // ORIG 0x08335082 区（与 0x08334e72 同构，sender 槽不同）：
                                    // 入库失败 → msg_80/msg_81 系统邮件 + inv.reset()。
                                    char sender[30];
                                    strncpy(sender,
                                            g_scriptStringManager_.findString(
                                                4, "game_server_msg_80", 0), 0x1d);
                                    char title[256];
                                    strncpy(title,
                                            g_scriptStringManager_.findString(
                                                4, "game_server_msg_81", 0), 0xff);
                                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                        sender, inv, 0,
                                        static_cast<unsigned int>(user->getCurCharacNo()),
                                        title, static_cast<int>(strlen(title)), 0,
                                        user->GetServerGroup(), false, false);
                                    inv.reset();
                                }
                                else
                                {
                                    // ORIG 0x08335184 区：此分支无 IsCreatureItemType 判定，
                                    // 恒按 itemspace 0 发送（Ghidra C local_24<0 的 else 臂）。
                                    user->SendUpdateItem(CUser::eSendTarget_1,
                                                         static_cast<ENUM_ITEMSPACE>(0), slot);
                                }
                            }
                        }
                    }
                    else
                    {
                        // ORIG 0x08334f9e 区：非特殊物品 → 直接入库。
                        // ORIG 0x08334f99..+0x61 两段式（同 0x08335069，P197③）。
                        int slot = -1;
                        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                            inv, static_cast<eItemAddReason>(0x25), true, true);
                        // ORIG 0x08334e61..+0x61c：同 0x08335071 的 shr 形（P211 类型轴）。
                        if ((unsigned char)((unsigned int)slot >> 31))
                        {
                            // ORIG 0x08334e72 区：入库失败 → msg_80/msg_81 系统邮件 + reset。
                            char sender[30];
                            strncpy(sender,
                                    g_scriptStringManager_.findString(
                                        4, "game_server_msg_80", 0), 0x1d);
                            char title[256];
                            strncpy(title,
                                    g_scriptStringManager_.findString(
                                        4, "game_server_msg_81", 0), 0xff);
                            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                sender, inv, 0,
                                static_cast<unsigned int>(user->getCurCharacNo()),
                                title, static_cast<int>(strlen(title)), 0,
                                user->GetServerGroup(), false, false);
                            inv.reset();
                        }
                        else if (inv.IsCreatureItemType())
                        {
                            user->SendUpdateItem(CUser::eSendTarget_1,
                                                 static_cast<ENUM_ITEMSPACE>(7), slot);
                        }
                        else
                        {
                            user->SendUpdateItem(CUser::eSendTarget_1,
                                                 static_cast<ENUM_ITEMSPACE>(0), slot);
                        }
                    }
                }
            }
            else
            {
                return 0x11;
            }
        }
    }

    user->IncCurConditionEventRewardStep();
    return 0;
}