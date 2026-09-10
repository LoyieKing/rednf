void CLogGameChannel::ReqSaveLog()
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("LogGameChannel.cpp", 0x20), true);
    guard.operator*()->operator<<(0x79);
    guard.operator*()->operator<<(-1);

    SIG_SAVE_LOGGAMECHANNEL* pSig =
        guard->GetInBuffer<SIG_SAVE_LOGGAMECHANNEL>();
    memset(pSig, 0, 0x3b);

    pSig->m_game = (char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    pSig->m_chGroup = (short)G_CEnvironment()->m_serverEnvir.m_channelNo;
    pSig->m_no = G_CEnvironment()->m_serverEnvir.m_gcNo;
    memcpy(pSig->m_channel, G_CEnvironment()->m_serverEnvir.m_channelName,
           strlen(G_CEnvironment()->m_serverEnvir.m_channelName));
    memcpy(pSig->m_ip, G_CEnvironment()->m_serverEnvir.m_serverIP,
           strlen(G_CEnvironment()->m_serverEnvir.m_serverIP));
    pSig->m_now = (short)m_currentUser;
    pSig->m_new = (short)m_inUser;
    pSig->m_out = (short)m_outUser;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
