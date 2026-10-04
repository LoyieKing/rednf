void CAppLoadChecker::RequestDB(int dbq, int logdbq, int timerq, int packetq)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ServerLoadChecker.cpp", 0x1b5),
        true);

    (**guard) << 0x183;
    (**guard) << -1;

    SIG_INSERT_SERVER_LOAD_STATISTIC* view =
        guard->GetInBuffer<SIG_INSERT_SERVER_LOAD_STATISTIC>();
    view->m_serverGroup =
        static_cast<signed char>(G_CEnvironment()->get_server_group());
    view->m_channelNo =
        static_cast<signed char>(G_CEnvironment()->get_channel_no());
    view->m_dbqCnt = static_cast<short>(dbq);
    view->m_logdbqCnt = static_cast<short>(logdbq);
    view->m_netqCnt = static_cast<short>(timerq);
    view->m_packetqCnt = static_cast<short>(packetq);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}