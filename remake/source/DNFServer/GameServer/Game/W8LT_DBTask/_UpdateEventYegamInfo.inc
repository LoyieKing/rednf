// _UpdateEventYegamInfo —— ORIG 0x0841a5ea（size 0x77）
// 无显式返回（ORIG 函数体仅 leave/ret，eax 残留 exec 结果；调用方按单字节消费）。
bool DB_SaveEventItem::_UpdateEventYegamInfo(int id, int type)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    if (type == 1)
    {
        db->set_query("upDate event_yegam_info set coin_check=unix_timestamp(now()) where id=%d",
                      id);
    }
    else
    {
        db->set_query("upDate event_yegam_info set item_check=unix_timestamp(now()) where id=%d and item!=0",
                      id);
    }
    db->exec(true);
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}