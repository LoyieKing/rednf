bool DB_LoadCerashopAddRestrictType::dispatch(int /*param1*/, int param2,
                                              Stream* stream)
{
    // [b28d-46 判词·N/N82] ORIG 0x080e1aca：354 vs 358；帧 0x50 vs 0x60 与
    // 槽号整体平移（N82/P112）⇒ 封顶。
    unsigned int mId = 0;      // -0x30
    unsigned int characNo = 0; // -0x34
    *stream >> mId;
    *stream >> characNo;

    // ---- 账号表 ----
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)1, (ENUM_SERVER_GROUP)0);
    if (db == 0)
        return false;
    db->set_query(
        "seLect ipg_no, count, next_date from account_cerashop_restrict where "
        "m_id = %u and end_date > UNIX_TIMESTAMP() limit 10", mId);
    if (!db->exec(true))
        return false;
    int nRows = db->get_n_rows();   // -0x28
    if (nRows > 0) {
        Stream* s = GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x1bc);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<(0x341);   // SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE
        guard.operator*()->operator<<(param2);
        SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* sig =
            guard.operator->()->GetInBuffer<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE>();
        *(int*)sig = nRows;               // +0x00 count
        *(int*)((char*)sig + 4) = 0;      // +0x04 type = account
        for (int i = 0; i < nRows; ++i) {
            if (!db->fetch())
                return false;
            int col = 1;   // -0x18 列号推进（0 基）
            if (!db->get_int(0, *(unsigned int*)((char*)sig + 8 + i * 0xc)))
                return false;
            col = 2;
            if (!db->get_int(1, *(unsigned int*)((char*)sig + 0xc + i * 0xc)))
                return false;
            col = 3;
            if (!db->get_int(2, *(unsigned int*)((char*)sig + 0x10 + i * 0xc)))
                return false;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }

    // ---- 角色表 ----
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    if (db2 == 0)
        return false;
    db2->set_query(
        "seLect ipg_no, count, next_date from charac_cerashop_restrict where "
        "charac_no = %u and end_date > UNIX_TIMESTAMP() limit 10", characNo);
    if (!db2->exec(true))
        return false;
    nRows = db2->get_n_rows();
    if (nRows > 0) {
        Stream* s = GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x1da);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<(0x341);
        guard.operator*()->operator<<(param2);
        SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* sig =
            guard.operator->()->GetInBuffer<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE>();
        *(int*)sig = nRows;               // +0x00 count
        *(int*)((char*)sig + 4) = 1;      // +0x04 type = charac
        for (int i = 0; i < nRows; ++i) {
            if (!db2->fetch())
                return false;
            int col = 1;
            if (!db2->get_int(0, *(unsigned int*)((char*)sig + 8 + i * 0xc)))
                return false;
            col = 2;
            if (!db2->get_int(1, *(unsigned int*)((char*)sig + 0xc + i * 0xc)))
                return false;
            col = 3;
            if (!db2->get_int(2, *(unsigned int*)((char*)sig + 0x10 + i * 0xc)))
                return false;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return true;
