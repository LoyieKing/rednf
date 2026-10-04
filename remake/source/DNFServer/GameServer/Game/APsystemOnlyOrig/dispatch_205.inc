bool DB_SaveFeaturedGoodsIdx::dispatch(int /*userIdx*/, int /*slot*/,
                                       Stream* stream)
{
    SIG_SAVE_FEATURED_GOODSIDX* sig =
        stream->GetOutBuffer<SIG_SAVE_FEATURED_GOODSIDX>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x84…：sig(-0x20)/db(-0x1c) 双槽、无 view 具名槽 ⇒ view cast
    // 内联进实参（每字段 `-mov -0x20,%eax; mov 0xNN(%eax)` 单级读）。
    db->set_query(
        "inSert into dnf_event_entry(event_id,m_id,occ_date,server_id,charac_no,obtain_date) values(%d,%s,from_unixtime(%d),%d,%d,from_unixtime(%d))",
        sig->m_eventId,
        NumberToString(sig->m_mId, 0),
        sig->m_obtainDate,
        sig->m_serverId,
        sig->m_characNo,
        sig->m_obtainDate);
    if (!db->exec(true))
        return 0;
    return 1;
}