bool DB_InsertServerLoadStatistic::dispatch(int userIdx, int slot,
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_INSERT_SERVER_LOAD_STATISTIC* sig =
        stream->GetOutBuffer<SIG_INSERT_SERVER_LOAD_STATISTIC>();
    db->set_query(
        "inSert into log_gameserver_load_stat set occ_time=now(), server_id=%d, channel_no=%d, dbq_cnt=%d, logdbq_cnt=%d, netq_cnt=%d, packetq_cnt=%d",
        (int)sig->m_serverGroup,
        (int)sig->m_channelNo,
        (int)sig->m_dbqCnt,
        (int)sig->m_logdbqCnt,
        (int)sig->m_netqCnt,
        (int)sig->m_packetqCnt);
    if (!db->exec(true))
        return 0;
    return 1;
}