// ============================================================================
// DB_TestTradeRestrict::dispatch @ 0x0844a0be
// mangled _ZN20DB_TestTradeRestrict8dispatchEiiP6Stream
// GetDBHandle(DB_HANDLE_3, 0) → >>charac_no(u32) → UPDATE trade_restrict=1 →
// exec 后恒返回 true。
// ============================================================================
bool DB_TestTradeRestrict::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    unsigned int charac_no = 0;
    *stream >> charac_no;
    db->set_query("upDate user_items set trade_restrict = 1 where charac_no = %u", charac_no);
    db->exec(true);
    return true;
}