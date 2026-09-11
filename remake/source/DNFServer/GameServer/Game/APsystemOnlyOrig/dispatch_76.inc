// ============================================================================
// DB_UpdateCoolTimeItem::dispatch —— ORIG 0x08430570（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO> →
// blob_to_str(+4, 0x194)；upDate charac_item_stat set cooltime_item='%s'
// where charac_no=%u；exec 失败 或 affected==0 补 inSert（charac_no,
// cooltime_item values(%u, '%s')）；inSert exec 失败返 0；全绿返 1。
bool DB_UpdateCoolTimeItem::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    DB_ContinuousItemStatView* sig =
        (DB_ContinuousItemStatView*)
        stream->GetOutBuffer<SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    char* blob = db->blob_to_str(0, sig->m_itemData, 0x194);
    db->set_query(
        "upDate charac_item_stat set cooltime_item='%s' where charac_no=%u",
        blob, sig->m_characNo);
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_item_stat(charac_no, cooltime_item) values(%u, '%s')",
            sig->m_characNo, blob);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}