// ORIG 0x8400432：ui_id<0 恒返 1；否则 UPDATE user_item 的 slot/clear_avatar_id。
// i386 cdecl 变参顺序：set_query(fmt, slot, clearAvatarId, ui_id)
// （ORIG 压栈 0x8=param3, 0xc=param4, 0x10=param2；SQL "%d,%d where ui_id=%d"）。
bool DB_AvatarItemMove::ChangeSlot(int userIdx, int slot, int clearAvatarId)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    bool ok;
    if (userIdx < 0)
        return 1;
    db->set_query("upDate user_items set slot=%d,clear_avatar_id=%d where ui_id=%d",
                  slot, clearAvatarId, userIdx);
    ok = db->exec(true);
    if (!ok)
        return 0;
    return 1;
}