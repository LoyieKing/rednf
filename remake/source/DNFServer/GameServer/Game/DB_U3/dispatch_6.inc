bool DB_InsertUnlimitSupportLog::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    SIG_INSERT_UNLIMIT_SUPPORT_LOG* sig =
        stream->GetOutBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>();

    db->set_query(
        "inSert into event_unlimitsupport_lev(m_id,occ_time,server_id,charac_no,"
        "charac_name) values(%s,now(),%d,%d,'%s')",
        NumberToString(reinterpret_cast<DBU3UnlimitIn*>(sig)->m_accId, 0),
        reinterpret_cast<DBU3UnlimitIn*>(sig)->m_serverId,
        reinterpret_cast<DBU3UnlimitIn*>(sig)->m_characNo,
        reinterpret_cast<DBU3UnlimitIn*>(sig)->m_name);
    db->exec(true);
    return true;
