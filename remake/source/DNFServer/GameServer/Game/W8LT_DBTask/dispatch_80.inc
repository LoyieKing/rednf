bool DB_UpdateAvatarItemLock::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 无初始化（+0x26/+0x38 直接 lea 传引用给 operator>>），带 `= 0`
    // 会多两条 movl $0 清零（42 vs 40 insns）。
    int uiId;
    int itemLockKey;
    *stream >> uiId;
    *stream >> itemLockKey;
    db->set_query(
        "upDate user_items set item_lock_key=%d where ui_id=%d",
        itemLockKey, uiId);
    if (!db->exec(true))
        return 0;
    return 1;
