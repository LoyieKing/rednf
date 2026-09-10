void CAppLoadChecker::RequestDB(int dbq, int logdbq, int timerq, int packetq)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ServerLoadChecker.cpp", 0x1b5),
        true);

    (**guard) << 0x183;
    (**guard) << -1;

    SigInsertServerLoadStatisticView* view =
        reinterpret_cast<SigInsertServerLoadStatisticView*>(
            guard->GetInBuffer<SIG_INSERT_SERVER_LOAD_STATISTIC>());
    view->m_serverGroup =
        static_cast<unsigned char>(G_CEnvironment()->get_server_group());
    view->m_channelNo =
        static_cast<unsigned char>(G_CEnvironment()->get_channel_no());
    view->m_dbq = static_cast<short>(dbq);
    view->m_logdbq = static_cast<short>(logdbq);
    view->m_timerq = static_cast<short>(timerq);
    view->m_packetq = static_cast<short>(packetq);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
