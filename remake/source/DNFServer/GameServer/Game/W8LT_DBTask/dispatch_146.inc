bool DB_UpdateOntimeLastRecvIdx::dispatch(int /*userIdx*/, int /*slot*/,
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;   // -0x24
    int lastNo = 0;         // -0x28
    *stream >> mId;
    *stream >> lastNo;
    unsigned int tableNo = mId % 10;   // -0xc（ORIG 0xcccccccd mul 序列）
    db->set_query("upDate event_1112_ontime_user_%u set last_no=%u where m_id=%s",
                  tableNo, lastNo, NumberToString(mId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa407, 5)(
            "upDate event_1112_ontime_user_%u set last_no=%u failed at m_id(%s)",
            tableNo, lastNo, NumberToString(mId, 0));
        return 0;
    }
    return 1;
