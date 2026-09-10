bool DB_InsertAccountFirstLoginFlag::dispatch(int /*userIdx*/, int /*slot*/,
                                               Stream* s)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int m_id = 0;       // -0x28：第 1 次 >>（unsigned int&）
    unsigned int characNo = 0;   // -0x2c：第 3 次 >>（unsigned int&）
    int          serverID = 0;   // -0x30：第 2 次 >>（int&）
    int          eventType;      // -0x34：第 4 次 >>（ORIG 无清零，照抄 P171）
    *s >> m_id;
    *s >> serverID;
    *s >> characNo;
    *s >> eventType;

    EventClassify::CEventScriptMng* sm = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    EventClassify::CEventEntity* ent = 0;
    if (eventType == 0x191 || eventType == 0x192)
    {
        ent = sm->get_event_entity(eventType);
        if (ent == 0)
            return 0;
    }
    else
        return 0;

    // ORIG 0x0843e744：toTString(const std::string&) 转换表名（get_db_table_name
    // 返回 &m_dbTableName，调用方按 std::string& 解释）；实参表达式内联（P0）。
    db->set_query("inSert into %s(m_id,server_id, charac_no, occ_date) "
                  "values(%s,%d, %u, now())",
                  toTString(ent->get_db_table_name()),
                  NumberToString(m_id, 0), serverID, characNo);
    if (!db->exec(true))
        return 0;
    return 1;
