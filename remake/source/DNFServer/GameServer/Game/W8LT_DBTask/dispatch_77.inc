// ============================================================================
// DB_UpdateEffectItem::dispatch —— ORIG 0x08430676（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。与 DB_UpdateCoolTimeItem 同构，SQL 列 effect_item。
bool DB_UpdateEffectItem::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO* sig =
        
        stream->GetOutBuffer<SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    char* blob = db->blob_to_str(0, reinterpret_cast<char*>(&sig->m_info), 0x194);
    db->set_query(
        "upDate charac_item_stat set effect_item='%s' where charac_no=%u",
        blob, sig->m_characNo);
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_item_stat(charac_no, effect_item) values(%u, '%s')",
            sig->m_characNo, blob);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}