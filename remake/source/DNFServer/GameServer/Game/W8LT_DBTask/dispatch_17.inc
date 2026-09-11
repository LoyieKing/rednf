// ORIG 0x0844c40a sz 0xd7：GetDBHandle(13,0) 为空返 0；GetOutBuffer<
// SIG_CERASHOP_ERROR_LOG> 为空返 0；inSert into cerashop_err_log(m_id,
// charac_no,server_id,channel_no,type,err_code,goods_id,time) values(%u,%u,
// %u,%u,%d,%u,%d,now()) → 返回 exec(true)。
bool DB_LogCeraShopError::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    DB_CerashopErrorLogView* sig =
        reinterpret_cast<DB_CerashopErrorLogView*>(
            stream->GetOutBuffer<SIG_CERASHOP_ERROR_LOG>());
    if (sig == 0)
        return 0;
    db->set_query("inSert into cerashop_err_log(m_id, charac_no, server_id, channel_no, type, err_code, goods_id, time) values (%u, %u, %u, %u, %d, %u, %d, now())",
                  sig->m_mId, sig->m_characNo, sig->m_serverId,
                  sig->m_channelNo, (int)sig->m_type, sig->m_errCode,
                  sig->m_goodsId);
    return db->exec(true);
}