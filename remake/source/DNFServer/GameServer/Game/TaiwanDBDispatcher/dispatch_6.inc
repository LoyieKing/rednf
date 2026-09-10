bool DBSelectPcroomDailyReward::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        // [b28d-solo-TaiwanDBDispatcher] P171/P232：ORIG 0x0815ca9c prologue 后
        // `movb $0x1,-0x15(%ebp)` 写后从不读（dead 局部）⇒ 照抄声明。
        bool bDead = true;
        (void)bDead;
        DBSelectPcroomDailyReward::MSG* sig = stream->GetOutBuffer<DBSelectPcroomDailyReward::MSG>();
        if (sig == 0)
            throw "msg is null";
        // [b28d-solo-TaiwanDBDispatcher] P28：ORIG 0x0815cae0 三条清零在 GetOutBuffer
        // 之后（-0x4c/-0x50/-0x51）⇒ 声明落在 sig 检查之后。
        unsigned int yesterday = 0;
        unsigned int reward = 0;
        char flag = 0;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG 0x0815cb49：G_CEnvironment()+0x37c 供 set_query（不检查返回值）；
        // 0x0815cd86 再调一次供 CheckDailyScheduleTime（两次独立 G_CEnvironment）。
        db->set_query(
            "seLect m_id, remain_reward_count, unix_timestamp(occ_date) FROM pcroom_daily_reward_tw WHERE m_id=%s",
            NumberToString(*reinterpret_cast<unsigned int*>(sig), 0),
            G_CEnvironment()->m_serverEnvir.m_fatigueTime);
        if (!db->exec(1))
            throw "select_exec";
        if (db->get_n_rows() == 0)
        {
            InterSelectPcroomDailyReward::makeRequest(uid, yesterday, 4);
            cMyTrace(__PRETTY_FUNCTION__, 0x300, 0)(
                "[Taiwan, Pcroom] pcroom_daily_reward_tw get_n_row==0 m_id:%s",
                NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
        }
        else
        {
            if (!db->fetch())
                throw "fetch";
            if (!db->get_int(0, yesterday))
                throw "get_int 0";
            if (!db->get_byte(1, flag))
                throw "get_int 1";
            if (!db->get_int(2, reward))
                throw "get_int 2";
            if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                                       reward,
                                       GlobalData::s_systemTime_.getCurSec()))
            {
                InterSelectPcroomDailyReward::makeRequest(uid, yesterday, 4);
                cMyTrace(__PRETTY_FUNCTION__, 0x311, 0)(
                    "[Taiwan, Pcroom] pcroom_daily_reward_tw yesterday m_id:%s",
                    NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
            }
            else
            {
                InterSelectPcroomDailyReward::makeRequest(uid, yesterday,
                                                          (char)flag);
                cMyTrace(__PRETTY_FUNCTION__, 0x316, 0)(
                    "[Taiwan, Pcroom] pcroom_daily_reward_tw today m_id:%s, remain_reward_count:%d",
                    NumberToString(*reinterpret_cast<unsigned int*>(sig), 0),
                    (char)flag);
            }
        }
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x31c,
            "[Taiwan, Pcroom] mobile_auth_reward_tw. (error:%s)", e);
        return 0;
    }
    // [b28d-solo-TaiwanDBDispatcher] P214②：ORIG 0x0815cef7 成功路径 jmp 到函数尾
    // 共享块，`mov $0x1,%ebx` 与 catch 的 `mov $0x0,%ebx` 汇合 ⇒ return 1 在 try 外。
    return 1;
