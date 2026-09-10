bool DB_LoadPuUserCheck::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    unsigned int mId = 0;
    unsigned char flag = 0;
    *stream >> mId;
    db->set_query("seLect m_id from pu_user_list where m_id = %u", mId);
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() != 0)
        flag = 1;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbcad), true);
    guard.operator*()->operator<<(0x2e1);
    guard.operator*()->operator<<(slot);
    SIG_LOAD_PU_USER_INFO* out = guard->GetInBuffer<SIG_LOAD_PU_USER_INFO>();
    *reinterpret_cast<char*>(out) = static_cast<char>(flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
