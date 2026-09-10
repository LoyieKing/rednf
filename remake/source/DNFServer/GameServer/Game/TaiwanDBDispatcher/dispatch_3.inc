bool DBSelectCreateDnfUserInfo::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        DBSelectCreateDnfUserInfo::MSG* sig = stream->GetOutBuffer<DBSelectCreateDnfUserInfo::MSG>();
        if (sig == 0)
            throw "msg is null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // b28d-lane6：ORIG 0x0815c034 不检查 set_query 返回值（仅 exec 检查），
        // 删检查后消去多出的一份 throw 块。
        db->set_query(
            "seLect m_id from event_create_dnf_user where occ_date=CURDATE() and m_id=%s",
            NumberToString(sig->m_id, 0));
        if (!db->exec(1))
            throw "select_exec";
        bool received = false;
        if (db->get_n_rows() != 0)
            received = true;
        InterSelectCreateDnfUserInfo::makeRequest(uid, sig->m_id, received);
        cMyTrace(__PRETTY_FUNCTION__, 0x24d, 0)(
            "[Taiwan, Event] select event_create_dnf_user. m_id:%s isReceived:%d",
            NumberToString(sig->m_id, 0), received ? 1 : 0);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x253,
            "[Taiwan, Event] select event_create_dnf_user. (error:%s)", e);
        return 0;
    }
}
