// ORIG 0x084225a8：*stream >> channelNo(-0x20) >> dbq(-0x24) →
// GetDBHandle(DB_HANDLE_4,0) → set_query("inSert into dnf_dbq_log(occ_time,
// channel_no,dbq) values(NOW(),%d,%d)", dbq, channelNo)（ORIG 实参序即
// dbq 在前 channelNo 在后，与 SQL 占位序相反）→ exec(true) 失败
// cMyTrace(0x5ad8,5) "DB_InsertQueueSizeLog, exec ERROR" 返 0；成功返 1。
bool DB_InsertQueueSizeLog::dispatch(int userIdx, int slot, Stream* stream)
{
    int channelNo = 0;
    int dbq = 0;
    // ORIG 0x08422…：channelNo(-0x20)/dbq(-0x24) 远槽、trace 临时对象
    // 复用近槽 -0x1c（CountOfHumanCertify 同法：P37 临时形翻槽序）。
    *stream >> channelNo;
    *stream >> dbq;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into dnf_dbq_log(occ_time,channel_no,dbq) values(NOW(),%d,%d)",
        dbq, channelNo);
    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x5ad8, 5)(
            "DB_InsertQueueSizeLog, exec ERROR");
        return 0;
    }
    return 1;
}