void InterSelectCreateDnfEventInfo::makeRequest(int rate, int hour, int eventDay)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x11b),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(9), 0xffffffff);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x120,
                              "[Taiwan, Event] hour:%d rate:%d event_day:%d",
                              hour, rate, eventDay);
    }
    else
    {
        msg->m_rate = rate;
        msg->m_hour = hour;
        msg->m_eventDay = eventDay;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
