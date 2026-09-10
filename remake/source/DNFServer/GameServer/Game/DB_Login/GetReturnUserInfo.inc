// ============================================================================
// GetReturnUserInfo（ORIG T 0x08410ad8）
// ============================================================================

int DB_Login::GetReturnUserInfo(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect unix_timestamp(expire_time), first_login from return_user where m_id=%s and expire_time > now()",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows == 0)
    {
        sig->m_returnUserDbFlag = false;        // +0x3f04
        return 1;
    }
    if (!db->fetch())
    {
        return 0;
    }
    if (!db->get_int(0, reinterpret_cast<int&>(sig->m_returnUserExpireTime)))  // +0x3f08
    {
        return 0;
    }
    int firstLogin = 0;
    if (!db->get_int(1, firstLogin))
    {
        return 0;
    }
    sig->m_returnUserFirstLogin = (firstLogin == 0);  // +0x3f05
    sig->m_returnUserDbFlag = true;                   // +0x3f04
    return 1;
}
