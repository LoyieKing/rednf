// ORIG Send @ 0x833f950：投递 DB_Q 后释放持有并返回 true。
bool CDBMsgSender::Send()
{
    if (m_stream == 0)
        return false;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, m_stream);
    m_stream = 0;
    return true;
}
