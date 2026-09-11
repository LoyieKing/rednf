bool DB_Login::_getAutoPunishedSecondUserData(unsigned int mid, unsigned int& tradeCnt,
                                              unsigned long long& tradeGold)
{
    char punishFlag = 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect trade_cnt,trade_gold,punish_flag,unix_timestamp(occ_time) from auto_punish_second_user where m_id=%s",
        NumberToString(mid, 0));
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    db->fetch();
    db->get_uint(0, tradeCnt);
    db->get_ulonglong(1, tradeGold);
    db->get_byte(2, punishFlag);
    int occTime = 0;
    db->get_int(3, occTime);    // ORIG：读入后未使用（死存储，保留语义注释）
    (void)occTime;
    return punishFlag != 0;
}