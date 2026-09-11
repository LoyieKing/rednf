// ----------------------------------------------------------------------------
// EventCommon::InsertEventItem2EventTable —— ORIG T 0x083f9b54 (size 0x127)
// db handle idx=3；成功记录 trace 并返回 @@identity，失败返回 0。
// ----------------------------------------------------------------------------
unsigned int EventCommon::InsertEventItem2EventTable(unsigned int characNo,
                                                     unsigned int itemId,
                                                     unsigned int stackCount,
                                                     unsigned int eventCode)
{
    // [Q8 2026-09-11 负结论] 手写串改为 `__PRETTY_FUNCTION__` **不可行**：
    // 4.4 对**类内 static 成员**的宏输出带 `static ` 前缀（实测串 0x7e vs
    // ORIG 0x8c5de40 的 0x77，差 7 = "static "），把 trace 日志文本改坏（语义轴）
    // ⇒ 回滚。且本 TU 四数 1/21/0/0/0 改前改后**完全相同**（该 prett 符号不进
    // missing_in_game 统计）⇒ 净收益 0。手写串（与 ORIG 逐字节相同）才是正确形。
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
}