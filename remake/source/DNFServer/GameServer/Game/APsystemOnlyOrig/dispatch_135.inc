// ============================================================================
// DB_AvatarChangeOwner::dispatch —— ORIG 0x084004bc（报告
// docs/class_func_reports/DB_AvatarChangeOwner/dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_AVATAR_CHANGE_OWNER> → GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, 无组)。
// upDate user_items set charac_no=%u,slot=%d where ui_id=%d && charac_no=%u
// （charac_no=m_newCharacNo+0x08、slot=m_slot+0x0c byte 零扩、
// ui_id=m_uiId+0x00、where charac_no=m_characNo+0x04）；exec 成功返 1。
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
}