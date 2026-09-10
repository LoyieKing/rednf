bool NewAccountLevelUpEventToJob::processLevelUpToJob(CUser& user, bool growup)
{
    // ORIG：全部 getter 在实参位内联求值（P24），不设持久局部 —— key 构造
    // firstGrow→esi(movsbl)、job→ebx 跨调用缓存、level→eax 直传；去重循环
    // 的 ==level 与 makeRequest/snprintf 的 level/job 各使用点重新调用 getter。
    LevelUpRewardItemTableKey key(user.get_charac_level(),
                                  user.get_charac_job(),
                                  user.getCurCharFirstGrowType());
    std::vector<LevelUpRewardItemData> rewards;    // vector C1 0x081b8c00
    if (growup)
    {
        if (!G_CDataManager()->m_levelUpReward.get_data_when_growup(key, rewards))
            return false;                          // ebx=0 出口
    }
    else
    {
        LevelUpRewardItemData data;                // C1 0x081b88bc
        if (!G_CDataManager()->m_levelUpReward.get_data(key, data))
            return false;
        rewards.push_back(data);                   // push_back 0x081b8a52
    }
    // getCharacLevelUpGift_AccountOnce（T 0x08691e5c）返回 &m_levelUpGift
    // （+0x8ec04，+0x00 即 multimap<int,int>）；empty/lower_bound/upper_bound/
    // insert 均以该指针为 this。迭代器经默认构造 + 赋值（临时中转拷贝，
    // P28），upper_bound 结果缓存，循环体内 it!=end() 防御检查。
    std::multimap<int, int>& gifts =
        *static_cast<std::multimap<int, int>*>(
            user.getCharacLevelUpGift_AccountOnce());
    if (!gifts.empty())
    {
        std::multimap<int, int>::iterator it;
        it = gifts.lower_bound(user.get_charac_job());
        std::multimap<int, int>::iterator upper =
            gifts.upper_bound(user.get_charac_job());
        while (it != upper)
        {
            if (it != gifts.end() && it->second == user.get_charac_level())
                return false;                      // 该等级已领取 → 中止
            ++it;
        }
    }
    gifts.insert(std::make_pair(user.get_charac_job(),
                                user.get_charac_level()));
    DB_EventAccountCharacterLevelUpOnceGift::makeRequest(
        user.get_acc_id(), static_cast<short>(user.GetServerGroupToPvP()),
        user.getCurCharacNo(), static_cast<short>(user.get_charac_job()),
        static_cast<short>(user.get_charac_level()));
    char body[256] = { '\0' };                     // -0x1b5 起 0x100 清零（P4 内联 memset）
    OS_API::snprintf(body, 0xff,
                     g_scriptStringManager_.findString(4, "game_server_msg_286",
                                                       (bool*)0),
                     user.get_charac_level());
    for (std::vector<LevelUpRewardItemData>::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        Inven_Item item;
        item.m_addInfo = it->m_field0;             // -0xb3 ← data.f0
        G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0,
                                                  item, it->m_field4);
        // findString("game_server_msg_285") 在循环体内实参位（ORIG 每次迭代调）。
        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_285",
                                                  (bool*)0),
                item, 0, user.getCurCharacNo(), body,
                static_cast<int>(strlen(body)), 0,
                static_cast<ENUM_SERVER_GROUP>(user.GetServerGroupToPvP()), 0,
                0) != 0)
        {
            LogManager::logFormat(
                1, "NewAccountLevelUpEventToJob.cpp",
                __PRETTY_FUNCTION__,
                0x7f, "NewAccountLevelUpEventToJob Event Start!");
        }
    }
    return true;                                   // ebx=1
