// ORIG 0x0818aea2：count(delete_flag=0 角色)>=0x24 → 槽位上限错误码2。
int CREATE_QUERY::arad_sp_create_character_check(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    int rc = db->select("seLect count(*) from charac_info where m_id=%d and delete_flag=0",
                        sig->m_accId);
    if (rc != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x62, "%s", "select charac_info error");
        return -1;
    }

    int count = 0;
    if (!db->get_int(0, count) || count >= 0x24)
    {
        sig->m_resultCode = 2;                                                                                // +0x5344 直写（helper 外联消除）
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x69, "%s", "max charac slot limit");
        return -1;
    }
    return 0;
}