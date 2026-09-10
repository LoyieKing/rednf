int EventAlttle::CheckAlreadyGetAlttleItem(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect item_check from event_alttle_info where m_id=%s", NumberToString(uid, 0));
    if (!db->exec(true))
        return 0;

    int check = 0;
    // ORIG 尾段 if 块常量岛（非三目 sete 折叠）：if/return 分离形。
    if (!db->get_int(0, check) || check == 0)
        return 0;
    return 1;
