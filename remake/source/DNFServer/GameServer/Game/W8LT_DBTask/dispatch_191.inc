bool DB_SaveGoldZeroTradeLog::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_GOLD_ZERO_TRADE_LOG* sig =
        stream->GetOutBuffer<SIG_SAVE_GOLD_ZERO_TRADE_LOG>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into dnf_zeromoney_log(to_m_id,to_nat_ip,to_peer_ip,to_nat_type,from_m_id,from_nat_ip,from_peer_ip,from_nat_type,occ_time,injury_money,to_mac_addr,from_mac_addr) values(%s,'%s','%s',%d,%s,'%s','%s',%d,now(),%d,'%s','%s')",
        NumberToString(sig->m_to_m_id, 0),
        sig->m_to_nat_ip, sig->m_to_peer_ip,
        (int)sig->m_to_nat_type,
        NumberToString(sig->m_from_m_id, 1),
        sig->m_from_nat_ip, sig->m_from_peer_ip,
        (int)sig->m_from_nat_type,
        sig->m_injury_money, sig->m_to_mac_addr,
        sig->m_from_mac_addr);
    if (!db->exec(true))
        return 0;
    return 1;
