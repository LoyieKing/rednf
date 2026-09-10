bool DB_UpdateQueryCountTimerDb::dispatch(int userIdx, int slot,
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    CQueryCounter* counter = db->GetQueryCounter();
    counter->UpdateQueryCounter();
    return 1;
