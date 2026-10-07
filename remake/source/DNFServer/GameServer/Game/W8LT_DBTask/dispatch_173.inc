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
        // [DNF-FIX 2026-10-08 c22] ORIG 0x084468c0 `movzbl 0x4; movzbl %al`：+0x04 按
        //   unsigned char 零扩展（旧码 char→unsigned int 符号扩展，>=0x80 写成负数）。
        (unsigned int)(unsigned char)sig->m_field4, sig->m_uiId, sig->m_characNo);
    if (db->exec(true) != 1)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xae5c, 5);
        trace("AVATAR_OPTION_CHAGE::DB_UPDATE_FAIL");
        return 0;
    }
    return 1;
}