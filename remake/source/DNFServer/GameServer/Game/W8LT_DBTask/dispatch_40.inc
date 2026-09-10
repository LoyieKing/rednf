bool DB_InsertQueryCounterLogdb::dispatch(int userIdx, int slot, Stream* stream)
{
    int qid = 0;
    int len = 0;
    char buf[0x6001] = { '\0' };
    *stream >> qid;
    *stream >> len;
    stream->get_binary(buf, len);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    // P0/P52：ORIG 无 counter 槽往返（GetQueryCounter 返回值直作 this）⇒ 链式。
    if (!db->GetQueryCounter()->RegisterQuery(buf, qid, 0))
        return 0;
    return 1;
