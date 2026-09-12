// [R13fixE2 E轴 prett] ORIG 0x08b9c9e0 = "…(int, itemindex_t, short int, arad_uint32)"
// ⇒ 形参拼写用 GameTypes.h 既有 typedef（同底层类型，mangled `clEimsjE` 不变）。
bool FnRewardInterface::operator()(int eventCode, itemindex_t itemIndex,
                                   short count, arad_uint32 param)
{
    if (m_user == 0 || m_user->get_charac_no(-1) == 0)
        return false;

    Inven_Item item;
    if (!ARAD_ITEM::arad_make_item(itemIndex, (int)count, item))
        return false;

    char mailBuf[0x100] = {0};
    char titleBuf[0x15] = {0};
    GetMailString(mailBuf, titleBuf);

    int mailRet = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, 0, m_user->get_charac_no(-1), mailBuf,
        (int)std::strlen(mailBuf), 0xc, m_user->GetServerGroup(), false,
        false);
    if (mailRet != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_EventPeriodDataManager.cpp",
            __PRETTY_FUNCTION__,
            0x2f,
            "[FnReward] send mail fail. (code:%u, charac:%u, item:%u, cnt:%d)",
            eventCode, m_user->get_charac_no(-1), itemIndex, (int)count);
        return false;
    }

    EventRewardItemArad data = make_data(eventCode);
    arad::SigSaveRewardEventItem sig;
    sig.set(eventCode, data.m_accId, data.m_characNo, itemIndex, count, param);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_EventPeriodDataManager.cpp", 0x36),
        true);
    DISPATCHER::make_internal_stream_jpn(guard,
                                         (ENUM_INTERNALPACKET_JPN)0x27,
                                         (int)m_user->GetUID());
    guard->put_binary(&sig, 0x16);
    return GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
}