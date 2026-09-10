void BingoEvent::sendMaxMatchLineReward(CUser& user)
{
    // [b28d-64] P48/P157：ORIG 0x080ca62d find 与 0x080ca652 end 各自独立调用
    // G_CDataManager()（3 次总计，含 find_item）；缓存 map 引用会少 1 次调用。
    std::map<int, std::pair<int, int> >::iterator it =
        G_CDataManager()->m_bingo.m_map30.find(kMaxMatchLine);
    if (it == G_CDataManager()->m_bingo.m_map30.end())
    {
        // ORIG 0x080ca695 引用 _ZZN10BingoEvent22sendMaxMatchLineRewardER5CUserE19
        // __PRETTY_FUNCTION__（P139）：必须传 __PRETTY_FUNCTION__ 生成同形局部静态符号。
        LogManager::logFormat(1, "BingoEvent.cpp",
                              __PRETTY_FUNCTION__, 0xef,
                              "bingo script error");
        return;
    }

    const int itemIdx = it->second.first;
    const int addInfo = it->second.second;

    Inven_Item item;
    CItem* found = G_CDataManager()->find_item(itemIdx);
    if (found != 0)
    {
        found->make_item(item);                     // vtable+0x08 槽
        item.set_add_info(addInfo);

        char sender[21];
        strncpy(sender, g_scriptStringManager_.findString(4, "game_server_msg_292", 0), 0x14);
        char title[256];
        strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_294", 0), 0xff);

        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, item, 0,
            static_cast<unsigned int>(user.getCurCharacNo()),
            title, static_cast<int>(strlen(title)),
            0, user.GetServerGroup(), false, false);
    }
