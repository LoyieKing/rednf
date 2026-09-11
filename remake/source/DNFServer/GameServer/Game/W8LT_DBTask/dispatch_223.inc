bool DB_UpdateUserFatigueQuantity::dispatch(int userIdx, int slot, Stream* stream)
{
    // ORIG 0x08441a8e：GetDBHandle(9,0) → >>u1 >>u2 → 表名 get_db_table_name(0x199)
    //   → upDate 加量（m_id 匹配）→ exec 失败返 0 → 影响行数 0 则 inSert → exec 失败返 0 → 返 1。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    unsigned int u1 = 0;
    unsigned int u2 = 0;
    stream->operator>>(u1);
    stream->operator>>(u2);
    std::string table(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_db_table_name(0x199));
    db->set_query(
        "upDate %s set fatigue_quantity=fatigue_quantity+%d where m_id=%s",
        toTString(table), u2, NumberToString(u1, 0));
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into %s(m_id, fatigue_quantity) values(%s, %d)",
            toTString(table), NumberToString(u1, 0), u2);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}