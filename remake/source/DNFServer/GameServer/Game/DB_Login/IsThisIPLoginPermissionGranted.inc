bool DB_Login::IsThisIPLoginPermissionGranted(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    unsigned int allowCnt = 0;
    sig->m_geoAllowSkipFlag = false;                 // +0xb5
    db->set_query("seLect count(*) from geo_allow where allow_ip = '%s'",
                  sig->m_blackIpCheckBuf);
    if (!db->exec(true))
    {
        // [b28d-solo-DB_Login] P221/P227：兄弟块三 trace 全表达式临时链复现 ORIG
        // 槽序（-0x3c/-0x2c/-0x1c 深→浅）。
        cMyTrace(__PRETTY_FUNCTION__, 0xbc49, 5)(
            "[QUERY_ERROR] (select from geo_allow)");
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 1;
    }
    if (!db->fetch() || !db->get_uint(0, allowCnt))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbc52, 5)(
            "[QUERY_ERROR] (fetch get_uint)");
        return 0;
    }
    if (allowCnt != 0)
    {
        sig->m_geoAllowSkipFlag = true;              // +0xb5 geo_allow 命中
    }
    db->set_query("insert into member_join_info(m_id, reg_date, ip) values(%d, now(), '%s')",
                  sig->m_accId,
                  sig->m_blackIpCheckBuf);
    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbc63, 5)(
            "[QUERY_ERROR] (insert into member_join_info)");
        return 0;
    }
    return 1;
}