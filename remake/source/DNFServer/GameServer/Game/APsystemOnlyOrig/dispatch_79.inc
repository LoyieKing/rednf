// ============================================================================
// DB_UpdateCreatureItemLock::dispatch —— ORIG 0x08439aba（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。*stream >> ui_id(int) → *stream >> item_lock_key(int) →
// upDate creature_items set item_lock_key=%d where ui_id=%d；返回 exec(true)==1。
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
}