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
