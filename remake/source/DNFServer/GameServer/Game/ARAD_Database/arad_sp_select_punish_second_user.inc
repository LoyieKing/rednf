// ORIG 0x0818bfb0：auto_punish_second_user 单行；punish_flag 字节为 0 即失败。
int LOGIN_QUERY::arad_sp_select_punish_second_user(SIG_LOGIN_DATA* data)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x293, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect trade_cnt,trade_gold,punish_flag,unix_timestamp(occ_time) from "
            "auto_punish_second_user where m_id=%d",
            data->m_accId) != 0)
    {
        return -1;
    }

    int occTime = 0;                           // ORIG select 前双预清槽（-0x10/-0x11）
    char punishFlag = 0;                       // ORIG 不回写 sig，仅做判定
    db->get_uint(0, data->m_totalHackUserTradeCnt);           // trade_cnt
    // trade_gold：ORIG 按 8 字节 long 槽读取（覆盖 m_totalHackUserTradeGold 与高位槽）
    db->get_ulonglong(
        1, *reinterpret_cast<unsigned long long*>(&data->m_totalHackUserTradeGold));
    db->get_byte(2, punishFlag);
    db->get_int(3, occTime);
    if (punishFlag != 0)
        return 0;
    return -1;
}