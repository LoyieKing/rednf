bool DB_GenCeraTicket::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;  // ORIG _ZZN16DB_GenCeraTicket8dispatch…E9handleIdx
    int result = 0;
    MySQL* db;
    bool ok;
    unsigned int mId;
    char ticketId[32];

    *stream >> mId;
    *stream >> result;
    *stream >> ticketId;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into user_cera_ticket(m_id,ticket_id,ticketKey,expire_time,delete_flag,update_time) values(%s,'%s',%d,adddate(now(),interval %d hour), 0, now())",
        NumberToString(mId, 0), ticketId, result, 3);
    ok = db->exec(true);
    if (ok != 1)
    {
        db->set_query(
            "upDate user_cera_ticket set ticket_id='%s',ticketKey=%d,expire_time=adddate(now(),interval %d hour), delete_flag=%d, update_time=now() where m_id=%s",
            ticketId, result, 3, 0, NumberToString(mId, 0));
        ok = db->exec(true);
        if (ok != 1)
            result = 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4c6d), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator*()->operator<<(result);
    guard.operator*()->operator<<(ticketId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
