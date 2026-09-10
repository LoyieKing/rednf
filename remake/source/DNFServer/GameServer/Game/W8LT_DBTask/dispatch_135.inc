bool DB_AvatarChangeOwner::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_AVATAR_CHANGE_OWNER* sig =
        stream->GetOutBuffer<SIG_AVATAR_CHANGE_OWNER>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set charac_no=%u,slot=%d where ui_id=%d && charac_no=%u",
        sig->m_newCharacNo, (unsigned int)sig->m_slot, sig->m_uiId,
        sig->m_characNo);
    // ORIG +0x86-0x99：exec 后 xor $1/test/je 双返回（==1 恒等优化会直传 al，
    // 与 ORIG 指令形态不符，故用 != 1 分支）。
    if (db->exec(true) != 1)
        return 0;
    return 1;
