bool DB_AvatarItemMove::ChangeSlot(int userIdx, int slot, int clearAvatarId)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    bool ok;
    if (userIdx < 0)
        return 1;
    db->set_query("upDate user_item set slot=%d,clear_avatar_id=%d where ui_id=%d",
                  slot, clearAvatarId, userIdx);
    ok = db->exec(true);
    if (!ok)
        return 0;
    return 1;
