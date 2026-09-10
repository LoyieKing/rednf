bool DB_InsertQueryCounterDb::dispatch(int userIdx, int slot, Stream* stream)
{
    char buf[0x6001];
    int len = 0;
    int qid = 0;
    memset(buf, 0, 0x6001);
    *stream >> len;
    stream->get_binary(buf, len);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // [b28d-solo-W8LT_DBTask] ORIG 0x08425b9x：`call SaveQuery; xor $1,%eax;
    // test %al,%al; jne <mov $0; jmp>` ⇒ 原码 P101 早返 + 裸 bool 判定
    // （`==1` 比较形 4.4 反产无 xor 直判）；P0/P52 链式 GetQueryCounter
    // （ORIG 无 counter 槽往返，receiver 为表达式右值）。
    if (!db->GetQueryCounter()->SaveQuery(buf, qid))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x63c1), true);
    guard.operator*()->operator<<(0xbf);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(qid);
    guard.operator*()->operator<<(len);
    guard.operator->()->put_binary(buf, len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    return 1;
