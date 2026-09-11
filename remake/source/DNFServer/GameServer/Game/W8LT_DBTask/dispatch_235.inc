bool DB_SavePcRoomPlayTimeEvent::dispatch(int userIdx, int slot,
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_PC_ROOM_PLAY_TIME* sig =
        stream->GetOutBuffer<SIG_SAVE_PC_ROOM_PLAY_TIME>();
    std::string tableName(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_db_table_name(0x5c));

    db->set_query(
        "upDate %s set play_time = %u, reward_cnt = %u, occ_date = now() where m_id = %s",
        toTString(tableName), sig->m_totalTime, (unsigned int)sig->m_rewardCnt,
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb25b,
            "<PCROOM_PLAY_TIME_EVENT> update_error m_id(%d), total_time(%d), reward_cnt(%d)",
            sig->m_accId, sig->m_totalTime, (unsigned int)sig->m_rewardCnt);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into %s(m_id, play_time, reward_cnt, occ_date) values(%s,%u,%u,now())",
            toTString(tableName), NumberToString(sig->m_accId, 0),
            sig->m_totalTime, (unsigned int)sig->m_rewardCnt);
        if (!db->exec(true))
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0xb266,
                "<PCROOM_PLAY_TIME_EVENT> insert_error m_id(%d), total_time(%d), reward_cnt(%d)",
                sig->m_accId, sig->m_totalTime, (unsigned int)sig->m_rewardCnt);
            return 0;
        }
    }
    return 1;
}