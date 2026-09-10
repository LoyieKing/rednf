int LOGIN_QUERY::arad_sp_select_punish_user_info(SIG_LOGIN_DATA* data)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x255, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect punish_type, unix_timestamp(occ_time),punish_value, apply_flag, "
            "unix_timestamp(start_time),unix_timestamp(end_time) from "
            "member_punish_info where m_id=%d and apply_flag=1 LIMIT %d",
            data->m_accId, 0x32) != 0)
    {
        return -1;
    }

    for (unsigned int row = 0; row < static_cast<unsigned int>(db->get_n_rows());
         ++row)
    {
        if (!db->get_int(0, data->m_punishInfos[row].m_type))
            return -1;
        if (!db->get_uint(1, reinterpret_cast<unsigned int&>(
                                 data->m_punishInfos[row].m_field04)))
            return -1;
        if (!db->get_int(2, data->m_punishInfos[row].m_stdDropRate))
            return -1;
        if (!db->get_int(3, data->m_punishInfos[row].m_field0c))
            return -1;
        if (!db->get_uint(4, reinterpret_cast<unsigned int&>(
                                 data->m_punishInfos[row].m_field10)))
            return -1;
        if (!db->get_uint(5, reinterpret_cast<unsigned int&>(
                                 data->m_punishInfos[row].m_endTime)))
        {
            return -1;
        }
    }
    return 0;
