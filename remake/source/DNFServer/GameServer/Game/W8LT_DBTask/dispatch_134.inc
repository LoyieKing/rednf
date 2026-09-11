// ============================================================================
// DB_SwapCreatureItem::dispatch —— ORIG 0x0841fe0c（报告
// docs/class_func_reports/DB_SwapCreatureItem/dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_SWAP_CREATURE_ITEM> → GetDBHandle(DB_HANDLE_3, 无组)。
// m_empty1(+0x08)!=1 时：upDate creature_items set slot=%d where ui_id=%d
// （slot=m_slot2+0x10、ui_id=m_itemIdx1+0x00）；exec 失败返 0。
// m_empty2(+0x14)!=1 时：同 SQL（slot=m_slot1+0x04、ui_id=m_itemIdx2+0x0c）；
// exec 失败返 0。全过返 1。无回包。
bool DB_SwapCreatureItem::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_SWAP_CREATURE_ITEM* sig =
        stream->GetOutBuffer<SIG_SWAP_CREATURE_ITEM>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    if (sig->m_empty1 != 1)
    {
        db->set_query("upDate creature_items set slot=%d where ui_id=%d",
                      sig->m_slot2, sig->m_itemIdx1);
        if (db->exec(true) != 1)
            return 0;
    }
    if (sig->m_empty2 != 1)
    {
        db->set_query("upDate creature_items set slot=%d where ui_id=%d",
                      sig->m_slot1, sig->m_itemIdx2);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}