bool DB_LogBuyItem::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序照 ORIG 栈槽（低→高）：buf(-0x5c) item_type(-0x50) item_count(-0x4c)
    // item_id(-0x48) charac_no(-0x44) m_id(-0x40) t(-0x3c) tm*(-0x28)
    // mktime(-0x24) hour_limit(-0x20) db(-0x1c)；cMyTrace(-0x38) 在
    // insert 失败块内具名构造。
    char buf[0xc];
    int itemType;
    unsigned int itemCount;
    unsigned int itemId;
    unsigned int characNo;
    unsigned int m_id;
    time_t t;
    struct tm* p;
    time_t m;
    int hourLimit;
    MySQL* db;
    time(&t);
    p = localtime(&t);
    hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
    if (p->tm_hour < hourLimit)
    {
        p = 0;
        t -= 0x15180;
        p = localtime(&t);
    }
    p->tm_hour = hourLimit;
    p->tm_min = 0;
    p->tm_sec = 0;
    m = mktime(p);
    m_id = 0;
    characNo = 0;
    itemId = 0;
    itemCount = 0;
    itemType = 0;
    memset(buf, 0, 0xc);
    sprintf(buf, "%04d-%02d-%02d", p->tm_year + 0x76c, p->tm_mon + 1, p->tm_mday);
    *stream >> m_id;
    *stream >> characNo;
    *stream >> itemId;
    *stream >> itemCount;
    *stream >> itemType;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    db->set_query(
        "update log_acquire_item\t\t\t\t\t\tset item_count = item_count + %d\t\t\t\t\t\twhere occ_date = '%s' and charac_no = %u and item_id = %u",
        itemCount, buf, characNo, itemId);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "insert into log_acquire_item\t\t\t\t\t\t\t(occ_date, m_id, charac_no, item_id, item_count, item_type)\t\t\t\t\t\t\tvalues ('%s', %s, %u, %u, %u, %d)",
            buf, NumberToString(m_id, 0), characNo, itemId, itemCount, itemType);
        if (!db->exec(true))
        {
            register char* midStr = NumberToString(m_id, 0);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x566, 5);
            trace("DB_LogBuyItem, exec ERROR m_id=%s charac_no=%u ItemID=%u Count=%u Type=%d",
                  midStr, characNo, itemId, itemCount, itemType);
            return 0;
        }
    }
    return 1;
