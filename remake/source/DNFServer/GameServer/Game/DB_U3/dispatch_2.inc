// ---- DB_UpdateHappyBeanInfo::dispatch ORIG @ 0x84258ee ----
// 读流 uid(uint)/characNo(uint)/name(char* 0x1d，读而未用)；DB_HANDLE_1。
// upDate event_happybean_log set cnt=cnt+1 where occ_date=now() and m_id=%s；
// 若 exec 失败或影响行数 0，inSert into event_happybean_log(occ_date,m_id,cnt)
// values(now(),%s,1)。恒返 1。
bool DB_UpdateHappyBeanInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    unsigned int uid = 0;
    unsigned int characNo = 0;
    char name[0x1d];
    *stream >> uid;
    *stream >> characNo;
    *stream >> name;

    db->set_query(
        "upDate event_happybean_log set cnt=cnt+1 where occ_date=now() and m_id=%s",
        NumberToString(uid, 0));
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_happybean_log(occ_date,m_id,cnt) values(now(),%s,1)",
            NumberToString(uid, 0));
        db->exec(true);
    }
    return 1;
}