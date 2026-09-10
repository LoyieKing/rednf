bool DB_Find_Factory_Hub_User::dispatch(int /*userIdx*/, int slot,
                                        Stream* stream)
{
    SIG_FIND_FACTORY_HUB_USER* sig =
        stream->GetOutBuffer<SIG_FIND_FACTORY_HUB_USER>();
    int characNo = 0;        // -0x28
    int unusedRowCount = 0;  // -0x20（ORIG 死局部，仅 movl $0，P232）
    int unusedFieldIdx = 0;  // -0x1c（同上）
    unsigned int mId = 0;    // -0x2c
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x08437a84：`movl $<str>,-0x14` 具名 char* 局部两跳（P244）。
    const char* query =
        "seLect charac_no,m_id from charac_info where charac_name='%s'";
    // ORIG 无 v/ov 视图指针槽——每处内联 cast（P89/P183）。
    db->set_query(query, sig->m_characName);
    if (!db->exec(true))
        return 0;
    for (unsigned int i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_int(0, characNo))
            return 0;
        if (!db->get_uint(1, mId))
            return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x850f), true);
    guard.operator*()->operator<<((int)0x1a1);
    guard.operator*()->operator<<((int)slot);
    SIG_FIND_FACTORY_HUB_USER_NO_CONN* out =
        guard.operator->()->GetInBuffer<SIG_FIND_FACTORY_HUB_USER_NO_CONN>();
    // [b28d-solo-W8LT_DBTask] 复核：显式 (int)/(short) cast 探针无效（仍 RHS
    // 两段中转）⇒ 字段/RHS 异型化旋钮（P211 档③）对本件不可翻，N76/N18a 封顶。
    out->m_mId = mId;
    out->m_characNo = characNo;
    out->m_flag = sig->m_flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
