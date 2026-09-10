int Inter_VillageAttackedReward::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_VillageAttackedRewardPktView* pPkt = reinterpret_cast<Inter_VillageAttackedRewardPktView*>(data);
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())   // test al → false 才继续
        return 0;
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||   // +0x1a8
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    if (user == 0)                                     // cmp 0xc(%ebp),0
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_e)          // msg+0x0e characNo
        return 0;

    const int rewardType = pPkt->m_16;
    // ORIG 0x084dfea6：cmp 链 5→6→3（jne 尾），reward3 代码在比较链后 fallthrough，
    // reward5/6 代码跳转在后。保持 if 顺序 5→6→3 以匹配。
    if (rewardType == 5)
    {   // 胜利点数奖励
        Arad_VillageAttackRewardItem::Data rewardData(   // -0x64
            pPkt->m_12, 0, 0);
        const int winPoint = pPkt->m_12;
        ARAD::Arad_DataManager* adm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        Arad_VillageAttackRewardItem* reward =
            adm->getAradVillageAttackReward();
        if (reward->find_reward_item(winPoint, rewardData))
        {
            user->SetVillageAttackedReward(true);
            _send_reward_item(user, (unsigned long)rewardData.itemIndex,
                              rewardData.itemCount, rewardData.winPoint);
            cMyTrace(__PRETTY_FUNCTION__,
                0x4d7b, 0)("VillageAttacked Reward WinPoint Item Index. "
                  "[charac:%u][win:%d][item:%u][count:%d]",
                  user->get_charac_no(-1), rewardData.winPoint,
                  rewardData.itemIndex, rewardData.itemCount);
        }
    }
    else if (rewardType == 6)
    {   // 排名奖励
        Arad_VillageAttackRewardItem::RankData rankData;   // -0x74（memset 0x10）
        const short rank =
            (short)pPkt->m_12;   // movswl
        ARAD::Arad_DataManager* adm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        Arad_VillageAttackRewardItem* reward =
            adm->getAradVillageAttackReward();
        if (reward->find_ranking_reward_item(rank, rankData))
        {
            // ORIG dispatch 以两组并行 int 数组访问 out：
            // itemIdx = out[0..1]（-0x74+i*4）、count = out[2..3]（-0x6c+i*4）。
            int* fields = reinterpret_cast<int*>(&rankData);
            for (int i = 0; i <= 1; ++i)                   // cmpl $0x1,setle
            {
                if (fields[i] != 0)
                {
                    _send_reward_item(user, (unsigned long)fields[i],
                                      fields[2 + i],
                                      pPkt->m_12);
                    cMyTrace(__PRETTY_FUNCTION__,
                        0x4d8c, 0)("VillageAttacked Reward Ranker Item Index. "
                          "[charac:%u][rank:%d][item:%u][count:%d]",
                          user->get_charac_no(-1),
                          pPkt->m_12,
                          fields[i], fields[2 + i]);
                }
            }
        }
        else
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x4d92, "VillageAttackedReward Invalid Rank. [rank:%d]",
                pPkt->m_12);
            return 0;
        }
    }
    else if (rewardType == 3)
    {   // 参战奖励（固定物品）
        user->SetVillageAttackedReward(true);
        Inven_Item item;                               // -0xb1
        item.m_addInfo = 1;                            // +0x02 dword
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)3, item, 1);
        item.m_fieldb = 1;                             // +0x0b word（endurance）
        if (item.m_addInfo == -1)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x4d5f,
                "Inter_VillageAttackedReward::dispatch_sig() error item_id(%d)",
                -1);
        }
        else
        {
            const char* title =
                g_scriptStringManager_.findString(4, "game_server_msg_124", 0);
            char titleBuf[0x14];                       // -0x55
            strncpy(titleBuf, title, 0x14);
            const char* body =
                g_scriptStringManager_.findString(4, "game_server_msg_125", 0);
            char bodyBuf[0xff];                        // -0x1b1
            strncpy(bodyBuf, body, 0xff);
            const ENUM_SERVER_GROUP group =
                (ENUM_SERVER_GROUP)user->GetServerGroup();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item, 0,
                (unsigned int)user->getCurCharacNo(), bodyBuf,
                (int)strlen(bodyBuf), 0x1e, group, false, false);
        }
    }
    return 0;
