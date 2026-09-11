// ============================================================================
// DB_UpdateReturnUserFirstLogin::dispatch —— ORIG T 0x0843dffe（报告
// docs/class_func_reports/DB_UpdateReturnUserFirstLogin/dispatch.md）。
// GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32) >>expire(int) >>first_login(int) →
// INSERT；exec 失败且 getDBError()==0x426 时改 UPDATE（update 成功也返 0，
// ORIG 汇编 0x843e133 恒置 0）。
// ============================================================================
bool DB_UpdateReturnUserFirstLogin::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    int expire_time = 0;
    int first_login = 0;
    *stream >> m_id;
    *stream >> expire_time;
    *stream >> first_login;
    db->set_query(
        "inSert into return_user(m_id, expire_time, first_login) values(%s, from_unixtime(%d), %d)",
        NumberToString(m_id, 0), first_login, expire_time);
    if (!db->exec(true))
    {
        if (db->getDBError() == 0x426)
        {
            db->set_query(
                "upDate return_user set expire_time = from_unixtime(%d), first_login=%d where m_id=%s",
                first_login, expire_time, NumberToString(m_id, 0));
            if (!db->exec(true))
                return 0;
        }
        return 0;
    }
    return 1;
}