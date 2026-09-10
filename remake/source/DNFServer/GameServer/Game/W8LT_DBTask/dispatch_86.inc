bool DB_UpdateItemCountValue::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;       // -0x10（SQL %s m_id，第一读）
    unsigned int itemCnt = 0;   // -0x14（SQL %d 增量，第二读）
    stream->operator>>(mId);
    stream->operator>>(itemCnt);
    db->set_query(
        "upDate %s set item_cnt=item_cnt+%d where m_id=%s",
        get_item_count_table_name(), itemCnt, NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into %s(m_id, item_cnt) values(%s, %d)",
            get_item_count_table_name(), NumberToString(mId, 0), itemCnt);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
