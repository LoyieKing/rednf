// ============================================================================
// DB_AvatarOptionChange::dispatch —— ORIG 0x0844687e（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_AVATAR_OPTION_CHANGE>（+0x00 ui_id、+0x04 charac_no byte、
// +0x08 ability_no）→ GetDBHandle(DB_HANDLE_3,0) → set_query("upDate
// user_items set ability_no=%d where ui_id=%d and charac_no=%d", charac_no,
// ui_id, ability_no)；exec(true)!=1 → cMyTrace(__PRETTY_FUNCTION__,0xae5c,5)
// ("AVATAR_OPTION_CHAGE::DB_UPDATE_FAIL") 返 0；成功返 1。
// ============================================================================
bool DB_AvatarOptionChange::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    SIG_AVATAR_OPTION_CHANGE* sig =
        (
            stream->GetOutBuffer<SIG_AVATAR_OPTION_CHANGE>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set ability_no=%d where ui_id=%d and charac_no=%d",
        (unsigned int)sig->m_field4, sig->m_uiId, sig->m_characNo);
    if (db->exec(true) != 1)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xae5c, 5);
        trace("AVATAR_OPTION_CHAGE::DB_UPDATE_FAIL");
        return 0;
    }
    return 1;
}