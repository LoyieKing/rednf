// ============================================================================
// DB_AvatarColorChange::dispatch —— ORIG 0x08446944（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_AVATAR_COLOR_CHANGE>（+0x00 charac_no、+0x04 ui_id、
// +0x08 color1 ushort、+0x0a color2 ushort）→ GetDBHandle(DB_HANDLE_3,0) →
// set_query("upDate user_items set color1=%d, color2=%d where ui_id=%d and
// charac_no=%d", color1, color2, ui_id, charac_no)；exec(true)!=1 →
// cMyTrace(__PRETTY_FUNCTION__,0xae70,5)("AVATAR_COLOR_CHANGE::DB_UPDATE_FAIL")
// 返 0；成功返 1。无基类 dispatch 前置检查、无回包。
// ============================================================================
bool DB_AvatarColorChange::dispatch(int /*userIdx*/, int /*slot*/, Stream* stream)
{
    // P89：单一视图表达式避免双局部槽（ORIG 仅 sig(-0x10) 槽）。
    DB_AvatarColorChangeView* sig =
        reinterpret_cast<DB_AvatarColorChangeView*>(
            stream->GetOutBuffer<SIG_AVATAR_COLOR_CHANGE>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set color1=%d, color2=%d where ui_id=%d and charac_no=%d",
        (unsigned int)sig->m_color1, (unsigned int)sig->m_color2,
        sig->m_uiId, sig->m_characNo);
    if (db->exec(true) != 1)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xae70, 5);
        trace("AVATAR_COLOR_CHANGE::DB_UPDATE_FAIL");
        return 0;
    }
    return 1;
}