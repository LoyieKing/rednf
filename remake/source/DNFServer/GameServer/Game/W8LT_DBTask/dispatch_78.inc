bool DB_UpdateAvatarJewelSlot::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_UPDATE_AVATAR_JEWEL_SOCKET* sig =
        stream->GetOutBuffer<SIG_UPDATE_AVATAR_JEWEL_SOCKET>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set jewel_socket='%s' where ui_id=%d and charac_no=%d",
        db->blob_to_str(0, sig->m_emblem, 0x1e),
        sig->m_avatarKey, sig->m_characNo);
    db->exec(true);
