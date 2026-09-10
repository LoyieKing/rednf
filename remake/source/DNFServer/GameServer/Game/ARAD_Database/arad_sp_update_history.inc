int LOGIN_QUERY::arad_sp_update_history(SIG_LOGIN_DATA* data, int modifyType)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x37c, "%s", "Database null!!");
        return -1;
    }

    if (db->select("seLect mousepass from member_mousepass  where m_id=%d",
                   data->m_accId) != 0)
    {
        return -1;
    }

    // b28d-50：ORIG 0x818c7f5 段 `lea -0x30(%ebp),%ebx; mov $0x9,%edx; rep stos`
    // （9 dword = 36B 内联清零）；4.4 对 memset 恒发 call（N78）⇒ 改显式零初始化
    // 复现 rep stos 与 push edi/ebx 帧形（P214③/P253，探针 /tmp/arad_probe/z1.cpp f2）。
    char oldMousePass[36] = {0};
    if (!db->get_str(0, oldMousePass, 0x24))
    {
        return -1;
    }
    if (db->select("inSert into member_mousepass_history"
                   "(occ_time, m_id, pre_mousepass, modify_type)  "
                   "values (now(),%d,'%s',%d)",
                   data->m_accId, oldMousePass,
                   modifyType) != 0)
    {
        return -1;
    }
    if (db->getAffectedRowCount() == 0)
    {
        return -1;
    }
    return 0;
