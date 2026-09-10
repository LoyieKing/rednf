bool FnRewardLethe::operator()(int eventCode, unsigned long itemIndex,
                               short count, unsigned int param)
{
    if (m_user == 0 || m_user->get_charac_no(-1) == 0)
        return false;

    Inven_Item item;
    if (!ARAD_ITEM::arad_make_item(itemIndex, (int)count, item))
        return false;

    char mailBuf[0x100] = {0};
    char titleBuf[0x15] = {0};
    // ORIG：param_1==0x60 用 soccer 金牌字符串，否则 lethe 夏日活动串。
    if (eventCode == 0x60)
    {
        std::strncpy(titleBuf,
                     g_scriptStringManager_.findString(
                         4, "2012_soccer_goldmedal_title", (bool*)0),
                     0x14);
        std::strncpy(mailBuf,
                     g_scriptStringManager_.findString(
                         4, "2012_soccer_goldmedal_mail", (bool*)0),
                     0xff);
    }
    else
    {
        std::strncpy(titleBuf,
                     g_scriptStringManager_.findString(
                         4, "lethe_2012_summer_event_title", (bool*)0),
                     0x14);
        std::strncpy(mailBuf,
                     g_scriptStringManager_.findString(
                         4, "lethe_2012_summer_event_mail", (bool*)0),
                     0xff);
    }

    int mailRet = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, 0, m_user->get_charac_no(-1), mailBuf,
        (int)std::strlen(mailBuf), 0xc, m_user->GetServerGroup(), false,
        false);
    if (mailRet != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_EventPeriodDataManager.cpp",
            __PRETTY_FUNCTION__,
            0x66,
            "[RewardLethe] send mail fail. (charac:%u, item:%u, cnt:%d)",
            m_user->get_charac_no(-1), itemIndex, (int)count);
        return false;
    }

    EventRewardItemArad data = make_data(eventCode);
    arad::SigSaveRewardEventItem sig;
    sig.set(eventCode, data.m_accId, data.m_characNo, itemIndex, count, param);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_EventPeriodDataManager.cpp", 0x6d),
        true);
    DISPATCHER::make_internal_stream_jpn(guard,
                                         (ENUM_INTERNALPACKET_JPN)0x27,
                                         (int)m_user->GetUID());
    guard->put_binary(&sig, 0x16);
    return GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
