int CREATE_QUERY::arad_sp_charater_name_check(MySQL* db, SIG_CREATE_CHARAC* sig,
                                              char const* dbName)
{
    int rc = db->select("seLect delete_flag from %s.charac_info where charac_name='%s'",
                        dbName, sig->m_name);
    if (rc != 0)
    {
        sig->m_resultCode = 2;                                                                                // +0x5344 直写（helper 外联消除）
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x1f, "%s", "select error");
        return -1;
    }
    int deleteFlag = 0;
    if (db->get_n_rows() != 0)
    {
        sig->m_resultCode = 2;                                                                            // +0x5344 直写（helper 外联消除）
        if (!db->get_int(0, deleteFlag))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_DB_Query.cpp",
                __PRETTY_FUNCTION__,
                0x28, "%s", "Parsing Error of Selected Field");
            return -1;
        }
        if (deleteFlag != 0)
        {
            sig->m_resultCode = 3;                                                                        // +0x5344 直写（helper 外联消除）；已删除角色名占用（可复用场景由上层定夺）
        }
        else
        {
            sig->m_resultCode = 1;                                                                        // +0x5344 直写（helper 外联消除）；在用角色名占用
        }
        return -1;
    }
    return 0;                                   // 名字不存在，可直接创建
