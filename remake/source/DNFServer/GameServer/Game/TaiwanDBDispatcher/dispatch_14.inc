// ORIG 0x0815b76a T ResetStayTimeEvent::dispatch（报告 dispatch.md）：
// GetDBHandle(9,0) null→throw "handle null"；set_query("upDate
// event_stay_time_charac set reward_flag = 0")（不检查）→ exec 失败 throw "exec"。
// catch 0x1ac 返 0，成功返 1。
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
}
