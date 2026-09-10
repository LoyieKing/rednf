bool CDBMsgSender::Send()
{
    if (m_stream == 0)
        return false;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, m_stream);
    m_stream = 0;
    return true;
}
