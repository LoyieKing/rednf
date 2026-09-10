bool DB_DeletePowerWarStatueMessage::dispatch(int /*userIdx*/, int /*slot*/,
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    int serverInfo = 0;   // -0x10
    *stream >> serverInfo;
    db->set_query(
        "deLete from event_server_message where server_info = %d and message_index in (1, 2, 3)",
        serverInfo);
    if (db->exec(true) != 1)
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x853d, "deLete_power_war_statue_message Query Error");
    }
    return 1;
