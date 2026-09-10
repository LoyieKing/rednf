bool ResetStayTimeEvent::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        db->set_query("upDate event_stay_time_charac set reward_flag = 0");
        if (!db->exec(true))
            throw "exec";
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x1ac,
            "[Taiwan, Event] reset event_stay_time_charac. (error:%s)", e);
        return 0;
    }
