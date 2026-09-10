bool DB_CreateAccountCargo::dispatch(int userIdx, int slot, Stream* stream)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8bc7), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    DB_AccountCargoResultView* sig =
        reinterpret_cast<DB_AccountCargoResultView*>(
            guard.operator->()->GetInBuffer<SIG_CREATE_ACCOUNT_CARGO_RESULT>());
    MySQL* db;
    // ORIG 0x0843ac…：mId(-0x1c)/capacity(-0x20) 比 guard(this=-0x18) 远
    // ⇒ 声明置 guard 之后（4.4 后声明占远槽）；清零语句保持在 memset
    // 调用之后（ORIG 0x0843… memset 后 `movl $0,-0x1c/-0x20`）。
    unsigned int mId;
    unsigned int capacity;
    memset(sig, 0, 0x8);
    mId = 0;
    capacity = 0;
    *stream >> mId;
    *stream >> capacity;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect * from account_cargo where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() != 0)
    {
        sig->m_result = 0x14;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    db->set_query(
        "inSert into account_cargo(m_id, capacity, money, occ_time, cargo) values(%s, %d, 0, now(),'')",
        NumberToString(mId, 0), capacity);
    if (db->exec(true) != 1)
        return 0;
    sig->m_result = 0x0;
    sig->m_capacity = capacity;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
