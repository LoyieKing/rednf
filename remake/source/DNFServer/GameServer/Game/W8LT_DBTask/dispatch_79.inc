bool DB_UpdateCreatureItemLock::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    int uiId;
    int itemLockKey;
    *stream >> uiId;
    *stream >> itemLockKey;
    db->set_query(
        "upDate creature_items set item_lock_key=%d where ui_id=%d",
        itemLockKey, uiId);
    if (!db->exec(true))
        return 0;
    return 1;
