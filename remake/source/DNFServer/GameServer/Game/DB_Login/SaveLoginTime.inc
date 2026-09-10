// ============================================================================
// SaveLoginTime（ORIG T 0x084148a6）
// ============================================================================

void DB_Login::SaveLoginTime(memberIdentificationNumber_t mid, int loginTime, char* ip)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    char table[20];                          // ORIG 槽序：table 深于 db/trace（-0x30/-0x1c/-0xc）
    memset(table, 0, sizeof(table));
    memcpy(table, "member_login", 0xd);      // ORIG 原样拷贝（含终结符）
    db->set_query(
        "inSert into %s(m_id, login_time, login_ip) values(%s, %d, '%s') on duplicate key upDate login_time=%d, login_ip='%s'",
        table, NumberToString(mid, 0), loginTime, ip, loginTime, ip);
    if (!db->exec(true))
    {
        register char* errId = NumberToString(mid, 0);   // P87：ORIG ctor 前载 %ebx 跨调用
        cMyTrace(__PRETTY_FUNCTION__, 0x3bd1, 0)(
            "DB_Login::SaveLoginTime, inSert ERROR m_id=%s, logintime=%d, ip=%s",
            errId, loginTime, ip);
    }
}
