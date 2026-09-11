// ORIG 0x0815bb8c：GetDBHandle(9,0) null → throw "handle null"；
// set_query("seLect rate, HOUR(CURTIME()), event_day FROM event_create_dnf_info
// WHERE occ_time=DATE_ADD(CURDATE(), INTERVAL HOUR(CURTIME()) HOUR)")；exec 失败
// throw "select_exec"；get_n_rows()==0 throw "get_n_rows=0"；fetch 失败 throw
// "fetch"；get_int(0/1/2) 失败 throw "get_int N"；
// InterSelectCreateDnfEventInfo::makeRequest(rate,hour,eventDay)；
// cMyTrace(0x20d)("[Taiwan, Event] select event_create_dnf_info. hour:%d rate:%d
// event_day:%d", c, a, b)；catch：getCurSec→localtime_r→makeRequest(-1,
// tm_hour, 0)，logFormat(0x219) 返 0。成功返 1。
// [b28d-solo-TaiwanDBDispatcher] AE（194/194）：原 NEAR 残差「trace 实参 callee-saved
// 对调」真因 = 实参顺序写错（真 bug）——ORIG 0x0815be02 push 序 0x8=hour/0xc=rate/
// 0x10=eventDay ⇒ 原码 (hour, rate, eventDay) 与格式串一一对应；旧写 (eventDay, rate,
// hour) 使日志 hour/event_day 两值互换。改序后 4.4 edi/esi/ebx 分配逐条复现。
bool DBSelectCreateDnfEventInfo::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        int rate = 0;
        int hour = 0;
        int eventDay = 0;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG：set_query 返回值不检查（仅 exec 检查）
        db->set_query(
            "seLect rate, HOUR(CURTIME()), event_day FROM event_create_dnf_info WHERE occ_time=DATE_ADD(CURDATE(), INTERVAL HOUR(CURTIME()) HOUR)");
        if (!db->exec(1))
            throw "select_exec";
        if (db->get_n_rows() == 0)
            throw "get_n_rows=0";
        if (!db->fetch())
            throw "fetch";
        if (!db->get_int(0, rate))
            throw "get_int 0";
        if (!db->get_int(1, hour))
            throw "get_int 1";
        if (!db->get_int(2, eventDay))
            throw "get_int 2";
        InterSelectCreateDnfEventInfo::makeRequest(rate, hour, eventDay);
        cMyTrace(__PRETTY_FUNCTION__, 0x20d, 0)(
            "[Taiwan, Event] select event_create_dnf_info. hour:%d rate:%d event_day:%d",
            // [b28d-solo-TaiwanDBDispatcher] 真 bug 修正：ORIG 0x0815be02 push 序
            // 0x8(%esp)=hour(-0x38)/0xc=rate(-0x34)/0x10=eventDay(-0x3c) ⇒ 原码实参
            // (hour, rate, eventDay)，与格式串 hour/rate/event_day 一一对应；
            // 旧写 (eventDay, rate, hour) 把两值打印互换。
            hour, rate, eventDay);
        return 1;
    }
    catch (const char* e)
    {
        int sec = GlobalData::s_systemTime_.getCurSec();
        struct tm tm;
        localtime_r((time_t*)&sec, &tm);
        InterSelectCreateDnfEventInfo::makeRequest(-1, tm.tm_hour, 0);
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x219,
            "[Taiwan, Event] select event_create_dnf_info. (error:%s)", e);
        return 0;
    }
}
