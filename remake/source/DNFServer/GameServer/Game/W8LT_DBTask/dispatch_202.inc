bool DB_RemoveFeaturedGoodsIdx::dispatch(int /*userIdx*/, int /*slot*/,
                                         Stream* stream)
{
    SIG_REMOVE_FEATURED_GOODSIDX* sig =
        stream->GetOutBuffer<SIG_REMOVE_FEATURED_GOODSIDX>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 无 view 缓存槽：逐字段重读 sig(-0x10)+具名 disp（P86/P89）。
    db->set_query(
        "deLete from dnf_event_entry where m_id=%s and event_id=%d and server_id=%d",
        NumberToString(sig->m_mId, 0),
        sig->m_eventId,
        sig->m_serverId);
    if (!db->exec(true))
        return 0;
    return 1;
}