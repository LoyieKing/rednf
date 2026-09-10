void DB_InsertArchieveEventLog::makeRequest(
    unsigned int accId, unsigned int characNo,
    SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7349), true);
    guard.operator*()->operator<<(0x117);          // SIG_INSERT_ACHIEVE_EVENT
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        unsigned int m_characNo;                   // +0x04
        SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE m_type;  // +0x08（原枚举存储）
    };
    View* v =
        reinterpret_cast<View*>(guard.operator->()->GetInBuffer<SIG_INSERT_ACHIEVE_EVENT>());
    v->m_accId = accId;
    v->m_characNo = characNo;
    v->m_type = type;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
