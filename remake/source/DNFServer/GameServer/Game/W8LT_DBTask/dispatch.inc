bool CLocalChina_DB_BreakAwayReset::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    *stream >> m_id;
    db->set_query(
        "UPDATE churn_member_info \t\t\t\t   SET accrue_cera = 0, first_reward_time = 0, last_reward_time = 0, \t\t\t\t\t   add_info = 0, luck_point = 0, last_update_time = 0, second_reward_time = 0, quest_time = 0 \t\t\t\t   WHERE m_id=%s",
        NumberToString(m_id, 0));
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localchina/localchina_DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x3ff, "[GM]CLocalChina_DB_BreakAwayReset Fail!!");
        return 0;
    }
    return 1;
