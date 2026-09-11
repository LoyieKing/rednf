// ORIG 0x0818c4f3：按时间窗查 member_mousepass_history 决定 Goblin 奖励标志。
int LOGIN_QUERY::arad_sp_check_goblin_reward(SIG_LOGIN_DATA* data, char* sinceTime)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x30d, "%s", "Database null!!");
        return -1;
    }

    // ORIG 未检查 set_query 返回值，直接 exec
    db->set_query("seLect m_id from member_mousepass_history where modify_type = %d "
                  "and m_id=%d and occ_time >= '%s'",
                  0x10, data->m_accId, sinceTime);
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            799, "%s", "arad_sp_check_goblin_reward fail");
        return -1;
    }
    // ORIG 0x0818c5e7-0x0818c5f1：`call get_n_rows; test %eax,%eax; mov...` ——
    // 单次调用后紧跟无效 test（Z 标志随后被 mov 覆盖，无分支消费），即原码空体
    // if(get_n_rows()){} 的 O0 残留（探针实证 if(x){} 恒出 call+test）。存储无条件。
    if (db->get_n_rows()) {}
    // ORIG 0x0818c4f3+0x??：`movb $0x1,0x38e0(%data)` 直写——0x38e0 即
    // SIG_LOGIN_DATA::m_passPadEnable（GoblinPad 启用标志，真名具名化；
    // login_field helper 在此 GCC 不内联反产外联 call，改用具名字段直写）。
    data->m_passPadEnable = true;
    data->m_passPadVersion = 2;
    return 0;
}