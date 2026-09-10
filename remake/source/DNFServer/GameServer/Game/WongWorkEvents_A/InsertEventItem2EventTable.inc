unsigned int EventCommon::InsertEventItem2EventTable(unsigned int characNo,
                                                     unsigned int itemId,
                                                     unsigned int stackCount,
                                                     unsigned int eventCode)
{
    static const char FUNC_NAME[] =
        "unsigned int WongWork::EventCommon::InsertEventItem2EventTable("
        "unsigned int, unsigned int, unsigned int, unsigned int)";
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "inSert into charac_event_items(charac_no,it_id,stack_count,event_code,"
        "reg_time) values(%d,%d,%d,%d,now())",
        characNo, itemId, stackCount, eventCode);
    if (!db->exec(true))
    {
        cMyTrace fail(FUNC_NAME, 0x25, 0);
        fail("FAIL : insert into charac_event_items(charac_no,it_id,stack_count,"
             "event_code,reg_time) values(%d,%d,%d,%d,now())",
             characNo, itemId, stackCount, eventCode);
        return 0;
    }

    cMyTrace ok(FUNC_NAME, 0x28, 0);
    ok("SUCCESS : insert into charac_event_items(charac_no,it_id,stack_count,"
       "event_code,reg_time) values(%d,%d,%d,%d,now())",
       characNo, itemId, stackCount, eventCode);
    return DBCommon::GetIdentity(db);
