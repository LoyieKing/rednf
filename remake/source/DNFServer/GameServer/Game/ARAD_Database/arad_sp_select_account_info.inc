int LOGIN_QUERY::arad_sp_select_account_info(SIG_LOGIN_DATA* data, char* param_2)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x1e8, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect m_id, passwd,first_ssn, second_ssn, "
            "mid(from_unixtime(reg_date)+0,1,8),user_name,email, user_id, hangame_flag "
            "from member_info where m_id=%d",
            data->m_accId) != 0)
    {
        return -1;
    }
    if (db->get_n_rows() != 1)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f5, 5)("query get_n_rows() ERROR m_id=%u", data->m_accId);
        return 0;
    }

    unsigned int fssn = 0;                    // first_ssn
    unsigned int sssn = 0;                    // second_ssn
    unsigned int regMid = 0;                  // mid(from_unixtime(reg_date))
    unsigned int hangameFlag = 0;
    // ORIG 0x818b7f0 起 col 槽 -0xc 为 0，各调用实参为 `addl $1,槽; mov 槽,%eax`
    //   ⇒ 源形统一 `++col`（先自增后读）。
    int col = 0;
    if (!db->get_uint(++col, data->m_accId))       // col1
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x201, 5)("user m_id ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, param_2, 0x84))                              // col2
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x207, 5)("user password ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, fssn))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x20d, 5)("user fssn ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, sssn))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x213, 5)("user sssn ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, regMid))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x219, 5)("user regdate ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, data->m_userDetailIpPort, 0x80))  // user_name（+0x2184）
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x21f, 5)("user user_name ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, data->m_userDetailSsn, 0x80))  // email（+0x2204）
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x225, 5)("user email ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, reinterpret_cast<char*>(data), 0x1e))           // user_id
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x22b, 5)("user id ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, hangameFlag))                                  // col9
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x231, 5)("user HanGameUser Check ERROR m_id=%u", data->m_accId);
        return 0;
    }

    data->m_regDate = regMid;
    data->m_hangameUser =
        static_cast<unsigned char>(hangameFlag == 1 ? 1 : 0);
    OS_API::snprintf(data->m_birth, 0x10, "%d%d",   // +0x2284
                     static_cast<int>(fssn), static_cast<int>(sssn));
    // ORIG 0x0818bcf1：先 fssn/=10000、sssn/=1000000（mul/shr 常量除法），
    // 再 sssn > 2 → year=fssn+2000(0x7d0)，否则 year=fssn+1900(0x76c)。
    fssn = fssn / 10000;
    sssn = sssn / 1000000;
    if (sssn > 2)
    {
        data->m_birthYear = fssn + 2000;
    }
    else
    {
        data->m_birthYear = fssn + 1900;
    }
    return 0;
