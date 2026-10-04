// ORIG 0x0815c3b0：GetOutBuffer<MSG> null → throw "msg is null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("inSert into
// event_create_dnf_user(occ_date,m_id,server_id,occ_time) VALUES(CURDATE(),%s,
// %d,CURTIME())", NumberToString(msg->m_id), G_CEnvironment()->+0x378
// (m_serverGroup))；exec 失败 throw "insert_exec"；cMyTrace(0x281)
// ("[Taiwan, Event] insert event_create_dnf_user. m_id:%d", msg->m_id)；catch
// logFormat(0x286) 返 0。成功返 1。
// [b28d-lane6] AE（已修）：P221 临时链 + +0x378 视图内联（ORIG 0x0815c3b0 单条 mov 0x378(%eax),%ebx）。
bool DBInsertCreateDnfUserInfo::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        DBInsertCreateDnfUserInfo::MSG* sig = stream->GetOutBuffer<DBInsertCreateDnfUserInfo::MSG>();
        if (sig == 0)
            throw "msg is null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG 0x0815c480: set_query("inSert into event_create_dnf_user(occ_date,
        //   m_id,server_id,occ_time) VALUES(CURDATE(),%s,%d,CURTIME())",
        //   NumberToString(sig->m_id,0), G_CEnvironment()->+0x378)
        // b28d-lane6：+0x378 视图具名访问 + 实参内联（ORIG 0x0815c480 单条
        // `mov 0x378(%eax),%ebx` 且无 serverGroup 具名槽）。
        db->set_query(
            "inSert into event_create_dnf_user(occ_date,m_id,server_id,occ_time) "
            "VALUES(CURDATE(),%s,%d,CURTIME())",
            NumberToString(*reinterpret_cast<unsigned int*>(sig), 0),
            G_CEnvironment()->m_serverEnvir.m_serverGroup);
        if (!db->exec(1))
            throw "insert_exec";
        cMyTrace(__PRETTY_FUNCTION__, 0x281, 0)(
            "[Taiwan, Event] insert event_create_dnf_user. m_id:%d",
            *reinterpret_cast<unsigned int*>(sig));
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x286,
            "[Taiwan, Event] insert event_create_dnf_user. (error:%s)", e);
        return 0;
    }
}
