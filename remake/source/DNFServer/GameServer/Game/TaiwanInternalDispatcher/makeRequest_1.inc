void InterSelectCreateDnfUserInfo::makeRequest(int uid, unsigned int memberId,
                                               bool received)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x14d),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0xa), uid);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x152,
                              "[Taiwan, Event] m_id:%d isReceived:%d",
                              memberId, received);
    }
    else
    {
        msg->m_id = memberId;
        msg->m_received = received;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
